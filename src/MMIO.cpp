#include "MMIO.h"

#include <algorithm>
#include <string>
#include "Portmap.h"

using namespace std;
using namespace tlm;
using namespace sc_core;
using namespace sc_dt;

MMIO::MMIO(sc_core::sc_module_name ModuleName) : sc_core::sc_module(ModuleName), m_Socket("SocketInMMIO"), m_DftModule("DFT"), m_DftClok("MClock", sc_core::sc_time(10,SC_NS))
{
	// Initialize registers to zero
    std::fill(m_Registers, m_Registers + SIZE, 0);
    InitRegisters();

    // Register callbacks for incoming interface method calls
    m_Socket.register_nb_transport_fw(this, &MMIO::nb_transport_fw);
    
    // Register threads
    SC_THREAD(ProcessRequests);

    // Bind ADC ports
    m_DftModule.Mclock(m_DftClok);
    m_DftModule.reset(m_DftReset);
    m_DftModule.start(m_DftStart);
    m_DftModule.read(m_DftRead);
    m_DftModule.take_settings(m_DftTakeSettings);
    m_DftModule.period(m_DftPeriod);
    m_DftModule.num_of_samples(m_DftNumOfSamples);
    m_DftModule.busy(m_DftBusyFlag);
    m_DftModule.no_more_samples(m_DftNoMoreSamplesFlags);
    m_DftModule.coeff(m_DftCoeff);
}

MMIO::~MMIO()
{
    // Enable this to memory dump
    #if 0
    cout << "==================== Init memory dump ====================" << endl;
    for(unsigned int Addr = 0; Addr < SIZE; Addr++)
    {
        cout << ">>> MMIO[0x" << hex << (Addr + ADC_BASE_ADDR) << "] = " << dec << m_Registers[Addr] << endl;
    }
    cout << "==================== End memory dump  ====================" << endl;
    #endif
}

tlm::tlm_sync_enum MMIO::nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay )
{
	ID_extension* id_extension = nullptr;
    unsigned char* byt = trans.get_byte_enable_ptr();
	trans.get_extension( id_extension ); 
	if(phase == BEGIN_REQ && id_extension != nullptr)
	{
        // Obliged to check the transaction attributes for unsupported features
        // and to generate the appropriate error response
        if (byt != 0) 
        {
            trans.set_response_status( TLM_BYTE_ENABLE_ERROR_RESPONSE );
            return TLM_COMPLETED;
        }

        TransRequest NewRequest;
        NewRequest.m_Id 		= id_extension->m_TransactionId;
        NewRequest.m_Cmd 		= trans.get_command();
        NewRequest.m_DataPtr 	= trans.get_data_ptr();
        NewRequest.m_DataLenght = trans.get_data_length();
        NewRequest.m_Addr 		= trans.get_address();
		m_RequestQueue.push(NewRequest);
		
		//Delay
		wait(delay);
		m_NewRequestEvent.notify();
		cout << name() << " BEGIN_REQ RECEIVED" << " TRANS ID " << dec << NewRequest.m_Id << " at time " << sc_time_stamp() << endl;
		return TLM_ACCEPTED;
	}
	return TLM_COMPLETED;
}

