#include "systemc.h"
#include "systemc-ams.h"

#include "TopModule.h"

int sc_main(int argc, char* argv[])
{
	TopModule MyTop("Top");
	sc_start(10, SC_MS);
	return 0;
}