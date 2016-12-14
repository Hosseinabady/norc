#pragma once
#ifndef PHYSICAL_LAYER_H_
#define PHYSICAL_LAYER_H_


#include "vppnoc_core.h"
#include "include/nocapp_utilities.h"

#include <fstream>

extern std::ofstream dumpphysicallayerfile;

namespace nocapp {
using namespace std;
using namespace vppnoc;
using namespace sc_core;
using namespace sc_dt;

class PhysicalLayer : public NoCProtocolLayer {
public:

	PhysicalLayer(sc_module_name name, 
	              int id, 
				        TileType ty) 
                : NoCProtocolLayer(name, id, ty)
                , m_inPort_from_net("inPort_from_net")
                , m_outPort2net("outPort2net")
                , m_inPort_from_router("inPort_from_router")
                , m_outPort2router("outPort2router")
	{

		m_phy_microscheduler       = new (phy_microscheduler)("PhyLyrMng", id, this);
    
    m_inPort_from_net.register_nb_transport_fw(this, &PhysicalLayer::nb_transport_fw, 0);
    m_outPort2net.register_nb_transport_bw(this, &PhysicalLayer::nb_transport_bw, 0);
    
    m_inPort_from_router.register_nb_transport_fw(this, &PhysicalLayer::nb_transport_fw, 1); 
    m_outPort2router.register_nb_transport_bw(this, &PhysicalLayer::nb_transport_bw, 1);

    m_send_downwards_flit                = new send_downwards_flit(&m_phy_microscheduler, this);
    m_send_upwards_flit                  = new send_upwards_flit(&m_phy_microscheduler, this);
    m_router_ready                       = new vppnoc_signal;
    *m_router_ready                       = true;
	}

	~PhysicalLayer() 
	{

	}
	//*********************************************************************************
	// 					Interfaces for receiving messages or packets
	//*********************************************************************************

  vppnoc_sync_enum_type  
  nb_transport_fw
  ( int socket_id 
  , vppnoc_transaction& flit_trans
  , vppnoc_phase & phase
  , sc_time & delay
  ) 
  {
 #ifdef DEBUG_DUMP_PHYSICAL
      dumpphysicallayerfile << name()
           << " skt_id = " 
           << socket_id
           << " pkt_id = " 
           << hdr(nocapp_flit_payload, &flit_trans, m_packet_id).get_field()
           << " flit_id = "
           << hdr(nocapp_flit_payload, &flit_trans, m_flit_id).get_field()
           << " src_tile = "
           << hdr(nocapp_flit_payload, &flit_trans, m_src_tile).get_field()
           << " dst_tile = "
           << hdr(nocapp_flit_payload, &flit_trans, m_dst_tile).get_field()
           << " sc_time = "
           << sc_time_stamp()
           << endl;
#endif 
		flit_trans.acquire();
    if (socket_id == 0 ) // downward flits from network layer
      m_phy_microscheduler->notify(flit_trans, delay, m_send_downwards_flit);
    else if (socket_id == 1 ) // upward flits from router
    {
      //m_phy_microscheduler->notify(flit_trans, delay, m_send_upwards_flit);
      vppnoc_sync_enum_type rtrn;
      rtrn = m_outPort2net->nb_transport_fw(flit_trans, phase, delay);
      return rtrn;
    }
    
  	return VPPNOC_ACCEPTED;
	}


  virtual
    vppnoc_sync_enum_type
    nb_transport_bw
    (
       int initiator_id
     , vppnoc_transaction& flit_trans
     , vppnoc_phase& phase
     , sc_time& delay) 
  {
    if (initiator_id == 0 ) {
//      flit_trans.set_status(PAYLOAD_RESOURCE_FREE);
      m_inPort_from_router->nb_transport_bw(// socket_id
                                            flit_trans
                                           , phase
                                           , delay);    
    } else {
      if (flit_trans.get_status() == PAYLOAD_RESOURCE_FREE) { 
        *m_router_ready = true;
        m_phy_microscheduler->notify(m_router_ready);
      }
    }
		return VPPNOC_ACCEPTED;
	}


  class
  send_downwards_flit
  : public 
    vppnoc_process<PhysicalLayer, vppnoc_transaction> 
  {
  public:
    vppnoc_phase  phase;
    sc_time       delay;      

	public:
		cr_context;
		send_downwards_flit( 
      VPPNOCMicroScheduler<PhysicalLayer>**	ma
    , PhysicalLayer* ownr) 
    : vppnoc_process<PhysicalLayer, vppnoc_transaction>(ma, ownr, true) 
		{
      phase = UNINITIALIZED_PHASE;
      delay = sc_time(1, SC_NS);
			cr_init();
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& flit_trans
    ) 
    {
      int channel_no = 0;
      vppnoc_sync_enum_type rtn;
      vppnoc_transaction* p_request_flit_trans = owner->m_flit_mm.allocate();
      cr_start();
      *(owner->m_router_ready) = false;
      rtn = owner->m_outPort2router.nb_transport_fw(channel_no, flit_trans, phase, delay);
      if ( rtn == VPPNOC_ACCEPTED) {
        flit_trans.set_status(PAYLOAD_OK); 
      } else {
        if(*(owner->m_router_ready) == false) {
          wait(flit_trans, delay, owner->m_router_ready, true, this);
          cr_return(1); 
        }
        flit_trans.set_status(PAYLOAD_RESOURCE_ERROR);
        
      }
      cr_reset();      
      owner->m_inPort_from_net->nb_transport_bw( flit_trans
                                               , phase
      
                                               , delay);
      
      cr_end();      
      
		}

	};
 
  class
  send_upwards_flit
  : public 
    vppnoc_process<PhysicalLayer, vppnoc_transaction> 
  {
  public:
    vppnoc_phase  phase;
    sc_time       delay;      

	public:
		cr_context;
		send_upwards_flit( 
      VPPNOCMicroScheduler<PhysicalLayer>**	ma
    , PhysicalLayer* ownr) 
    : vppnoc_process<PhysicalLayer, vppnoc_transaction>(ma, ownr, true) 
		{
      phase = UNINITIALIZED_PHASE;
      delay = sc_time(1, SC_NS);
			cr_init();
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& flit_trans
    ) 
    {
      
      owner->m_outPort2net->nb_transport_fw(flit_trans, phase, delay);
      return 1;
		}

	};

public: 
  typedef VPPNOCMicroScheduler<PhysicalLayer>                         phy_microscheduler;
  /* Ports */
  vppnoc_slave_socket<PhysicalLayer, NO_OF_CHANNELS>                  m_inPort_from_net;
  vppnoc_master_socket<PhysicalLayer, NO_OF_CHANNELS>                 m_outPort2net;
  vppnoc_slave_socket<PhysicalLayer, NO_OF_CHANNELS>                  m_inPort_from_router;
  vppnoc_master_socket<PhysicalLayer, NO_OF_CHANNELS>                 m_outPort2router;

  phy_microscheduler*	                                                m_phy_microscheduler;
  vppnoc_mm<vppnoc_payload_base, nocapp_packet_payload>               m_packet_mm;
  vppnoc_mm<vppnoc_payload_base, nocapp_flit_payload>                 m_flit_mm;
  vppnoc_process<PhysicalLayer, vppnoc_transaction>*                  m_send_downwards_flit;
  vppnoc_process<PhysicalLayer, vppnoc_transaction>*                  m_send_upwards_flit;
  vppnoc_signal*                                                      m_router_ready;

};


}

#endif /*PHYSICAL_LAYER_H_*/
