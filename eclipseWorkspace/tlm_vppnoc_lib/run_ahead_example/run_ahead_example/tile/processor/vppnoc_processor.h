#pragma once
#ifndef __NOCAPP_PROCESSOR_H__
#define __NOCAPP_PROCESSOR_H__


#include <systemc>
#include "vppnoc_core.h"
#include "include/nocapp_utilities.h"
#include "protocol_data_unit/nocapp_message_payload.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace vppnoc;


namespace nocapp {

class nocapp_processor : 
	public vppnoc_module {
	
public:
	nocapp_processor
  (  sc_module_name name
   , int id
	 , TileTypeEnum ty)
   :  vppnoc_module(name, id, ty)
	{
    m_outPort2nibuff(*this);
    m_outPort_interrupt(*this);
    m_p_processor_microscheduler       = new (processor_microscheduler)("processor", id, this);
    m_p_cpu = new cpu(&m_p_processor_microscheduler, this);
  }


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
  
  virtual void start_of_simulation() {
    sc_time delay = sc_time(1, SC_NS);//SC_ZERO_TIME;
    
    vppnoc_payload_base* message_trans = m_message_mm.allocate();
    message_trans->acquire();
    message_trans->set_time_stamp(sc_time_stamp());

//    if (  false
//            || get_id() == 0 
//            || get_id() == 1
//            || get_id() == 2
//            || get_id() == 3
//            || get_id() == 4
//            || get_id() == 5
//            || get_id() == 6
//            || get_id() == 7
//            || get_id() == 12
//       )
    {
      m_p_processor_microscheduler->notify(*message_trans, delay, m_p_cpu);
    }
  }
  
  class
  cpu
  : public 
    vppnoc_process<nocapp_processor, vppnoc_transaction> 
  {
	public:
    vppnoc_sync_enum_type rtrn;
    int i;
	public:
	  cr_context;
		cpu( 
      VPPNOCMicroScheduler<nocapp_processor>**	ma
    , nocapp_processor* ownr) 
    : vppnoc_process<nocapp_processor, vppnoc_transaction>(ma, ownr, false) 
		{
			cr_init();
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& message_trans
    ) 
    {
		  sc_core::sc_time t = sc_core::sc_time(5, sc_core::SC_NS);
      

      (message_trans).set_payload_kind(MESSAGE_PAYLOAD);
      (message_trans).set_transaction_command(WRITE);
		  //setheaderfield(nocapp::nocapp_message_payload, msg1, m_cmd, nocapp::TASK_RLS_REQ);
		  hdr(nocapp::nocapp_message_payload, &message_trans, m_cmd).set_field(nocapp::TASK_RLS_REQ);
		  hdr(nocapp::nocapp_message_payload, &message_trans, m_message_id).set_field(1);
		  hdr(nocapp::nocapp_message_payload, &message_trans, m_src_core).set_field(1);
		  hdr(nocapp::nocapp_message_payload, &message_trans, m_cast).set_field(nocapp::UNI_CAST);
		  hdr(nocapp::nocapp_message_payload, &message_trans, m_dst_tile).set_field((owner->get_id()+2)%(NO_ROUTER_PORT));
		  hdr(nocapp::nocapp_message_payload, &message_trans, m_src_tile).set_field(owner->get_id());


//		  std::cout << "I am master " << std::endl;
      vppnoc_phase phase = BEGIN_REQ;
 			sc_time delay = sc_core::sc_time(1, sc_core::SC_NS);
      
      cr_start();
      i = 0;
      while (i < 2) {
		    rtrn = owner->m_outPort2nibuff->nb_transport_fw(message_trans, phase, delay);
        if (rtrn == VPPNOC_REJECTED)
        {
//          std::cout << "Buffer Full " << std::endl;
          wait(message_trans, delay, this);
          cr_return(1);
        } else
        {
          owner->m_outPort_interrupt->nb_transport_fw(message_trans, phase, delay);
          i++;
//          std::cout << "Buffer WRITE " << i <<std::endl;
        }
      }
      cr_end();
      return 1;
    }
  };  
  
public:
  typedef VPPNOCMicroScheduler<nocapp_processor>                            processor_microscheduler;
  vppnoc_master_socket<nocapp_processor, NO_OF_CHANNELS>                    m_outPort2nibuff;
  vppnoc_master_socket<nocapp_processor, NO_OF_CHANNELS>                    m_outPort_interrupt;
  vppnoc_mm<vppnoc_payload_base, nocapp_message_payload>                    m_message_mm;	
  vppnoc_process<nocapp_processor, vppnoc_transaction>*                     m_p_cpu;
  VPPNOCMicroScheduler<nocapp_processor, vppnoc_transaction>*	              m_p_processor_microscheduler;
};

}

#endif /*__NOCAPP_PROCESSOR_H__*/


