#pragma once
#ifndef __CONSUMER_LAYER_H__
#define __CONSUMER_LAYER_H__

#include <list>
#include <systemc>
#include "vppnoc_core.h"
#include "utilities.h"
#include "payload.h"

#define LENGTH 10

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace vppnoc;
extern double total_time;

namespace nocapp {

class consumer : 
	public vppnoc_module {
	
public:

  typedef VPPNOCMicroScheduler<consumer>  trans_microscheduler;

	consumer
  (  sc_module_name name )
   :  vppnoc_module(name, 0, IO_TILE)
	{
		m_trans_microscheduler            = new (trans_microscheduler)("TrnLyrMng", 0, this);
    m_consume_process                 = new consume_process(&m_trans_microscheduler, this);

    m_inPort_from_producer.register_nb_transport_fw(this, &consumer::nb_transport_fw, 0);
  
	}


	~consumer() {
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
//       m_trans_microscheduler->notify(trans, delay, m_packet2msg);
    } else if (socket_id == 1) {


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
//    m_trans_microscheduler->notify(packet_trans, delay, m_ack_backward);
  
		return VPPNOC_ACCEPTED;
	}


	
	
  virtual void start_of_simulation() {
    sc_time t = SC_ZERO_TIME;
    
//    vppnoc_payload_base* message_payload = m_message_mm.allocate();
    
//    if (get_id() == 1) 
    {
//      m_trans_microscheduler->notify(*message_payload, t, m_msg2packet);
    }
  }

  class
  consume_process
  : public 
    vppnoc_process<consumer, vppnoc_transaction> 
  {
	public:
//    vppnoc_transaction* p_message_trans;
    vppnoc_sync_enum_type rtrn;
    int packet_id;
	public:
		cr_context;
		consume_process( 
      VPPNOCMicroScheduler<consumer>**	ma
    , consumer* ownr) 
    : vppnoc_process<consumer, vppnoc_transaction>(ma, ownr, true) 
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

    return 0;
		}

	};
	std::list<vppnoc_transaction*>*                                  m_input_buffer;
	/* Ports */
  vppnoc_slave_socket<consumer, 1>                                 m_inPort_from_producer;
	VPPNOCMicroScheduler<consumer, vppnoc_transaction>*	             m_trans_microscheduler;
  vppnoc_process<consumer, vppnoc_transaction>*                    m_consume_process;
};

}

#endif /*__NOCAPP_TRANSPORT_LAYER_H__*/

