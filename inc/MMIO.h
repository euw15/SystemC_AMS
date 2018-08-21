#ifndef MMIO_H_
#define MMIO_H_

#include "systemc.h"
#include "systemc-ams.h"

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "UtilCommon.h"
#include "ADC.h"

#include <queue>

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
	int m_Registers[SIZE];

	ADC m_AdcModule;
    sc_core::sc_clock m_AdcClok;
    
    // ADC In ports
	sc_signal<bool> m_AdcReset;
	sc_signal<bool> m_AdcStart;
	sc_signal<bool> m_AdcRead;
	sc_signal<bool> m_AdcTakeSettings;
	sc_signal<sc_uint<14>> m_AdcPeriod;
	sc_signal<sc_uint<13>> m_AdcNumOfSamples;

	// ADC Out ports
	sc_signal<bool> m_AdcBusyFlag;
	sc_signal<bool> m_AdcNoMoreSamplesFlags;
	sc_signal<sc_uint<26>> m_AdcSample;
	void ExecuteRegisterAction(unsigned int Addr, unsigned int Data);
	bool IsAddrAndCommandOk(unsigned int Addr, tlm::tlm_command Cmd);
	void InitRegisters();
};

#endif