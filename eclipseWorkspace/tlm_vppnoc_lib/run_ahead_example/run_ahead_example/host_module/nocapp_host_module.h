#pragma once
#ifndef __NOCAPP_HOST_MODULE_H__
#define __NOCAPP_HOST_MODULE_H__


#include <systemc>
#include "vppnoc_core.h"
#include "include/nocapp_utilities.h"
#include <sstream>

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace vppnoc;


namespace nocapp {

class nocapp_host_module : 
	public vppnoc_module {
	
public:
  nocapp_host_module
    ( sc_module_name name
    , int id
    , TileType ty
    ) : vppnoc_module(name, id, ty) 
  {
    for (int i = 0; i < 2*(NO_OF_ROW+NO_OF_COL); i++)
    {
      ostringstream inport_name;
      inport_name << "host_in_port(" << i <<")";
      m_p_inPort[i]  = new   vppnoc_slave_socket<nocapp_host_module, NO_OF_CHANNELS>(inport_name.str().c_str());
      (*m_p_inPort[i])(*this);
      ostringstream outport_name;
      outport_name << "host_out_port(" << i <<")";
	    m_p_outPort[i] = new  vppnoc_master_socket<nocapp_host_module, NO_OF_CHANNELS>(outport_name.str().c_str());
      (*m_p_outPort[i])(*this);
    }
  }


     	virtual
  vppnoc_sync_enum_type  
    nb_transport_fw
    (int socket_id
     , vppnoc_transaction& message_trans
     , vppnoc_phase & phase
     , sc_time & delay
     ) 
  {
     m_p_outPort[0]->nb_transport_fw(0, message_trans, phase, delay);
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


  vppnoc_master_socket<nocapp_host_module, NO_OF_CHANNELS>*                    m_p_outPort[2*(NO_OF_ROW+NO_OF_COL)];  
  vppnoc_slave_socket<nocapp_host_module, NO_OF_CHANNELS>*                     m_p_inPort[2*(NO_OF_ROW+NO_OF_COL)];  
	
};

}

#endif /*__NOCAPP_HOST_MODULE_H__*/

