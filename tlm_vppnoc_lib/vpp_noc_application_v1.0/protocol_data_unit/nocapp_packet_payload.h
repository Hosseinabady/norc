
#ifndef NOC_PACKET_PAYLOAD_H_
#define NOC_PACKET_PAYLOAD_H_

#include "vppnoc_core.h"
#include "include\nocapp_utilities.h"

namespace nocapp {

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
	vppnoc_field<PacketCommandType, 4>	 m_cmd;
	vppnoc_field<unsigned int, 4>        m_crc;
	vppnoc_field<PacketType, 4>			     m_packet;
	vppnoc_field<unsigned int, 8> 			 m_packet_id;
	vppnoc_field<unsigned int, 4>				 m_src_core;
	vppnoc_field<unsigned int, 4>        m_channel;
	vppnoc_field<CastingType, 4>         m_cast; 
	vppnoc_field<RoutingType, 4>         m_routing;
	vppnoc_field<unsigned int, 10>        m_dst_tile;
	vppnoc_field<unsigned int, 10>        m_src_tile;
	vppnoc_field<unsigned int, 8>        m_message_id;    //Debuge
};

typedef vppnoc_pdu<nocapp::PacketHeader, bool, PACKET_DATA_LENGTH>   nocapp_packet_payload;



}


#endif /*NOC_PACKET_PAYLOAD_H_*/