void MMIO::ProcessRequests()
{
	//const sc_time process_request_delay = sc_time(10, SC_NS);
	tlm_generic_payload trans;
	tlm_phase phase = BEGIN_RESP;
	sc_time trans_delay = sc_time(10, SC_NS);
    ID_extension* id_extension = new ID_extension();
    trans.set_extension( id_extension );

	while(true)
	{
		// Wait for an event to pop out of the back end of the queue   
        if(!m_RequestQueue.empty())
        {
            TransRequest request = m_RequestQueue.front();     
            m_RequestQueue.pop();
            
            id_extension->m_TransactionId = request.m_Id;
            trans.set_command(request.m_Cmd);
            trans.set_data_ptr(request.m_DataPtr);
            trans.set_data_length(request.m_DataLenght);
            trans.set_address(request.m_Addr);
            
            tlm::tlm_command cmd = trans.get_command();   
            sc_dt::uint64    adr = trans.get_address();   
            unsigned char*   ptr = trans.get_data_ptr();   
            unsigned int     len = trans.get_data_length();
            if (adr < DFT_BASE_ADDR || len > sizeof(unsigned int))   
            	SC_REPORT_ERROR("TLM2", "Target does not support given generic payload transaction");

            RegisterStatus reg_stat = IsCommandSupportedByRegister(adr, cmd);
            if(RegisterStatus::CMD_SUPPORTED == reg_stat)
            {
                if ( cmd == tlm::TLM_READ_COMMAND )
                {
                    ExecuteRegisterAction(adr);
                    memcpy(ptr, &m_Registers[adr-DFT_BASE_ADDR], len);
                }
                else if ( cmd == tlm::TLM_WRITE_COMMAND )
                {
                    memcpy(&m_Registers[adr-DFT_BASE_ADDR], ptr, len);
                    ExecuteRegisterAction(adr);
                }
            
                // Obliged to set response status to indicate successful completion   
                trans.set_response_status( tlm::TLM_OK_RESPONSE );
                std::cout << name() << " BEGIN_RESP SENT" << " TRANS ID " << std::dec << id_extension->m_TransactionId <<  " at time " << sc_time_stamp() << std::endl;
                // Call on backward path to complete the transaction  
                m_Socket->nb_transport_bw( trans, phase, trans_delay ); 
            }
            else
            {
                std::string error = RegisterStatus::WRITE_NOT_SUPPORTED == reg_stat? "Register does not support write command" : "Register does not support read command";
                SC_REPORT_ERROR("TLM2", error.c_str());
            }
  
        }
        else
        {
            wait(m_NewRequestEvent); 
        }
	}
}

void MMIO::ExecuteRegisterAction(unsigned int Addr)
{
    const sc_time delay = sc_time(10, SC_NS);
    unsigned int AddrWithOutOffset = Addr - DFT_BASE_ADDR;
    switch(AddrWithOutOffset)
    {
        case DFT_RESET:
            DftReset_t RegRst;
            RegRst.AllBits = m_Registers[AddrWithOutOffset];
            if(0 != RegRst.Reset)
            {
                m_DftReset = true;
                wait(delay);
                m_DftReset = false;
            }
            break;
        case DFT_CTRL1:
            DftCtrl1_t RegCtrl1;
            RegCtrl1.AllBits = m_Registers[AddrWithOutOffset];
            m_DftTakeSettings = true;
            m_DftPeriod = RegCtrl1.Period;
            m_DftNumOfSamples = RegCtrl1.NumofSamples;
            wait(delay);
            m_DftTakeSettings = false;
            break;
        case DFT_CTRL2:
            DftCtrl2_t RegCtrl2;
            RegCtrl2.AllBits = m_Registers[AddrWithOutOffset];
            if(0 != RegCtrl2.Start)
            {
                m_DftStart = true;
                wait(delay);
                m_DftStart = false;
            }
            break;
        case DFT_RAM_DATA:
            DftRamData_t RegRam;
            m_DftRead = true;
            wait(delay);
            m_DftRead = false;
            RegRam.Sample = m_DftCoeff.read();
            m_Registers[AddrWithOutOffset] = RegRam.Sample;
            break;
        default:
            break;
    }
}

MMIO::RegisterStatus MMIO::IsCommandSupportedByRegister(unsigned int Addr, tlm::tlm_command Cmd)
{
    RegisterStatus Status = RegisterStatus::CMD_SUPPORTED;
    unsigned int AddrWithOutOffset = Addr - DFT_BASE_ADDR;
    switch(AddrWithOutOffset)
    {
        case DFT_RESET:
            Status = (TLM_WRITE_COMMAND == Cmd) ? RegisterStatus::CMD_SUPPORTED : RegisterStatus::READ_NOT_SUPPORTED;
            break;
        case DFT_CTRL1:
            Status = (TLM_WRITE_COMMAND == Cmd) ? RegisterStatus::CMD_SUPPORTED : RegisterStatus::READ_NOT_SUPPORTED;
            break;
        case DFT_CTRL2:
            Status = (TLM_WRITE_COMMAND == Cmd) ? RegisterStatus::CMD_SUPPORTED : RegisterStatus::READ_NOT_SUPPORTED;
            break;
        case DFT_RAM_DATA:
            Status = (TLM_READ_COMMAND == Cmd) ? RegisterStatus::CMD_SUPPORTED : RegisterStatus::WRITE_NOT_SUPPORTED;
            break;
        default:
            break;
    }
    return Status;
}

void MMIO::InitRegisters()
{
    m_Registers[DFT_RESET] = 0;
    m_Registers[DFT_CTRL1] = 0x000190001;
    m_Registers[DFT_CTRL2] = 0;
    m_Registers[DFT_RAM_DATA] = 0;
}