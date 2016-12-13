#ifndef NOC_MESSAGE_PAYLOAD_H__
#define NOC_MESSAGE_PAYLOAD_H__


#include "vppnoc_core.h"
#include "include\nocapp_utilities.h"
using namespace vppnoc;

namespace nocapp {
enum MessageTypeEnum {REQ_MESSAGE,
					  RESP_MESSAGE
					 };

// Message Command
enum MessageCmdEnum {TASK_SEEK = 1,
                     ACK_SEEK,
					 READ_DATA,
					 WRITE_DATA,
					 TASK_ACTIVE_REQ,
					 TASK_ACTIVE_ACK,
					 TASK_CMD_START	,
					 TASK_RLS_REQ,
					 TASK_RLS_ACK,
					 APPLICATION_MAPPED_FAILED,
					 APPLICATION_RELEASED,
					 APPLICATION_MAPPED
					};



enum TileStatusTypeEnum { TILE_BUSY = 1,
                          TILE_READY,
						  TILE_FREE,
						  TILE_RESERVED,
						};
typedef TileStatusTypeEnum TileStatusType;

struct Volunteer {
	vppnoc_field<int, 4>              tile_id;
	vppnoc_field<int, 4>              core_id;
	vppnoc_field<TileStatusType, 2>   tile_status;
};

typedef MessageTypeEnum   MessageType;
typedef MessageCmdEnum    MessageCmd;




struct MessageHeader{
//	NoCTask*		    m_src_task;				//source task 
	vppnoc_field<int, 4>     		            m_src_core;				//source core 
	vppnoc_field<int, 4>     		            m_dst_core;				//source core 
	vppnoc_field<int, 10>                   m_src_tile;
	vppnoc_field<int, 10>                   m_dst_tile;
	vppnoc_field<MessageType, 4>		        m_type;					//SHOULD BE CHECKED
	vppnoc_field<unsigned int, 32> 	        m_src_address;			//SHOULD BE CHECKED
// field for task searching
//	NoCTask*            m_req_task;				//Requested task
	vppnoc_field<MessageCmd, 5>             m_cmd;  //
	//field for respons to task searching
	vppnoc_field<CastingType, 4>            m_cast; 
	vppnoc_field<int, 4>                    m_host_tile_id;
	vppnoc_field<int, 4>                    m_host_type;			// 0-- imposible, 1-- task_esixt 2-- free space exist 
	vppnoc_field<int, 8>                    m_message_id;    //Debuge
	vppnoc_field<int, 4>                    m_life_time;
	nocapp::Volunteer								        m_volunteer[TASK_SEEK_LIFE_TIME];
	vppnoc_field<int, 4>                    m_payload_size;
	vppnoc_field<int, 4>                    m_period;
};

typedef vppnoc_pdu<MessageHeader, bool, MESSAGE_DATA_LENGTH>   nocapp_message_payload;

}
#endif
