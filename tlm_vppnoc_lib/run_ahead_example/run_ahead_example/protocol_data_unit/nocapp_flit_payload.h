
#ifndef NOC_FLIT_PAYLOAD_H_
#define NOC_FLIT_PAYLOAD_H_

#include "vppnoc_core.h"
#include "include\nocapp_utilities.h"

using namespace  vppnoc;
namespace nocapp {

enum FlitCommandEnum   {RQ_FLIT,
						ACK_FLIT,
						NORMAL_FLIT,
						ROAD_BLK,		//Road Block Flit
						RLS_FLIT		//Release Reserved Resources Flit
						} ;
typedef FlitCommandEnum  FlitCommandType;



enum flit_t {FIRST_FLIT,
	           MIDDLE_FLIT,
	           LAST_FLIT } ;

typedef flit_t      flit_type;

struct FlitHeader {
	vppnoc_field<FlitCommandType, 4>		m_cmd;               // type of flit
	vppnoc_field<unsigned int, 4>       m_channel;
	vppnoc_field<flit_type, 2>			    m_flit;              //
	vppnoc_field<bool, 1>               m_rls_flit;
	vppnoc_field<bool, 1>               m_head_flit;
	vppnoc_field<unsigned int, 4>       m_lifetime;
	vppnoc_field<CastingType, 4>        m_cast; 
	vppnoc_field<RoutingType, 4>        m_routing;
	vppnoc_field<unsigned int, 10>  	  m_dst_tile;          // 
  vppnoc_field<unsigned int, 10>  	  m_src_tile;          // 
 	vppnoc_field<unsigned int, 8>     	m_flit_id;           // Debuge
//  vppnoc_field<unsigned int, 8>     m_message_id;        // Debuge
	vppnoc_field<unsigned int, 8>       m_packet_id;         // debuge
};

typedef vppnoc_pdu<nocapp::FlitHeader, bool, FLIT_DATA_LENGTH>   nocapp_flit_payload;

}



#endif /*NOC_Flit_PAYLOAD_H_*/
