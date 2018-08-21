#include "UtilCommon.h"

unsigned int DigitizeRead(double Value)
{
	if(Value > V_MAX)
	{
		Value = V_MAX; 
	}
	if(Value < V_MIN)
	{
		Value = V_MIN;
	}
	double ratio = (Value + V_MAX) / (2*V_MAX);
	double result = ratio * MAX_VALUE;
	return static_cast<unsigned int>(result);
}

double UndigitizeRead(unsigned int Value)
{
	double ratio = static_cast<double>(Value) / MAX_VALUE;
	double read = (2*ratio*V_MAX) - V_MAX;
	return read;
}