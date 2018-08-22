#ifndef MMIO_H_
#define MMIO_H_

#include "systemc.h"
#include "systemc-ams.h"

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "UtilCommon.h"
#include "DFT.h"

#include <queue>
#include <utility>

class MMIO : public sc_core::sc_module
{
public:
	SC_HAS_PROCESS(MMIO);

	MMIO(sc_core::sc_module_name ModuleName);
	virtual ~MMIO();
	
	// Callbacks
  	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay);
  	
  	// Threads
  	void ProcessRequests();

  	// Attributes
	enum{ SIZE = 100};
  	tlm_utils::simple_target_socket<MMIO> m_Socket;

private:

  	sc_core::sc_event m_NewRequestEvent;
	std::queue<TransRequest> m_RequestQueue;
	unsigned int m_Registers[SIZE];

	
	DFT m_DftModule;
    
    // ADC In ports
    sc_core::sc_clock m_DftClok;
	sc_signal<bool> m_DftReset;
	sc_signal<bool> m_DftStart;
	sc_signal<bool> m_DftRead;
	sc_signal<bool> m_DftTakeSettings;
	sc_signal<sc_uint<14>> m_DftPeriod;
	sc_signal<sc_uint<13>> m_DftNumOfSamples;

	// ADC Out ports
	sc_signal<bool> m_DftBusyFlag;
	sc_signal<bool> m_DftNoMoreSamplesFlags;
	sc_signal<sc_uint<26>> m_DftSample;

	enum class RegisterStatus
	{
		WRITE_NOT_SUPPORTED,
		READ_NOT_SUPPORTED,
		CMD_SUPPORTED
	};

	void ExecuteRegisterAction(unsigned int Addr);
	RegisterStatus IsCommandSupportedByRegister(unsigned int Addr, tlm::tlm_command Cmd);
	void InitRegisters();



};

#endif