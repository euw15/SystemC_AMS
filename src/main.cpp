#include "systemc.h"
#include "systemc-ams.h"

#include "TopModule.h"

int sc_main(int argc, char* argv[])
{
    TopModule top("TopModule");
    sc_start();
	return 0;
}