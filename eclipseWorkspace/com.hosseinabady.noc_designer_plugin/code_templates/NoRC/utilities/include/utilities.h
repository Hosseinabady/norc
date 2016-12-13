#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <iostream>
#include "tlm.h"
#include <fstream>
#include <model/include/modelutilities.h>

#define MESSAGE_DATA_LENGTH 	300 //+16 = 1040
#define PACKET_DATA_LENGTH   	128//32// 256 //+32  = 160
#define FLIT_DATA_LENGTH  		64//16 // 64 //+48   = 112 --> 120
#define PHIT_DATA_LENGTH    	32//16 // 32 //+12   = 44


enum TileTypeEnum   {DUMMY_TILE,
					 IO_TILE = 0,
					 INTER_TILE
						} ;

typedef TileTypeEnum                 TileType;

//#define DEBUG
//#define DEBUG_SYSTEMC

#define RIGHT_PORT	                 0
#define UP_PORT		                 1
#define LEFT_PORT	                 2
#define DOWN_PORT	                 3
#define LOCAL_PORT	                 4



#define  NO_OF_ROW_CONST                            8
#define  NO_OF_COL_CONST                            8
#define  NR_OF_CHANNEL_CONST                        2
#define  INPUT_CHANNEL_BUFFER_LENGTH_CONST          4
#define  OUTPUT_CHANNEL_BUFFER_LENGTH_CONST         4
#define  NR_OF_ROUTER_PORT_CONST                    5


//Address map in a tile
#define NETWORK_INTERFACE_ADD        0x0000000000000000
#define RTR_CORE_0_INTERFACE_ADD     0x0000000000000001
#define RTR_CORE_1_INTERFACE_ADD     0x0000000000000002
#define MEMORY_MODULE                0x0000000000000003


#define AMBABUS_TO_TRANSPORT_DELAY   1
#define TRANSPORT_TO_NETWORK_DELAY   1
#define NETWORK_TO_PHYSICAL_DELAY    1
#define PHYSICAL_TO_PHYSICAL_DELAY   1

//Delay in SC_NS
#define INPUT_BUFFER                 1
#define ROUTER_ALGORITHM_DELAY       2
#define ARBITER_DELAY                1
#define SWITCHING_DELAY              1
#define OUTPUT_BUFF_DELAY            1
#define OUTPUT_BUFF_CNT_DELAY        1


#define INFINITIVE                   1000000

#define TASK_SEEK_LIFE_TIME          5

#define NUMBER_OF_RETRY_TO_MAP_TASK  3

typedef    tlm::tlm_generic_payload	 transaction_type;
typedef    tlm::tlm_phase            phase_type;
typedef    tlm::tlm_sync_enum        sync_enum_type;

#define NO_OF_RECONFIGURABLE_CORE    2
#define RECONFIGURATION_RELEASE_WAIT 200



 enum ActivityTypeEnum {	FREE,
						ACTIVE,
						RESERVED
					      };
typedef ActivityTypeEnum   ActivityType;


#define UNDER_CONSTRUCTION() std::cout << "Under construction "<< "File name : " << __FILE__ << " Line Number: " << __LINE__ << std::endl
#define LogReport()           std::cout << "\t\tFunction Name: "<< __FUNCTION__ << "\n\t\tFile name : " << __FILE__ << "\n\t\tLine Number: " << __LINE__ << std::endl



void PrMsg(char* FileName, int LineNumber, sc_core::sc_module_name name);
#define LogPhit(phit)  std::cout << "=====Phit Log======\n" << " m_b_last_phit = " << noc_gp_hdr(phit, m_b_last_phit)  << "\n m_phit_no = " << noc_gp_hdr(phit, m_phit_no) <<std::endl;


void trafficLog(sc_core::sc_module_name name, int id, char direction[]);
#define PrintMessage() PrMsg(__FILE__, __LINE__, name())


#define NOCPROTOCOLLAYER_PARAM NR_OF_CHANNEL_CONST,\
		   NR_OF_CHANNEL_CONST,\
		   INPUT_CHANNEL_BUFFER_LENGTH_CONST,\
		   OUTPUT_CHANNEL_BUFFER_LENGTH_CONST,\
		   tlm::tlm_base_protocol_types,\
		   tlm::tlm_base_protocol_types,\
		   tlm::tlm_generic_payload,\
		   tlm::tlm_phase,\
		   tlm::tlm_sync_enum


#define ROUTER_TEMPLATE_PARAMETER NR_OF_CHANNEL_CONST,\
		   NR_OF_CHANNEL_CONST,\
		   INPUT_CHANNEL_BUFFER_LENGTH_CONST,\
		   OUTPUT_CHANNEL_BUFFER_LENGTH_CONST,\
		   tlm::tlm_base_protocol_types,\
		   tlm::tlm_base_protocol_types,\
		   tlm::tlm_generic_payload,\
		   tlm::tlm_phase,\
		   tlm::tlm_sync_enum

#define TILE_TEMPLATE_PARAMETER NR_OF_CHANNEL_CONST,\
		    NR_OF_CHANNEL_CONST,\
		    INPUT_CHANNEL_BUFFER_LENGTH_CONST,\
		    OUTPUT_CHANNEL_BUFFER_LENGTH_CONST,\
		    tlm::tlm_base_protocol_types,\
		    tlm::tlm_base_protocol_types,\
		    tlm::tlm_generic_payload,\
		    tlm::tlm_phase,\
		    tlm::tlm_sync_enum

#endif /*UILITIES_H_*/

