#include "CPU.h"

#include "UtilCommon.h"
#include "Portmap.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace tlm;
using namespace sc_core;

const size_t CPU::kTotalTransactions = 1000;

CPU::CPU(sc_core::sc_module_name ModuleName) : sc_core::sc_module(ModuleName), m_Socket("SocketOutCPU"), m_TransVectorData(kTotalTransactions, 0), m_TransStatus(kTotalTransactions, false)
{
	// Register callbacks for incoming interface method calls
   	m_Socket.register_nb_transport_bw(this, &CPU::nb_transport_bw);

   	// Register Threads
    SC_THREAD(GenerateRequests);

    // Print creation
    cout << name() << " init at time " << sc_time_stamp() << endl;
}

CPU::~CPU()
{

}

void CPU::GenerateRequests()
{

	const sc_time delay_between_trans_creation = sc_time(10, SC_NS);
	tlm_generic_payload trans;
    tlm_phase phase = BEGIN_REQ;
	sc_time trans_delay = sc_time(10, SC_NS);
	ID_extension* id_extension = new ID_extension();
	trans.set_extension( id_extension );

	const unsigned int c_iNumOfSamples = 100;	
	const unsigned int c_iPeriod = 1;			// MClock of DFT is 100MHz so (50MHz)
	const unsigned int c_iNumOfCoeffs = c_iNumOfSamples * 2;
	unsigned int CoeffBuffer[c_iNumOfCoeffs];

	// call reset register
	{
		wait(delay_between_trans_creation);

		DftReset_t reg = {0};
		reg.Reset = 1;
		m_TransVectorData[id_extension->m_TransactionId] = reg.AllBits;
		tlm_command cmd = TLM_WRITE_COMMAND;
		sc_dt::uint64 addr = DFT_BASE_ADDR + DFT_RESET;
		unsigned char* data_ptr = reinterpret_cast<unsigned char*>(&m_TransVectorData[id_extension->m_TransactionId]);

		trans.set_command( cmd );
		trans.set_address( addr );
		trans.set_data_ptr( data_ptr );   
		trans.set_data_length( sizeof(unsigned int) );

		cout << ">>>  " << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
		m_Socket->nb_transport_fw( trans, phase, trans_delay );

		// Increment transaction Id for next transaction
		id_extension->m_TransactionId++;
	}

	// call dft ctrl 1 register to set period and number of samples
	{
		wait(delay_between_trans_creation);

		DftCtrl1_t reg = {0};
		reg.Period = c_iPeriod;
		reg.NumofSamples = c_iNumOfSamples;
		m_TransVectorData[id_extension->m_TransactionId] = reg.AllBits;
		tlm_command cmd = TLM_WRITE_COMMAND;
		sc_dt::uint64 addr = DFT_BASE_ADDR + DFT_CTRL1;
		unsigned char* data_ptr = reinterpret_cast<unsigned char*>(&m_TransVectorData[id_extension->m_TransactionId]);

		trans.set_command( cmd );
		trans.set_address( addr );
		trans.set_data_ptr( data_ptr );   
		trans.set_data_length( sizeof(unsigned int) );

		cout << ">>>  " << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
		m_Socket->nb_transport_fw( trans, phase, trans_delay );

		// Increment transaction Id for next transaction
		id_extension->m_TransactionId++;
	}

	// call dft ctrl 2 register to start dft capture and calculaction sequence
	{
		wait(delay_between_trans_creation);

		DftCtrl2_t reg = {0};
		reg.Start = 1;
		m_TransVectorData[id_extension->m_TransactionId] = reg.AllBits;
		tlm_command cmd = TLM_WRITE_COMMAND;
		sc_dt::uint64 addr = DFT_BASE_ADDR + DFT_CTRL2;
		unsigned char* data_ptr = reinterpret_cast<unsigned char*>(&m_TransVectorData[id_extension->m_TransactionId]);

		trans.set_command( cmd );
		trans.set_address( addr );
		trans.set_data_ptr( data_ptr );   
		trans.set_data_length( sizeof(unsigned int) );

		cout << ">>>  " << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
		m_Socket->nb_transport_fw( trans, phase, trans_delay );

		// wait until we get a response that the start sequence was ok
		WaitUntilResponse(id_extension->m_TransactionId);

		// Increment transaction Id for next transaction
		id_extension->m_TransactionId++;
	}

	// Read Coeffs
	{
		// wait until the DFT ends its calculation sample
		sc_time dft_calc_delay = sc_time(40, SC_US);	// (MClock) T * Period * 2 * Num of Samples
		wait(dft_calc_delay);

		tlm_command cmd = TLM_READ_COMMAND;
		sc_dt::uint64 addr = DFT_BASE_ADDR + DFT_RAM_DATA;

		trans.set_command( cmd );
		trans.set_address( addr );
		trans.set_data_length( sizeof(unsigned int) );

		for(unsigned int iCoeffCount = 0; iCoeffCount < c_iNumOfCoeffs; iCoeffCount++)
		{
			wait(delay_between_trans_creation);

			unsigned char* data_ptr = reinterpret_cast<unsigned char*>(&m_TransVectorData[id_extension->m_TransactionId]);
			trans.set_data_ptr( data_ptr ); 

			cout << ">>>  " << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
			m_Socket->nb_transport_fw( trans, phase, trans_delay );

			// wait until we get a respond to the read command
			WaitUntilResponse(id_extension->m_TransactionId);

			DftRamData_t reg;
			reg.AllBits = m_TransVectorData[id_extension->m_TransactionId];
			CoeffBuffer[iCoeffCount] = reg.Sample;

			// Increment transaction Id for next transaction
			id_extension->m_TransactionId++;
		}
	}

	// Store Coeffs on main memory
	{
		tlm_command cmd = TLM_WRITE_COMMAND;
		trans.set_command( cmd );
		trans.set_data_length( sizeof(unsigned int) );

		for(unsigned int iCoeffCount = 0; iCoeffCount < c_iNumOfCoeffs; iCoeffCount++)
		{
			wait(delay_between_trans_creation);
			sc_dt::uint64 addr = iCoeffCount;
			m_TransVectorData[id_extension->m_TransactionId] = CoeffBuffer[iCoeffCount];
			unsigned char* data_ptr = reinterpret_cast<unsigned char*>(&m_TransVectorData[id_extension->m_TransactionId]);
			trans.set_address( addr );
			trans.set_data_ptr( data_ptr );  

			cout << ">>>  " << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
			m_Socket->nb_transport_fw( trans, phase, trans_delay );

			// Increment transaction Id for next transaction
			id_extension->m_TransactionId++;
		}
	}
}

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
	ID_extension* id_extension = nullptr;
	trans.get_extension( id_extension );
    tlm_command cmd = trans.get_command();   
    sc_dt::uint64 adr = trans.get_address();   
    
    if (phase == BEGIN_RESP) 
    {                
		// Initiator obliged to check response status   
		if (trans.is_response_error() )   
			SC_REPORT_ERROR("TLM2", "Response error from nb_transport");
		
		// Delay
		wait(delay);
		m_TransStatus[id_extension->m_TransactionId] = true;	//make transaction as received
		cout << "<<<  " << name() << " BEGIN_RESP RECEIVED" << " TRANS ID " << id_extension->m_TransactionId << ", trans/bw = { " << (cmd ? 'W' : 'R') << ", 0x" << hex << adr << " } , data = " << dec << m_TransVectorData[id_extension->m_TransactionId] << " at time " << sc_time_stamp() << endl;
		return TLM_ACCEPTED; 
    }
    return TLM_COMPLETED;
}

void CPU::WaitUntilResponse(unsigned int TransactionId)
{
	const sc_time delay = sc_time(10, SC_NS);
	while(!m_TransStatus[TransactionId])
	{
		wait(delay);
	}
}
