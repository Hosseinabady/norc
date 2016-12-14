#ifndef __UTILITIES__
#define __UTILITIES__

#define hdr(_type,_pdu,_field)                     ((_type*)((_pdu)->m_pdu))->view_as.pdu.hdr._field
#define UNDER_CONSTRUCTION() std::cout << "Under construction "<< "File name : " << __FILE__ << " Line Number: " << __LINE__ << std::endl


#endif //__UTILITIES__