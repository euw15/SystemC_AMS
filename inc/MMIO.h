#ifndef MMIO_H_
#define MMIO_H_

#include "systemc.h"
#include "systemc-ams.h"

#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "UtilCommon.h"
#include "TdfWaves.h"

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
  	void ProcessAms();

  	// Attributes
	enum{ SIZE = 100};
  	tlm_utils::simple_target_socket<MMIO> m_Socket;

private:
  	sc_core::sc_event m_NewRequestEvent;
	std::queue<TransRequest> m_RequestQueue;
	int m_Registers[SIZE];
};

#endif