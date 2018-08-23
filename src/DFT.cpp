#include "DFT.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include "UtilCommon.h"

DFT::~DFT()
{
	if(nullptr != m_VcdFile)
	{
		sca_util::sca_close_vcd_trace_file(m_VcdFile);
	}
	
	// Enable this to memory dump
	#if 0
	PrintSamples();
	#endif
}

void DFT::Process()
{
	if(reset.read())
	{
		std::cout << name() << " >> reset" << std::endl;
		ResetSettings();
		// Reset out ports
		busy.write(false);
		no_more_samples.write(false);
		coeff.write(0);
	}
	else if(!busy.read())
	{
		if(take_settings.read())
		{
			unsigned int new_period = period.read();
			unsigned int new_num_of_samples = num_of_samples.read();

			std::cout << name() << " >> taking settings" << std::endl;
			std::cout << "Period = " << new_period << std::endl;
			std::cout << "Num Of Samples = " << new_num_of_samples << std::endl;
			
			if(new_num_of_samples > 1 && new_period != 0)
			{
				m_Period = new_period;
				m_NumOfSamples = new_num_of_samples;
			}
		}
		else if(start.read())
		{
			std::cout << name() << " >> start samples capture" << std::endl;
			busy.write(true);

			// Reset Counters
			m_SampleCounter = 0;
			m_PeriodCounter = 0;
			m_CoeffCounter = 0;
			m_CoeffReadCounter = 0;

			// Set sampling flag
			m_SamplingFlag = true;
		}
		else if(read.read())
		{
			if(0 != m_CoeffCounter)
			{
				if(m_CoeffReadCounter != m_CoeffCounter)
				{
					std::cout << name() << " >> read a coeff" << std::endl;
					float fp_value = static_cast<float>(m_Ram[m_CoeffReadCounter]);
					unsigned int fp_value_to_uint = ConvertToUInt(fp_value);
					coeff.write(fp_value_to_uint);
					m_CoeffReadCounter++;
				}
				else
				{
					no_more_samples.write(true);
				}
			}
		}
		else
		{
			no_more_samples.write(false);
		}
	}
	else
	{
		if(m_SamplingFlag)
		{
			if(0 != m_SampleCounter)
			{
				if(IsCorrectTimeToTakeSample())
				{
					CaptureMeasurement();
				}
				if(m_SampleCounter == m_NumOfSamples)
				{
					// Finish sampling sequence
					std::cout << name() << " >> end samples capture" << std::endl;
					m_SamplingFlag = false;
				}
			}
			else
			{
				CaptureMeasurement();
			}
		}
		else
		{
			const unsigned int c_TotalCoeffs = m_SampleCounter * 2;
			int counter = m_CoeffCounter / 2;
			double re_coeff = 0.0;
			double im_coeff = 0.0;

			for(int n = 0; n < m_SampleCounter; n++)
			{
            	re_coeff  += m_Samples[n] * cos(-2.0*M_PI*n*counter / m_SampleCounter);
	    		im_coeff  += m_Samples[n] * sin(-2.0*M_PI*n*counter / m_SampleCounter);
			}

			m_Ram[m_CoeffCounter] = re_coeff;
			m_CoeffCounter++;
			m_Ram[m_CoeffCounter] = im_coeff;
			m_CoeffCounter++;

			if(c_TotalCoeffs == m_CoeffCounter)
			{
				std::cout << name() << " >> end dft calc" << std::endl;
				busy.write(false);
			}
		}
	}
}

bool DFT::IsCorrectTimeToTakeSample()
{
	bool bTakeSample = true;
	if(m_Period != 1)
	{
		if(m_PeriodCounter == m_Period-1)
		{
			m_PeriodCounter = 0;
		}
		else
		{
			bTakeSample = false;
			m_PeriodCounter++;
		}
	}
	return bTakeSample;
}

void DFT::CaptureMeasurement()
{
	m_Samples[m_SampleCounter] = ReadSample();
	m_SampleCounter++;
}

double DFT::ReadSample()
{
	return m_WaveResult.read();
}

void DFT::ResetSettings()
{
	m_SamplingFlag = false;

	m_PeriodCounter = 0;
	m_Period = DFT_DEFAULT_PERIOD;
	
	m_SampleCounter = 0;
	m_NumOfSamples = DFT_DEFAULT_NUM_OF_SAMPLES;

	m_CoeffCounter = 0;
	m_CoeffReadCounter = 0;
	
	m_Ram.resize(RAM_SIZE);
	m_Samples.resize(RAM_SIZE);
	std::fill(m_Ram.begin(), m_Ram.end(), 0.0);
	std::fill(m_Samples.begin(), m_Samples.end(), 0.0);
}

void DFT::PrintSamples()
{
	for(unsigned int iCoeffIdx = 0; iCoeffIdx < m_CoeffCounter; iCoeffIdx++)
	{
		std::cout << "RAM[" << iCoeffIdx << "] = " << m_Ram[iCoeffIdx] << std::endl;
	}

}