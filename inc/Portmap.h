#ifndef PORTMAP_H_
#define PORTMAP_H_

#define ADC_BASE_ADDR 0x00000100
#define ADC_RESET 0x00000000
#define ADC_CTRL1 0x00000001
#define ADC_CTRL2 0x00000002

typedef union
{
	struct
	{
		unsigned int Reset 	: 1;
		unsigned int Spare1	: 31;
	};
	unsigned int AllBits;
} AdcReset_t;

#endif