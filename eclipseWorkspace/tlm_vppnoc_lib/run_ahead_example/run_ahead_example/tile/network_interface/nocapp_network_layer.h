//******************************************************************************
//			Electrical Engineering Departmenet
//			University of Bristol
//
//			Author:   Mohammad Hosseinabady
//			Email:    M.Hosseinabady@bristol.ac.uk
//
//			Date:     12 September 2008
//******************************************************************************
#pragma once
#ifndef NETWORK_LAYER_H_
#define NETWORK_LAYER_H_

#include<list>
#include "vppnoc_core.h"
#include "include/nocapp_utilities.h"

#include "protocol_data_unit/nocapp_message_payload.h"
#include <math.h>
#include <fstream>

extern ofstream dumpnetworklayerfile;

namespace nocapp {

#include <map>
using namespace std;
using namespace vppnoc;
using namespace sc_core;
using namespace sc_dt;


class 
NetworkLayer 
:public NoCProtocolLayer {
public:
	NetworkLayer
	(
	   sc_module_name name
	 , int id
	 , TileType ty
	 ) :
	   NoCProtocolLayer(name, id, ty)
   , m_inPort_from_trans("inPort_from_trans")
   , m_outPort2trans("outPort2trans")
   , m_inPort_from_phy("inPort_from_phy")
   , m_outPort2phy("outPort2phy")
   , m_outPort2buf("outPort2buf")
	 , mNetworkLayerDelay(1, SC_NS)
	{
    m_inPort_from_trans.register_nb_transport_fw(this, &NetworkLayer::nb_transport_fw, 0);
    m_outPort2trans.register_nb_transport_bw(this, &NetworkLayer::nb_transport_bw, 0);

    m_inPort_from_phy.register_nb_transport_fw(this, &NetworkLayer::nb_transport_fw, 1);
    m_outPort2phy.register_nb_transport_bw(this, &NetworkLayer::nb_transport_bw, 1);

    m_outPort2buf.register_nb_transport_bw(this, &NetworkLayer::nb_transport_bw, 2);
    
    m_net_microscheduler      = new (net_microscheduler)("NetLyrMng", id, this);


    m_packet2flit     = new packet2flit(&m_net_microscheduler, this);
    m_flit2packet     = new flit2packet(&m_net_microscheduler, this);
    m_ack_backward    = new ack_backward(&m_net_microscheduler, this);
    m_flit_sent       = new vppnoc_signal;
		for (int i = 0; i < NO_OF_CHANNELS; i++) {
			m_channel_state[i]           = CHANNEL_IDLE;
			m_output_packet_index[i]     = -1;
		}
	}

	~NetworkLayer() {

	
		}
	//*********************************************************************************
	// 					Interfaces for receiving messages or packets
	//*********************************************************************************

	vppnoc_sync_enum_type  
    nb_transport_fw
    ( int socket_id
     , vppnoc_transaction& trans
     , vppnoc_phase & phase
     , sc_time & delay
    ) 
  {
#ifdef DEBUG_DUMP_NET
    if (socket_id == 1) 
      dumpnetworklayerfile << name()
           << " packet_id = " 
           << hdr(nocapp_flit_payload, &trans, m_packet_id).get_field()
           << " flit_id = "
           << hdr(nocapp_flit_payload, &trans, m_flit_id).get_field()
           << " src_tile = "
           << hdr(nocapp_flit_payload, &trans, m_src_tile).get_field()
           << " dst_tile = "
           << hdr(nocapp_flit_payload, &trans, m_dst_tile).get_field()
           << " socket_id = "
           << socket_id
           << " sc_time = "
           << sc_time_stamp()
           << endl;
    else
      dumpnetworklayerfile << name()
           << " packet_id = " 
           << hdr(nocapp_packet_payload, &trans, m_packet_id).get_field()
           << " dst_tile = "
           << hdr(nocapp_packet_payload, &trans, m_dst_tile).get_field()
           << " socket_id = "
           << socket_id
           << " sc_time = "
           << sc_time_stamp()
           << endl;

#endif
    trans.acquire();
    if (socket_id == 0)    // Downwards path, packets from transport layer
    {
      m_net_microscheduler->notify(trans, delay,m_packet2flit);
    } else if (socket_id == 1)  // Upwards path, flits from physical layer
    {
      trans.set_payload_kind(FLIT_PAYLOAD);
      trans.set_transaction_command(CONTROL_FREE_CHANNEL);
      vppnoc_sync_enum 
      rtrn = m_outPort2buf->nb_transport_fw( trans
                                                  , phase
                                                  , delay);
      if (rtrn == VPPNOC_ACCEPTED)
        m_net_microscheduler->notify(trans, delay, m_flit2packet);
      return rtrn;      
    }
    return VPPNOC_ACCEPTED;
  }

  virtual
    vppnoc_sync_enum_type
    nb_transport_bw
    (
       int socket_id
     , vppnoc_transaction& flit_trans
     , vppnoc_phase& phase
     , sc_time& delay) 
  {                                  

//      m_net_microscheduler->notify(flit_trans, delay, m_ack_backward);

      *(m_flit_sent) = true;
      m_net_microscheduler->notify(m_flit_sent);

   
		return VPPNOC_ACCEPTED;
	}

  virtual void start_of_simulation() {
   }
private:

  class
  packet2flit
  : public 
    vppnoc_process<NetworkLayer, vppnoc_transaction> 
  {
  public:
    vppnoc_sync_enum_type rtrn;
    vppnoc_transaction* p_flit_trans;
    int index_tmp;
    
	public:
		cr_context;
		packet2flit( 
      VPPNOCMicroScheduler<NetworkLayer>**	ma
    , NetworkLayer* ownr) 
    : vppnoc_process<NetworkLayer, vppnoc_transaction>(ma, ownr, true) 
		{
			cr_init();

		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& packet_trans
    ) 
    {
      int channel_no = 0;
      

      
			sc_time delay = sc_time(1, SC_NS);
      vppnoc_phase phase = BEGIN_REQ;
      packet_trans.acquire();
			cr_start();

      rtrn = VPPNOC_UPDATED;
      while (rtrn == VPPNOC_UPDATED) {
        p_flit_trans = owner->m_flit_mm.allocate();
        p_flit_trans->acquire();
        p_flit_trans->set_payload_kind(FLIT_PAYLOAD);
        p_flit_trans->set_transaction_command(READ);
        p_flit_trans->set_time_stamp(sc_time_stamp());
         
        rtrn = owner->m_outPort2buf.nb_transport_fw(channel_no, *p_flit_trans, phase, delay);

        if (rtrn == VPPNOC_UPDATED) {
//          *(owner->m_flit_sent) = false;
          index_tmp = 0;
          do {
            *(owner->m_flit_sent) = false;
//            cout << owner->name() << " " << index_tmp++ << " " << sc_time_stamp() << endl;
            owner->send_flit(p_flit_trans);
            if (*(owner->m_flit_sent) == false) {
              wait(packet_trans, delay, owner->m_flit_sent, true, this);
              cr_return(1);
            }
          } while(p_flit_trans->get_status() == PAYLOAD_RESOURCE_ERROR);

        } else {
           
        }
      }
      if (rtrn != VPPNOC_UPDATED) {
        packet_trans.set_time_stamp(sc_time_stamp());
        owner->m_inPort_from_trans->nb_transport_bw(// socket_id
                                          packet_trans
                                        , phase
                                        , delay);
      }
      
			cr_end();
      return 1;
		}

	};


  class
  flit2packet
  : public 
    vppnoc_process<NetworkLayer, vppnoc_transaction> 
  {
  public:
    vppnoc_sync_enum_type rtrn;
    vppnoc_transaction* p_flit_trans;
    vppnoc_transaction* p_packet_trans;
    
	public:
		cr_context;
		flit2packet( 
      VPPNOCMicroScheduler<NetworkLayer>**	ma
    , NetworkLayer* ownr) 
    : vppnoc_process<NetworkLayer, vppnoc_transaction>(ma, ownr, true) 
		{
			cr_init();
      p_packet_trans = owner->m_packet_mm.allocate();

		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& flit_trans
    ) 
    {
      int channel_no = 0;

			sc_time delay = sc_time(1, SC_NS);
      vppnoc_phase phase = BEGIN_REQ;
      flit_trans.acquire();
      flit_trans.set_payload_kind(FLIT_PAYLOAD);
      flit_trans.set_transaction_command(WRITE);
      
      
            
      rtrn = owner->m_outPort2buf->nb_transport_fw( flit_trans
                                                  , phase
                                                  , delay);
      if (rtrn == VPPNOC_UPDATED) {
        flit_trans.set_status(PAYLOAD_OK);
        flit_type flit_t = hdr(nocapp_flit_payload, &flit_trans, m_flit).get_field();
       
        if (flit_t == LAST_FLIT) {
          unsigned int src_t = hdr(nocapp_flit_payload, &flit_trans, m_src_tile).get_field();
          hdr(nocapp_packet_payload, p_packet_trans, m_src_tile).set_field(src_t);          
          owner->m_outPort2trans->nb_transport_fw( *p_packet_trans
                                                 , phase
                                                 , delay);
        }
      } else {
        flit_trans.set_status(PAYLOAD_RESOURCE_ERROR);
      }
      owner->m_inPort_from_phy->nb_transport_bw(flit_trans, phase, delay);
      return 1;
		}

	};


  
  int send_flit 
  (
    vppnoc_transaction* p_flit_trans
  ) 
  {
    int channel_no = 0;

    
    int next_tile = 0; //****************************


    sc_time delay;
    {
 		
  		sc_time delay = sc_time(TRANSPORT_TO_NETWORK_DELAY, SC_NS);
  
      vppnoc_phase  phase = BEGIN_REQ;
      vppnoc_sync_enum_type rtrn;
      
			rtrn =  m_outPort2phy.nb_transport_fw(channel_no, *p_flit_trans, phase, delay);
      if (rtrn != VPPNOC_ACCEPTED)
      {
        cout << " Transport Layer sending packety Error " << endl;
      }
		}
		return 1; 
  }
	

  class
  ack_backward
  : public 
    vppnoc_process<NetworkLayer, vppnoc_transaction> 
  {
  public:
    vppnoc_sync_enum_type rtrn;
    vppnoc_transaction* p_flit_trans;
    
	public:
		cr_context;
		ack_backward( 
      VPPNOCMicroScheduler<NetworkLayer>**	ma
    , NetworkLayer* ownr) 
    : vppnoc_process<NetworkLayer, vppnoc_transaction>(ma, ownr, true) 
		{
			cr_init();

		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& packet_trans
    ) 
    {
      *(owner->m_flit_sent) = true;
      owner->m_net_microscheduler->notify(owner->m_flit_sent);

      return 1;
		}

	};



	enum SendFlitOutStateEnum   {SEND_OUT_IDLE,
								 SEND_OUT_WAIT,
								} ;
	typedef SendFlitOutStateEnum                        SendFlitOutStateType;

	enum NETWORK_NIPartEnum {NETWORK_DOWN_STREAM_FW,
							 NETWORK_DOWN_STREAM_BW,
							 NETWORK_UP_STREAM_FW,
							 NETWORK_UP_STREAM_BW,
							 NETWORK_MAIN_ACTION_WAIT,
							 NETWORK_WAIT_LIST_ACTIVE,
							 NETWORK_INPUT_BUFF,
							 NETWORK_SEND_FLIT_OUT
							};
	typedef NETWORK_NIPartEnum network_ni_part_type;
	

	sc_time           mNetworkLayerDelay;
	
	
	map<network_ni_part_type, unsigned int>                      m_action_label;
	int				                                                   m_tile_id;
	TileType		                                                 m_tile_type;
	SendFlitOutStateType*                                        m_sent_flit_output_state;
	
	vppnoc_transaction*                                          m_p_packet_out_trans;
	map<int, int>				                                         m_channel_to_session;//m_response_channel_table;

	int*                                                         m_input_buffer_length;

public:
	typedef VPPNOCMicroScheduler<NetworkLayer>                   net_microscheduler;
	vppnoc_slave_socket<NetworkLayer, NO_OF_CHANNELS>            m_inPort_from_trans;
  vppnoc_master_socket<NetworkLayer, NO_OF_CHANNELS>           m_outPort2trans;

  vppnoc_slave_socket<NetworkLayer, NO_OF_CHANNELS>            m_inPort_from_phy;
	vppnoc_master_socket<NetworkLayer, NO_OF_CHANNELS>           m_outPort2phy;
  
  vppnoc_master_socket<NetworkLayer, NO_OF_CHANNELS>           m_outPort2buf;

	list<vppnoc_transaction*>*                                   m_input_buffer;
	ChannelStateType                                             m_channel_state[NO_OF_CHANNELS];
	net_microscheduler*	                                         m_net_microscheduler;
	struct sending_packet {
		sending_packet(vppnoc_transaction* pkt, bool snd_flg) : packet(pkt), send_flag(snd_flg) {
			
		}
		vppnoc_transaction*          packet;
		bool                                 send_flag;
	};
	vector<sending_packet>                                       m_input_packet_buffer[NO_OF_CHANNELS];
  int                                                          m_output_packet_index[NO_OF_CHANNELS];
	vppnoc_process<NetworkLayer, vppnoc_transaction>*            m_sendout_packets;
	
	vppnoc_mm<vppnoc_payload_base, nocapp_flit_payload>          m_flit_mm;
  vppnoc_mm<vppnoc_payload_base, nocapp_packet_payload>        m_packet_mm;
  vppnoc_process<NetworkLayer, vppnoc_transaction>*            m_downstream_fw;
  vppnoc_signal*                                               m_flit_sent;
  vppnoc_process<NetworkLayer, vppnoc_transaction>*            m_packet2flit;
  vppnoc_process<NetworkLayer, vppnoc_transaction>*            m_flit2packet;
  vppnoc_process<NetworkLayer, vppnoc_transaction>*            m_ack_backward;
};

}
#endif /*NETWORK_LAYER_H_*/

 