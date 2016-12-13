#ifndef NOC_MESSAGE_PAYLOAD_H__
#define NOC_MESSAGE_PAYLOAD_H__

#include "model/protocol_data_unit/include/noc_tlm_extension_payload.h"
#include "model/tasks/include/noc_task.h"
#include "NoRC/utilities/include/utilities.h"





namespace tlm_noc {

enum MessageTypeEnum {REQ_MESSAGE,
					  RESP_MESSAGE
					 };

// Message Command
enum MessageCmdEnum {TASK_SEEK,
                     ACK_SEEK,
					 READ_DATA,
					 WRITE_DATA,
					 TASK_ACTIVE_REQ,
					 TASK_ACTIVE_ACK,
					 TASK_CMD_START	,
					 TASK_RLS_REQ,
					 TASK_RLS_ACK,
					 APPLICATION_MAPPED_FAILED,
					 APPLICATION_MAPPED
					};

enum CastingTypeEnum {UNI_CAST,
					  MUT_CAST,
					  BRD_CAST
					  };

enum TileStatusTypeEnum { TILE_BUSY,
                          TILE_READY,
						  TILE_FREE,
						  TILE_RESERVED,
						};
typedef TileStatusTypeEnum TileStatusType;

struct Volunteer {
	Volunteer*     next;
	unsigned int   tile_id;
	unsigned int   core_id;
	TileStatusType tile_status;
};

typedef MessageTypeEnum   MessageType;
typedef MessageCmdEnum    MessageCmd;
typedef CastingTypeEnum   CastingType;

enum RoutingTypeEnum {XY, RND, PUR_FLOODING, DIR_FLOODING};
typedef RoutingTypeEnum   RoutingType;
//enum tlm_noc_message_layer_phase_enum { UNINITIALIZED_MESSAGE_PHASE=0, BEGIN_MESSAGE_REQ=1, END_MESSAGE_REQ, BEGIN_MESSAGE_RESP, END_MESSAGE_RESP };

//typedef  tlm_noc_message_layer_phase_enum tlm_noc_message_layer_phase;


struct MessageHeader{
	NoCTask*		    m_src_task;				//source task 
	int     		    m_src_core;				//source core 
	int     		    m_dst_core;				//source core 
	int                 m_src_tile;
	int                 m_dst_tile;
	MessageType		    m_type;					//SHOULD BE CHECKED
	sc_dt::uint64	    m_src_address;			//SHOULD BE CHECKED
	// field for task searching
	NoCTask*            m_req_task;				//Requested task
	MessageCmd          m_cmd;  //
	//field for respons to task searching
	CastingType         m_cast; 
	int                 m_host_tile_id;
	int                 m_host_type;			// 0-- imposible, 1-- task_esixt 2-- free space exist 
	int                 m_message_id;    //Debuge
	int                 m_life_time;
	Volunteer*          m_volunteer;
	int                 m_payload_size;
};

typedef noc_tlm_extension_payload<tlm_noc::MessageHeader, unsigned char, MESSAGE_DATA_LENGTH>   noc_message_payload;

}

#endif
