#ifndef PORTMAP_H_
#define PORTMAP_H_

#define ADC_BASE_ADDR 	0x00000100

#define ADC_RESET 		0x00000000	// WO
#define ADC_CTRL1 		0x00000001	// R/W
#define ADC_CTRL2 		0x00000002	// WO
#define ADC_RAM_DATA	0x00000003	// RO

typedef union
{
	struct
	{
		unsigned int Reset 	: 1;
		unsigned int Spare1	: 31;
	};
	unsigned int AllBits;
} AdcReset_t;

typedef union
{
	struct
	{
		unsigned int Period 		: 14;
		unsigned int NumofSamples 	: 13;
		unsigned int Spare1			: 16;
	};
	unsigned int AllBits;
} Adc_Ctrl1_t;

typedef union
{
	struct
	{
		unsigned int Start		: 1;
		unsigned int Spare1		: 31;
	};
	unsigned int AllBits;
} Adc_Ctrl2_t;

typedef union
{
	struct
	{
		unsigned int Sample		: 26;
		unsigned int Spare1		: 6;
	};
	unsigned int AllBits;
} Adc_Ram_Data_t;

#endif