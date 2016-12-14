#pragma once
#ifndef TILE_INTERFACE_H_
#define TILE_INTERFACE_H_

#include <systemc>
#include "vppnoc_core.h"
#include "tile/processor/vppnoc_processor.h"
#include "tile/network_interface/nocapp_transport_layer.h"
#include "tile/network_interface/nocapp_network_layer.h"
#include "tile/network_interface/nocapp_physical_layer.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace vppnoc;

namespace nocapp {

using namespace std;

class nocapp_network_interface : public NoCProtocolLayer {

public:

	nocapp_network_interface
  (
    sc_module_name name, int id, TileTypeEnum ty) 
    : NoCProtocolLayer(name, id, ty)
    , m_inPort("m_inPort")
    , m_outPort("m_outPort")
    , m_processor("processor", id, ty) 
  {
		m_tpl = new TransportLayer       ("tpl1", id, ty);
		m_ntl = new NetworkLayer         ("ntl1", id, ty);
		m_phl = new PhysicalLayer        ("phl1", id, ty);
    m_buf = new nocapp_ni_fifo_buffer("buf1", id, ty);
		
    m_tpl->m_outPort2net(m_ntl->m_inPort_from_trans);	
    m_tpl->m_outPort2buf(m_buf->m_inPort_from_trans);

    m_ntl->m_outPort2trans(m_tpl->m_inPort_from_net);
    m_ntl->m_outPort2phy(m_phl->m_inPort_from_net);
    m_ntl->m_outPort2buf(m_buf->m_inPort_from_net);


    (m_inPort)(m_phl->m_inPort_from_router);
    m_phl->m_outPort2router(m_outPort);
    m_phl->m_outPort2net(m_ntl->m_inPort_from_phy);


    m_processor.m_outPort2nibuff(m_buf->m_inPort_from_cpu);

    m_processor.m_outPort_interrupt(m_tpl->m_inPort_interrupt);

		for (int i = 0; i < NO_OF_CHANNELS; i++)
			m_channel_state[i] = CHANNEL_IDLE;

	}

	vppnoc_sync_enum_type  
  nb_transport_fw
  ( int socket_id
  , vppnoc_transaction& message_trans
  , vppnoc_phase & phase
  , sc_time & delay) {
		message_trans.acquire();
		for (int i = 0; i < NO_OF_CHANNELS; i++)
			if (m_channel_state[i] == CHANNEL_IDLE) {
				vppnoc_sync_enum_type rtn = m_buf->nb_transport_fw(i, message_trans, phase, delay);
        m_channel_state[i] = CHANNEL_BUSY;
//				message_trans.release();
				return rtn;
			}
//		message_trans.release();
		return VPPNOC_REJECTED;
	}


  virtual
    vppnoc_sync_enum_type
    nb_transport_bw
    (
       int initiator_id
     , vppnoc_transaction& r_response_message_trans
     , vppnoc_phase& phase
     , sc_time& delay) 
  {
		return VPPNOC_ACCEPTED;
	}


  nocapp_ni_fifo_buffer*                            m_buf;
	TransportLayer*                                   m_tpl;
	NetworkLayer*                                     m_ntl;
	PhysicalLayer*                                    m_phl;
	ChannelStateType                                  m_channel_state[NO_OF_CHANNELS];

  vppnoc_target_socket<NO_OF_CHANNELS>              m_inPort;
  vppnoc_initiator_socket<NO_OF_CHANNELS>           m_outPort;

  nocapp_processor                                  m_processor;
};

}
#endif /*TILE_INTERFACE_H_*/
