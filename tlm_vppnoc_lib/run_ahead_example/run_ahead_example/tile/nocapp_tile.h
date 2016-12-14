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
    for (int i = 0; i < NO_ROUTER_PORT; i++)
    {
      ostringstream interface_name;
      interface_name << "interface(" << i <<")";
      m_nocapp_network_interface[i] = new nocapp_network_interface(interface_name.str().c_str(), i, ty);
    }

    m_norc_microscheduler      = new (norc_microscheduler)("NoRCMng", id, this);

    m_routers = new nocapp_router("router", id, ty);


    for (int i  = 0; i < NO_ROUTER_PORT; i++) 
    {
      m_routers->m_outPort[i](m_nocapp_network_interface[i]->m_inPort);
      m_nocapp_network_interface[i]->m_outPort(m_routers->m_inPort[i]);
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

  nocapp_router*                                                m_routers;
  nocapp_network_interface*                                     m_nocapp_network_interface[NO_ROUTER_PORT];
  
  

};
}






#endif //__NOCAPP_TILE_H__