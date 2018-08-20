#ifndef TOPMODULE_H_
#define TOPMODULE_H_

#include "systemc.h"

#include "CPU.h"
#include "Router.h"
#include "Memory.h"
#include "MMIO.h"

#include <memory>

class TopModule : public sc_core::sc_module
{
public:

	SC_CTOR(TopModule)
	{
		// Instantiate components   
		mp_Cpu = std::make_unique<CPU>("CPU");
		mp_Router = std::make_unique<Router>("Router");
		mp_Mem = std::make_unique<Memory>("Memory");
		mp_MMIO = std::make_unique<MMIO>("MMIO");
		
		// Bind sockets
		mp_Cpu->m_Socket.bind(mp_Router->m_SocketIn);
		mp_Router->m_SocketOutMEM.bind(mp_Mem->m_Socket);
		mp_Router->m_SocketOutMMIO.bind(mp_MMIO->m_Socket);
	}
	
	virtual ~TopModule()
	{

	};

	std::unique_ptr<Memory> mp_Mem;
	std::unique_ptr<CPU> mp_Cpu;
	std::unique_ptr<Router> mp_Router;
	std::unique_ptr<MMIO> mp_MMIO;
};

#endif