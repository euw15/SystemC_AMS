#include "UtilCommon.h"
#include <cstring>

unsigned int ConvertToUInt(float ValueToConvert)
{
	unsigned int result;
	size_t len = sizeof(float);
	unsigned char* ptr = reinterpret_cast<unsigned char*>(&ValueToConvert);
	memcpy(&result, ptr, len);
	return result;
}

float ConvertToFloat(unsigned int ValueToConvert)
{
	float result;
	size_t len = sizeof(unsigned int);
	unsigned char* ptr = reinterpret_cast<unsigned char*>(&ValueToConvert);
	memcpy(&result, ptr, len);
	return result;
}