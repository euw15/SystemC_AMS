#include "DFT.h"

#include <iostream>
#include <algorithm>
#include "UtilCommon.h"

DFT::~DFT()
{
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
		sample.write(0);
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
			m_NumOfStoredSamples = 0;
		}
		else if(read.read())
		{
			if(0 != m_NumOfStoredSamples)
			{
				if(m_SampleReadIndex != m_NumOfStoredSamples)
				{
					std::cout << name() << " >> read a sample" << std::endl;
					float fp_value = static_cast<float>(m_Ram[m_SampleReadIndex]);
					unsigned int fp_value_to_uint = ConvertToUInt(fp_value);
					sample.write(fp_value_to_uint);
					m_SampleReadIndex++;
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
		if(0 != m_SampleCounter)
		{
			if(m_SampleCounter == m_NumOfSamples)
			{
				std::cout << name() << " >> end samples capture" << std::endl;
				busy.write(false);
				m_SampleCounter = 0;
				m_PeriodCounter = 0;
				m_NumOfStoredSamples = m_NumOfSamples;
			}
			else
			{
				if(IsCorrectTimeToTakeSample())
				{
					CaptureMeasurement();
				}
			}
		}
		else
		{
			CaptureMeasurement();
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
	m_Ram[m_SampleCounter] = ReadSample();
	m_SampleCounter++;
}

double DFT::ReadSample()
{
	return m_WaveOut.read();
}

void DFT::ResetSettings()
{
	m_PeriodCounter = 0;
	m_Period = DFT_DEFAULT_PERIOD;
		
	m_SampleCounter = 0;
	m_NumOfSamples = DFT_DEFAULT_NUM_OF_SAMPLES;
	m_NumOfStoredSamples = 0;

	m_SampleReadIndex = 0;
	
	m_Ram.resize(RAM_SIZE);
	std::fill(m_Ram.begin(), m_Ram.end(), 0.0);
}

void DFT::PrintSamples()
{
	for(unsigned int iSampleIdx = 0; iSampleIdx < m_NumOfStoredSamples; iSampleIdx++)
	{
		std::cout << "RAM[" << iSampleIdx << "] = " << m_Ram[iSampleIdx] << std::endl;
	}

}