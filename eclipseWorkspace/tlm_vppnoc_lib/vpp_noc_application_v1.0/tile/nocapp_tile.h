#ifndef  __NOCAPP_TILE_H__
#define __NOCAPP_TILE_H__

#include "tile/router/nocapp_router.h"
#include "tile/network_interface/nocapp_network_interface.h"

#include <sstream>


namespace nocapp {
using namespace std;
using namespace vppnoc;
using namespace sc_core;
using namespace sc_dt;

class 
nocapp_tile 
: public vppnoc_module {

public:
  nocapp_tile( sc_module_name name
	           , int id
	           , TileType ty
	) : vppnoc_module(name, id, ty)
	{
    for (int i = 0; i < NO_ROUTER_PORT-1; i++)
    {
      ostringstream inport_name;
      inport_name << "tile_in_port(" << i <<")";
      m_p_inPort[i]  = new   vppnoc_target_socket<NO_OF_CHANNELS>(inport_name.str().c_str());
      ostringstream outport_name;
      outport_name << "tile_out_port(" << i <<")";
	    m_p_outPort[i] = new  vppnoc_initiator_socket<NO_OF_CHANNELS>(outport_name.str().c_str());
    }

    m_norc_microscheduler      = new (norc_microscheduler)("NoRCMng", id, this);


    m_routers = new nocapp_router("router", id, ty);
    m_nocapp_network_interface = new nocapp_network_interface("network_interface", id, ty);

    m_routers->m_outPort[0](m_nocapp_network_interface->m_inPort);
    m_nocapp_network_interface->m_outPort(m_routers->m_inPort[0]);

    for (int i  = 0; i < NO_ROUTER_PORT-1; i++) 
    {
      (*m_p_inPort[i])(m_routers->m_inPort[i+1]);
      (m_routers->m_outPort[i+1])(*m_p_outPort[i]);

    }

  }
  ~nocapp_tile(){};

 	virtual
  vppnoc_sync_enum_type  
    nb_transport_fw
    (int socket_id
     , vppnoc_transaction& message_trans
     , vppnoc_phase & phase
     , sc_time & delay
     ) 
  {
     return VPPNOC_ACCEPTED;
	}

  virtual
    vppnoc_sync_enum_type
    nb_transport_bw
    (
       int socket_id
     , vppnoc_transaction& r_response_message_trans
     , vppnoc_phase& phase
     , sc_time& delay) 
  {

    
		return VPPNOC_ACCEPTED;
	}


public:
  typedef VPPNOCMicroScheduler<nocapp_tile>                     norc_microscheduler;
 
  norc_microscheduler*                                          m_norc_microscheduler;

 	vppnoc_initiator_socket<NO_OF_CHANNELS>*                      m_p_outPort[NO_ROUTER_PORT-1];  
  vppnoc_target_socket<NO_OF_CHANNELS>*                         m_p_inPort[NO_ROUTER_PORT-1];  

  nocapp_router*                                                m_routers;
  nocapp_network_interface*                                     m_nocapp_network_interface;

};
}






#endif //__NOCAPP_TILE_H__