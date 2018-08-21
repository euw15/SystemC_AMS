#include "systemc.h"
#include "systemc-ams.h"

#include "TopModule.h"

int sc_main(int argc, char* argv[])
{
	TopModule MyTop("Top");
	sc_start(10, SC_US);
/*    //TopModule top("TopModule");
    sc_core::sc_clock MyClock("AClock", sc_core::sc_time(10,SC_NS));
    
    // in ports
	sc_signal<bool> s_reset;
	sc_signal<bool> s_start;
	sc_signal<bool> s_read;
	sc_signal<bool> s_take_settings;
	sc_signal<sc_uint<14>> s_period;
	sc_signal<sc_uint<13>> s_num_of_samples;

	// Out ports
	sc_signal<bool> s_busy;
	sc_signal<bool> s_no_more_samples;
	sc_signal<sc_uint<26>> s_sample;
	
	ADC myadc("Adc");

	myadc.Mclock(MyClock);
	myadc.reset(s_reset);
	myadc.start(s_start);
	myadc.read(s_read);
	myadc.take_settings(s_take_settings);
	myadc.period(s_period);
	myadc.num_of_samples(s_num_of_samples);
	
	// Out ports
	myadc.busy(s_busy);
	myadc.no_more_samples(s_no_more_samples);
	myadc.sample(s_sample);
	
    sc_start(10, sc_core::SC_NS);
    s_reset = true;
    sc_start(10, sc_core::SC_NS);
    s_reset = false;
    s_take_settings = true;
    s_period = 1;
    s_num_of_samples = 100;
    sc_start(10, sc_core::SC_NS);
    s_take_settings = false;
    s_start = true;
    sc_start(10, sc_core::SC_NS);
    s_start = false;
    sc_start(10*101,sc_core::SC_NS);
    s_read = true;
    for(unsigned int iSampleIdx = 0; iSampleIdx < 100; iSampleIdx++)
    {
    	sc_start(10, sc_core::SC_NS);
    	std::cout << UndigitizeRead(s_sample.read()) << std::endl;
    }*/
	return 0;
}