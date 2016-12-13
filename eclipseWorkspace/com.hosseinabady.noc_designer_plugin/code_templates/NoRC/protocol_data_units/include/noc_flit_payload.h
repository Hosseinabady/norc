#ifndef NOC_FLIT_PAYLOAD_H_
#define NOC_FLIT_PAYLOAD_H_

#include "NoRC/utilities/include/utilities.h"
#include "noc_payload.h"

namespace tlm_noc {

enum FlitCommandEnum   {NORMAL_FLIT,
						ROAD_BLK,		//Road Block Flit
						RLS_FLIT		//Release Reserved Resources Flit
						} ;
typedef FlitCommandEnum  FlitCommandType;



enum flit_t {FIRST_FLIT,
	         MIDDLE_FLIT,
	         LAST_FLIT } ;

typedef flit_t      flit_type;

struct FlitHeader {
	FlitCommandType		m_cmd;               // type of flit
	short int      		m_port_numbers;      // used in the router switch for random routing algorithm
	int					m_source_task;       //
	int                 m_session;
	flit_type			m_flit;              //
	int         		m_flit_id;           //
	bool                m_head_flit;
	int           		m_volunteer_id;      //
	int           		m_volunteer_cost;    //
	int 				m_source_tile_id;
	int                 m_lifetime;
	CastingType         m_cast; 
	RoutingType         m_routing;
	int                 m_tile_id;
	int                 m_flit_number;       // for debug
	int					m_dst_tile;          // first flit
	int                 m_task_id;           // request task id
	int                 m_message_id;        // Debuge
	int                 m_packet_id;         // debuge
	bool                m_rls_flit;
};

typedef noc_tlm_extension_payload<tlm_noc::FlitHeader, char, FLIT_DATA_LENGTH>   noc_flit_payload;



}



#endif /*NOC_Flit_PAYLOAD_H_*/
