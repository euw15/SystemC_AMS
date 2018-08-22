#ifndef ADC_H_
#define ADC_H_

#include "systemc.h"
#include "systemc-ams.h"
#include "TdfWaves.h"
#include "filter.h"

#include <vector>

#define ADC_DEFAULT_NUM_OF_SAMPLES 	100
#define ADC_DEFAULT_PERIOD 			1

#define WAVE_AMPL					5.0
#define WAVE_FREQ					1000000	// 1MHz

SC_MODULE(ADC)
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
	sc_out<sc_uint<26>> sample;

	void Process();

	SC_CTOR(ADC) : m_WaveGenerator("Sine", WAVE_AMPL, WAVE_FREQ, sca_core::sca_time(10, sc_core::SC_NS))
	{
		ResetSettings();

		// Bind the wave generator
		m_WaveGenerator.out(m_WaveOut);

		SC_METHOD(Process);
			sensitive << Mclock.pos();
			sensitive << reset;

		// Print creation
    	cout << name() << " init at time " << sc_time_stamp() << endl;
	}

	virtual ~ADC();

private:

	sc_signal<double> m_WaveOut;
	Sine m_WaveGenerator;

	unsigned int m_PeriodCounter;
	unsigned int m_Period;
	
	unsigned int m_SampleCounter;
	unsigned int m_NumOfSamples;
	unsigned int m_NumOfStoredSamples;
	
	unsigned int m_TotalSamples;
	unsigned int m_SampleReadIndex;

	std::vector<double> m_Ram;

	bool IsCorrectTimeToTakeSample();
	void CaptureMeasurement();
	double ReadSample();

	void ResetSettings();

	// For debug purposes
	void PrintSamples();
};

#endif
