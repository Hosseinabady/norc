#ifndef NOC_PACKET_PAYLOAD_H_
#define NOC_PACKET_PAYLOAD_H_

#include "NoRC/utilities/include/utilities.h"
#include "noc_payload.h"
namespace tlm_noc {

enum PacketCommandEnum   {REQ_PACKET,  		// Request Packet
						  RES_PACKET,   		// Response Packet
						  ACK_PACKET,   		// Acknowledge Packet
						  NAK_PACKET,  		// Non-Acknowledge Packet
						  RLS_PACK,     		// Release Resource Packet
						  CHANNELFULL_PACKET	// Channel Full Packet
						 } ;
typedef PacketCommandEnum  PacketCommandType;

enum PacketTypeEnum {FIRST_PACKET,
			         MIDDLE_PACKET,
	                 LAST_PACKET } ;

typedef PacketTypeEnum  PacketType;


struct PacketHeader {
	PacketCommandType	m_cmd;
	int        			m_crc;
	PacketType			m_packet;
	int 				m_packet_id;
	int					m_src_core;
	int                 m_session;
	CastingType         m_cast; 
	RoutingType         m_routing;
	int                 m_dst_tile;
	int                 m_src_tile;
	int                 m_message_id;    //Debuge
};

typedef noc_tlm_extension_payload<tlm_noc::PacketHeader, char, PACKET_DATA_LENGTH>   noc_packet_payload;



}
#endif /*NOC_PACKET_PAYLOAD_H_*/
