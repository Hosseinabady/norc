#pragma once
#ifndef __NOCAPP_TRANSPORT_LAYER_H__
#define __NOCAPP_TRANSPORT_LAYER_H__

#include <list>
#include <systemc>
#include "vppnoc_core.h"
#include "include/nocapp_utilities.h"
#include "protocol_data_unit/nocapp_message_payload.h"
#include "tile/network_interface/nocapp_ni_fifo_buffer.h"
#include <math.h>

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace vppnoc;
extern double total_time;

namespace nocapp {

class TransportLayer : 
	public NoCProtocolLayer {
	
public:

  typedef VPPNOCMicroScheduler<TransportLayer>  trans_microscheduler;

	TransportLayer
  (  sc_module_name name
   , int id
	 , TileTypeEnum ty)
   :  NoCProtocolLayer(name, id, ty)
	{
		m_trans_microscheduler       = new (trans_microscheduler)("TrnLyrMng", id, this);
    m_msg2packet                 = new msg2packet(&m_trans_microscheduler, this);
    m_packet2msg                 = new packet2msg(&m_trans_microscheduler, this);
    m_ack_backward               = new ack_backward(&m_trans_microscheduler, this);
    m_network_layer_ready        = new vppnoc_signal;
    m_interrupt                  = new vppnoc_signal;
    *(m_interrupt)               = false;

    m_inPort_from_net.register_nb_transport_fw(this, &TransportLayer::nb_transport_fw, 0);
    m_inPort_interrupt.register_nb_transport_fw(this, &TransportLayer::nb_transport_fw, 1);
    
    m_outPort2net.register_nb_transport_bw(this, &TransportLayer::nb_transport_bw, 1);
    m_outPort2buf.register_nb_transport_bw(this, &TransportLayer::nb_transport_bw, 2);


	}


	~TransportLayer() {
		

	}

	//*********************************************************************************
	// 					Interfaces for receiving messages or packets
	//*********************************************************************************
	virtual
  vppnoc_sync_enum_type  
    nb_transport_fw
    (int socket_id
     , vppnoc_transaction& trans
     , vppnoc_phase & phase
     , sc_core::sc_time & delay
     ) 
  {

    if (socket_id == 0)
    {
       m_trans_microscheduler->notify(trans, delay, m_packet2msg);
    } else if (socket_id == 1) {
      *(m_interrupt) = true;
      m_trans_microscheduler->notify(m_interrupt);     
    }
    return VPPNOC_ACCEPTED;
	}

  virtual
    vppnoc_sync_enum_type
    nb_transport_bw
    (
       int socket_id
     , vppnoc_transaction& packet_trans
     , vppnoc_phase& phase
     , sc_time& delay) 
  {
    m_trans_microscheduler->notify(packet_trans, delay, m_ack_backward);
  
		return VPPNOC_ACCEPTED;
	}


	
	
  virtual void start_of_simulation() {
    sc_time t = SC_ZERO_TIME;
    
    vppnoc_payload_base* message_payload = m_message_mm.allocate();
    
//    if (get_id() == 1) 
    {
      m_trans_microscheduler->notify(*message_payload, t, m_msg2packet);
    }
  }

  class
  msg2packet
  : public 
    vppnoc_process<TransportLayer, vppnoc_transaction> 
  {
	public:
//    vppnoc_transaction* p_message_trans;
    vppnoc_sync_enum_type rtrn;
    int packet_id;
	public:
		cr_context;
		msg2packet( 
      VPPNOCMicroScheduler<TransportLayer>**	ma
    , TransportLayer* ownr) 
    : vppnoc_process<TransportLayer, vppnoc_transaction>(ma, ownr, true) 
		{
			cr_init();
//      p_message_trans = owner->m_message_mm.allocate();
      packet_id = 1;
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& message_trans
    ) 
    {
      int channel_no = 0;

			sc_time delay = sc_core::sc_time(1, sc_core::SC_NS);
      vppnoc_phase phase = BEGIN_REQ;
      
			cr_start();
			while(1) 
			{
        if (*(owner->m_interrupt) == false) {
          wait(message_trans, delay, owner->m_interrupt, true, this);
          cr_return(1);
        }
        message_trans.set_payload_kind(MESSAGE_PAYLOAD);
        message_trans.set_transaction_command(READ);

        rtrn = owner->m_outPort2buf.nb_transport_fw(channel_no, message_trans, phase, delay);
        
        if (rtrn == VPPNOC_UPDATED)
        {
          while (rtrn == VPPNOC_UPDATED) {
            rtrn = VPPNOC_UPDATED;
            vppnoc_transaction* p_packet_trans;
            p_packet_trans = owner->m_packet_mm.allocate();
            p_packet_trans->acquire();


            p_packet_trans->set_payload_kind(PACKET_PAYLOAD);
            p_packet_trans->set_transaction_command(READ);
            p_packet_trans->set_time_stamp(sc_time_stamp());
            rtrn = owner->m_outPort2buf.nb_transport_fw(channel_no, *p_packet_trans, phase, delay);
            if (rtrn == VPPNOC_UPDATED) {
              *(owner->m_network_layer_ready) = false;
  
              owner->send_packet(&message_trans, p_packet_trans);
              packet_id++;
              wait(message_trans, delay, owner->m_network_layer_ready, true, this);
              cr_return(1);
            } else {
              message_trans.set_payload_kind(MESSAGE_PAYLOAD);
              message_trans.set_transaction_command(POP);

              owner->m_outPort2buf.nb_transport_fw(message_trans, phase, delay);
              packet_id = 1;
            }
          }
        } else {
          *(owner->m_interrupt) = false;
//          cout << " VPPNOC_REJECTED " << sc_core::sc_time_stamp() <<endl;
        }

			}
			cr_end();
		}

	};

class
  packet2msg
  : public 
    vppnoc_process<TransportLayer, vppnoc_transaction> 
  {
	public:
//    vppnoc_transaction* p_message_trans;
    vppnoc_sync_enum_type rtrn;
    int packet_id;
	public:
		cr_context;
		packet2msg( 
      VPPNOCMicroScheduler<TransportLayer>**	ma
    , TransportLayer* ownr) 
    : vppnoc_process<TransportLayer, vppnoc_transaction>(ma, ownr, true) 
		{
			cr_init();
//      p_message_trans = owner->m_message_mm.allocate();
      packet_id = 1;
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& packet_trans
    ) 
    {
      int channel_no = 0;                             

			sc_core::sc_time delay = sc_core::sc_time(1, sc_core::SC_NS);
      vppnoc_phase phase = BEGIN_REQ;

      packet_trans.set_payload_kind(PACKET_PAYLOAD);
      packet_trans.set_transaction_command(WRITE);

      
      owner->m_outPort2buf->nb_transport_fw( packet_trans
                                           , phase
                                           , delay);
      PacketType packet_t = hdr(nocapp_packet_payload, &packet_trans, m_packet).get_field();
      if (packet_t == LAST_PACKET) {
/*        simulation_timer.stopTimer();
        total_time += simulation_timer.getElapsedTime();
        std::cout << "latency time   = " << total_time << std::endl;
        simulation_timer.startTimer();
        cout << " tile "
             << owner->get_id()
             << " at "
             << " systemc time = " << sc_time_stamp()
             << endl;*/
      }
  

      return 1;
		}

	};

  
  int send_packet 
  (
    vppnoc_transaction* p_message_trans
  , vppnoc_transaction* p_packet_trans
  ) 
  {
    int channel_no = 0;

    int packet_id = 0; //****************************
    int next_tile = 0; //****************************


    sc_time delay;
    {
		  unsigned int src_core = hdr(nocapp_message_payload, p_message_trans, m_src_core).get_field();
		  hdr(nocapp_packet_payload, p_packet_trans, m_src_core).set_field(src_core);
		  unsigned int dst_tile = hdr(nocapp_message_payload, p_message_trans, m_dst_tile).get_field();
		  hdr(nocapp_packet_payload, p_packet_trans, m_dst_tile).set_field(dst_tile);
		  unsigned int src_tile = hdr(nocapp_message_payload, p_message_trans, m_src_tile).get_field();
		  hdr(nocapp_packet_payload, p_packet_trans, m_src_tile).set_field(src_tile);
		  CastingType cast     = hdr(nocapp_message_payload, p_message_trans, m_cast).get_field();
		  hdr(nocapp_packet_payload, p_packet_trans, m_cast).set_field(cast);
		  if (cast == UNI_CAST && 
		     (cast == ACK_SEEK || 
  		    cast == TASK_ACTIVE_REQ || 
	    		cast == TASK_ACTIVE_ACK ||
			  	cast == TASK_CMD_START  ||
			    cast == WRITE_DATA) ||
			    cast == TASK_RLS_REQ || 
			    cast == TASK_RLS_ACK ) 
		  {
			  hdr(nocapp_packet_payload, p_packet_trans, m_routing).set_field(XY);
		  }
		  MessageCmd msg_cmd = hdr(nocapp_message_payload, p_message_trans, m_cmd).get_field();
		  if (cast == BRD_CAST && msg_cmd == TASK_SEEK) {
			  hdr(nocapp_packet_payload, p_packet_trans, m_routing).set_field(XY);//DIR_FLOODING;//PUR_FLOODING;
			  hdr(nocapp_packet_payload, p_packet_trans, m_dst_tile).set_field(next_tile);
		  }
  		sc_core::sc_time delay = sc_core::sc_time(TRANSPORT_TO_NETWORK_DELAY, sc_core::SC_NS);
		  int out_channel = hdr(nocapp_packet_payload, p_packet_trans, m_src_core).get_field();
      vppnoc_phase  phase = BEGIN_REQ;
      vppnoc_sync_enum_type rtrn;
      
      rtrn =  m_outPort2net.nb_transport_fw(out_channel, *p_packet_trans, phase, delay);
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
    vppnoc_process<TransportLayer, vppnoc_transaction> 
  {
  public:
    vppnoc_sync_enum_type rtrn;
    vppnoc_transaction* p_flit_trans;
    
	public:
		cr_context;
		ack_backward( 
      VPPNOCMicroScheduler<TransportLayer>**	ma
    , TransportLayer* ownr) 
    : vppnoc_process<TransportLayer, vppnoc_transaction>(ma, ownr, false) 
		{
			cr_init();
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& packet_trans
    ) 
    {

      *(owner->m_network_layer_ready) = true;
       owner->m_trans_microscheduler->notify(owner->m_network_layer_ready);
      return 1;
		}

  };


private: //Functions
	bool find_volunteer(Volunteer* vl, int tid) {
		Volunteer* vl_tmp = vl;
		for (int i = 0; i <TASK_SEEK_LIFE_TIME; i++ ) {
			if (vl_tmp->tile_id.get_field() == tid)
				return 1;
			vl_tmp = vl_tmp++;
		}
		return 0;
	}
private:

	std::list<vppnoc_transaction*>*                           m_input_buffer;
	enum TRANSPORT_NIPartEnum {TRANSPORT_DOWN_STREAM_FW,
							 TRANSPORT_DOWN_STREAM_BW,
							 TRANSPORT_UP_STREAM_FW,
							 TRANSPORT_UP_STREAM_BW,
							 TRANSPORT_MAIN_ACTION_WAIT,
							 TRANSPORT_WAIT_LIST_ACTIVE,
							 TRANSPORT_INPUT_BUFF,
							 TRANSPORT_INPUT_RELEASE
							};
	typedef TRANSPORT_NIPartEnum transport_ni_part_type;

	
	bool                                        m_buffer_used[NO_OF_CHANNELS];



	std::map<transport_ni_part_type, unsigned int>                m_action_label;
	//OLD variables




	sc_core::sc_time                                mTransportLayerDelay;


	


public:	
struct DynamicTask {
		NoCTask*        task;
		ActivityType    activity;
	};
//	DynamicTask reconfigurable_area[NO_OF_RECONFIGURABLE_CORE];



public: //Functions
	int get_tile_id() {
		return get_id();
	}



public:
  
	/* Ports */
  vppnoc_slave_socket<TransportLayer, NO_OF_CHANNELS>                    m_inPort_from_net;
  vppnoc_slave_socket<TransportLayer, NO_OF_CHANNELS>                    m_inPort_interrupt;
  vppnoc_master_socket<TransportLayer, NO_OF_CHANNELS>                   m_outPort2net;
  vppnoc_master_socket<TransportLayer, NO_OF_CHANNELS>                   m_outPort2buf;
	VPPNOCMicroScheduler<TransportLayer, vppnoc_transaction>*	             m_trans_microscheduler;
	/* Functors */
  vppnoc_process<TransportLayer, vppnoc_transaction>*                    m_msg2packet;
  vppnoc_process<TransportLayer, vppnoc_transaction>*                    m_packet2msg;
  vppnoc_process<TransportLayer, vppnoc_transaction>*                    m_ack_backward;

	vppnoc_mm<vppnoc_payload_base, nocapp_packet_payload>                  m_packet_mm;
  vppnoc_mm<vppnoc_payload_base, nocapp_message_payload>                 m_message_mm;

  struct sending_message {
		sending_message
    (
      vppnoc_transaction* msg
    , bool snd_flg
    ) 
    : message(msg)
    , send_flag(snd_flg) 
    {
		}
		vppnoc_transaction*          message;
		bool                                 send_flag;
	};
  vector<sending_message>                                                m_input_message_buffer[NO_OF_CHANNELS];
  int                                                                    m_output_message_index[NO_OF_CHANNELS];

  vppnoc_transaction*                                                    m_p_sending_packet_trans[NO_OF_CHANNELS];
  vppnoc_signal*                                                         m_network_layer_ready;
  vppnoc_signal*                                                         m_interrupt;


	
};

}

#endif /*__NOCAPP_TRANSPORT_LAYER_H__*/

