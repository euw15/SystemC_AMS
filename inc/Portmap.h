#ifndef PORTMAP_H_
#define PORTMAP_H_

#define DFT_BASE_ADDR 	0x00000100

#define DFT_RESET 		0x00000000	// WO
#define DFT_CTRL1 		0x00000001	// WO
#define DFT_CTRL2 		0x00000002	// WO
#define DFT_RAM_DATA	0x00000003	// RO

typedef union
{
	struct
	{
		unsigned int Reset 	: 1;
		unsigned int Spare1	: 31;
	};
	unsigned int AllBits;
} DftReset_t;

typedef union
{
	struct
	{
		unsigned int Period 		: 14;
		unsigned int NumofSamples 	: 13;
		unsigned int Spare1			: 16;
	};
	unsigned int AllBits;
} DftCtrl1_t;

typedef union
{
	struct
	{
		unsigned int Start		: 1;
		unsigned int Spare1		: 31;
	};
	unsigned int AllBits;
} DftCtrl2_t;

typedef union
{
	struct
	{
		unsigned int Sample		: 32;
	};
	unsigned int AllBits;
} DftRamData_t;

#endif