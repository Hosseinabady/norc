#ifndef __NOCAPP_NORC_H__
#define __NOCAPP_NORC_H__

#include "tile/nocapp_tile.h"
#include <sstream>


namespace nocapp {
using namespace std;
using namespace vppnoc;
using namespace sc_core;
using namespace sc_dt;

class 
nocapp_norc 
: public vppnoc_module {

public:
  nocapp_norc( sc_module_name name
	           , int id
	           , TileType ty
	) : vppnoc_module(name, id, ty)
	{
    m_tile = new nocapp_tile("system_under_test", 1, INTER_TILE);
  }



  ~nocapp_norc(){};
public:
   	virtual
  vppnoc_sync_enum_type  
    nb_transport_fw
    (int socket_id
     , vppnoc_transaction& message_trans
     , vppnoc_phase & phase
     , sc_core::sc_time & delay
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



  nocapp_tile*                                                   m_tile;
};
}
#endif //__NOCAPP_NORC_H__