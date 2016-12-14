
//#define DEBUG_DUMP_NET
//#define DEBUG_DUMP_ROUTER
//#define DEBUG_DUMP_PHYSICAL
//#define   DEBUG_DUMP_BUFFER

#ifndef UTILITIES_H
#define UTILITIES_H

namespace nocapp {


#define  NO_OF_ROW                                                                 20
#define  NO_OF_COL                                                                 20

#define  NO_ROUTER_PORT                                                            5
#define  ROUTER_BUFFER_LENGTH                                                      2




#define MESSAGE_DATA_LENGTH                                                        1024 //100 //  
#define PACKET_DATA_LENGTH                                                         100  //130 //  
#define FLIT_DATA_LENGTH  														                             63   //120 //  



#define NO_OF_CHANNELS                                                             5
#define TASK_SEEK_LIFE_TIME                                                        10
enum ActivityTypeEnum { CORE_FREE, CORE_ACTIVE, CORE_RESERVED };


enum ChannelStateEnum { CHANNEL_IDLE,
						            CHANNEL_BUSY,
						          };


typedef ChannelStateEnum   ChannelStateType;


#define TRANSPORT_TO_NETWORK_DELAY												                         1
#define NETWORK_LAYER_UPSTREAM_INPUT_BUFFER_DELAY                                  1


typedef ActivityTypeEnum   ActivityType;



enum CastingTypeEnum {UNI_CAST = 1,
					  MUT_CAST,
					  BRD_CAST
					  };
typedef CastingTypeEnum   CastingType;
enum RoutingTypeEnum {XY, RND, PUR_FLOODING, DIR_FLOODING};
typedef RoutingTypeEnum   RoutingType;

#define      NI_BUFFER_LENGTH                                                      10



#define UNDER_CONSTRUCTION() std::cout << "Under construction "<< "File name : " << __FILE__ << " Line Number: " << __LINE__ << std::endl
#define hdr(_type,_pdu,_field)                     ((_type*)((_pdu)->m_pdu))->view_as.pdu.hdr._field



}

#endif //UTILITIES_H