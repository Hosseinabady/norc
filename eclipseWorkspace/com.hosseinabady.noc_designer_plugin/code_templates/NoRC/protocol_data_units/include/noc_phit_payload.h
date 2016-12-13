#ifndef NOC_PHIT_PAYLOAD_H_
#define NOC_PHIT_PAYLOAD_H_
#include "tlm.h"
#include <cassert>


namespace tlm_noc {

struct PhitHeader {
	bool			   m_b_last_phit;
	int 			   m_phit_id;
	bool 			   m_ack;
	bool               m_data_phit;
	bool               m_rls_phit;
	int				   flit_type_debug;		//Debuge
	int                m_message_id;        //Debuge
	int                m_packet_id;         //debuge
	int                m_flit_id;			//debuge

};

typedef noc_tlm_extension_payload<tlm_noc::PhitHeader, char, PHIT_DATA_LENGTH>   noc_phit_payload;


}

#endif /*NOC_PHIT_PAYLOAD_H_*/

