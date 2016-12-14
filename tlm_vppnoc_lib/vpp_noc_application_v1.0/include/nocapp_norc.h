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
    for (int i = 0; i < 2*(NO_OF_ROW+NO_OF_COL); i++)
    {
      ostringstream inport_name;
      inport_name << "norc_in_port(" << i <<")";
      m_p_inPort[i]  = new   vppnoc_target_socket<NO_OF_CHANNELS>(inport_name.str().c_str());
      ostringstream outport_name;
      outport_name << "norc_out_port(" << i <<")";
	    m_p_outPort[i] = new  vppnoc_initiator_socket<NO_OF_CHANNELS>(outport_name.str().c_str());
    }

    m_norc_microscheduler      = new (norc_microscheduler)("NoRCMng", id, this);
    for (int i = 0; i < 2*(NO_OF_ROW+NO_OF_COL); i++)
    {
 
    }
	  
    for (int i = 0; i < NO_OF_COL; i++) 
    {
      for (int j = 0; j < NO_OF_ROW; j++) 
      {
        ostringstream tile_name;
        tile_name << "tile(" << i <<",";
        tile_name << j << ")";
        m_tile[i][j] = new nocapp_tile(tile_name.str().c_str(), j*NO_OF_COL+i, INTER_TILE);
      }

    }

    for (int i = 0; i < NO_OF_COL; i++) 
    {
      for (int j = 0; j < NO_OF_ROW; j++) 
      {

        
        if (j == 0          ) { (*m_tile[i][j]->m_p_outPort[3])(*m_p_outPort[3*NO_OF_COL+i]); 
                                (*m_p_inPort[3*NO_OF_COL+i])(*m_tile[i][j]->m_p_inPort[3]);     }
        
        if (j == NO_OF_ROW-1) { (*m_tile[i][j]->m_p_outPort[1])(*m_p_outPort[1*NO_OF_COL+i]); 
                                (*m_p_inPort[1*NO_OF_COL+i])(*m_tile[i][j]->m_p_inPort[1]);     }

        if (i == 0          ) { (*m_tile[i][j]->m_p_outPort[2])(*m_p_outPort[2*NO_OF_COL+j]); 
                                (*m_p_inPort[2*NO_OF_COL+j])(*m_tile[i][j]->m_p_inPort[2]);     }

        if (i == NO_OF_COL-1) { (*m_tile[i][j]->m_p_outPort[0])(*m_p_outPort[0*NO_OF_COL+j]); 
                                (*m_p_inPort[0*NO_OF_COL+j])(*m_tile[i][j]->m_p_inPort[0]);     }
        
        if (i+1 < NO_OF_COL ) { (*m_tile[i][j]->m_p_outPort[0])(*m_tile[i+1][j]->m_p_inPort[2]);
                                (*m_tile[i+1][j]->m_p_outPort[2])(*m_tile[i][j]->m_p_inPort[0]); }
        if (j+1 < NO_OF_ROW ) { (*m_tile[i][j]->m_p_outPort[1])(*m_tile[i][j+1]->m_p_inPort[3]);
                                (*m_tile[i][j+1]->m_p_outPort[3])(*m_tile[i][j]->m_p_inPort[1]); }

      }
    }
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
  typedef VPPNOCMicroScheduler<nocapp_norc>                      norc_microscheduler;
  
  norc_microscheduler*                                           m_norc_microscheduler;
  vppnoc_target_socket<NO_OF_CHANNELS>*                          m_p_inPort[2*(NO_OF_ROW+NO_OF_COL)];
	vppnoc_initiator_socket<NO_OF_CHANNELS>*                       m_p_outPort[2*(NO_OF_ROW+NO_OF_COL)];  

  nocapp_tile*                                                  m_tile[NO_OF_ROW][NO_OF_COL];
};
}
#endif //__NOCAPP_NORC_H__