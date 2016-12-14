#ifndef __PAYLOAD_H__
#define __PAYLOAD_H__


#include "vppnoc_core.h"

using namespace vppnoc;

namespace nocapp {


struct Header{
	vppnoc_field<int, 4>     		            m_value;				
};

typedef vppnoc_pdu<Header, bool, 2>   payload;

}
#endif
