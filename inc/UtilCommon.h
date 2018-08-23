#ifndef UTILCOMMON_H_
#define UTILCOMMON_H_

#include "tlm.h"
#include "systemc.h"

#define V_MAX  		5.0
#define V_MIN 	   -5.0
#define MAX_VALUE 	67108863

struct ID_extension : tlm::tlm_extension<ID_extension> 
{
	unsigned int m_TransactionId;

	ID_extension(unsigned int TransactionId = 0) : m_TransactionId(TransactionId) 
	{

	}

	virtual tlm_extension_base* clone() const
	{ 
		ID_extension* t = new ID_extension();		
		t->m_TransactionId = this->m_TransactionId;
		return t;
	}

	virtual void copy_from(tlm_extension_base const &ext)
	{
		m_TransactionId = static_cast<ID_extension const &>(ext).m_TransactionId;
	}
};

struct TransRequest
{
	int 				m_Id;
	tlm::tlm_command 	m_Cmd;
	unsigned char* 		m_DataPtr;
	unsigned int 		m_DataLenght;
	sc_dt::uint64 		m_Addr;
};

unsigned int ConvertToUInt(float ValueToConvert);
float ConvertToFloat(unsigned int ValueToConvert);

#endif