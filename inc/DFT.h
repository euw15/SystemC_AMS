#ifndef DFT_H_
#define DFT_H_

#include "systemc.h"
#include "systemc-ams.h"
#include "TdfWaves.h"
#include "filter.h"

#include <vector>

#define DFT_DEFAULT_NUM_OF_SAMPLES 	100
#define DFT_DEFAULT_PERIOD 			1

#define WAVE_SIN1_AMPL                   5.0     
#define WAVE_SIN1_FREQ                   4000000 // 4MHz

#define WAVE_SIN2_AMPL                   5.0
#define WAVE_SIN2_FREQ                   20000000 // 20MHz

SC_MODULE(WaveAdder)
{
public:
	sc_in<double> In1;
	sc_in<double> In2;
	sc_out<double> Out;

	void Process()
	{
		Out.write(In1.read() + In2.read());
	}

	SC_CTOR(WaveAdder)
	{
		SC_METHOD(Process);
			sensitive << In1;
			sensitive << In2;
	}
};

SC_MODULE(DFT)
{
public:
	enum{RAM_SIZE = 8192};

	// In ports
	sc_in<bool> Mclock;
	sc_in<bool> reset;
	sc_in<bool> start;
	sc_in<bool> read;
	sc_in<bool> take_settings;
	sc_in<sc_uint<14>> period;
	sc_in<sc_uint<13>> num_of_samples;

	// Out ports
	sc_out<bool> busy;
	sc_out<bool> no_more_samples;
	sc_out<sc_uint<32>> coeff;

	void Process();

	SC_CTOR(DFT) : 	m_WaveAdder("WaveAdder"),
					m_Wave1("Sine1", WAVE_SIN1_AMPL, WAVE_SIN1_FREQ, sca_core::sca_time(10, sc_core::SC_NS)),
					m_Wave2("Sine2", WAVE_SIN2_AMPL, WAVE_SIN2_FREQ, sca_core::sca_time(10, sc_core::SC_NS))
					
	{
		ResetSettings();

		// Bind the waves
		m_Wave1.out(m_WaveOut1);
		m_Wave2.out(m_WaveOut2);

		m_WaveAdder.In1(m_WaveOut1);
		m_WaveAdder.In2(m_WaveOut2);
		m_WaveAdder.Out(m_WaveResult);

		SC_METHOD(Process);
			sensitive << Mclock.pos();
			sensitive << reset;

		// Print creation
    	cout << name() << " init at time " << sc_time_stamp() << endl;

    	// Add tracking
    	m_VcdFile = sca_util::sca_create_vcd_trace_file("input_waves.vcd");
    	sca_trace(m_VcdFile, m_WaveOut1, "Wave1");
    	sca_trace(m_VcdFile, m_WaveOut2, "Wave2");
    	sca_trace(m_VcdFile, m_WaveResult, "WaveR");
	}

	virtual ~DFT();
	
private:
	
	sc_signal<double> m_WaveOut1;
	sc_signal<double> m_WaveOut2;
	sc_signal<double> m_WaveResult;

	WaveAdder m_WaveAdder;

	sca_util::sca_trace_file* m_VcdFile;

	Sine m_Wave1;
	Sine m_Wave2;

	unsigned int m_Period;
	unsigned int m_NumOfSamples;

	unsigned int m_PeriodCounter;
	unsigned int m_SampleCounter;

	unsigned int m_CoeffCounter;
	unsigned int m_CoeffReadCounter;

	bool m_SamplingFlag;
	
	std::vector<double> m_Ram;
	std::vector<double> m_Samples;

	bool IsCorrectTimeToTakeSample();
	void CaptureMeasurement();
	double ReadSample();

	void ResetSettings();

	// For debug purposes
	void PrintSamples();
};

#endif
