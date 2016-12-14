#pragma once
#ifndef __PRODUCER_H__
#define __PRODUCER_H__


#include <systemc>
#include "vppnoc_core.h"
#include "payload.h"
#include "utilities.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace vppnoc;


namespace nocapp {

class producer : 
	public vppnoc_module {
	
public:
	producer
  (  sc_module_name name )
   :  vppnoc_module(name, 0, IO_TILE)
	{
    m_outPort2Consumer(*this);

    m_p_producer_microscheduler       = new (producer_microscheduler)("producer", 0, this);
    m_p_cpu = new cpu(&m_p_producer_microscheduler, this);
  }


   	virtual
  vppnoc_sync_enum_type  
    nb_transport_fw
    (int socket_id
     , vppnoc_transaction& trans
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
     , vppnoc_transaction& r_response_trans
     , vppnoc_phase& phase
     , sc_time& delay) 
  {
   
		return VPPNOC_ACCEPTED;
	}
  
  virtual void start_of_simulation() {
    sc_time delay = sc_time(1, SC_NS);//SC_ZERO_TIME;
    
    vppnoc_payload_base* trans = m_mm.allocate();
    trans->acquire();
    trans->set_time_stamp(sc_time_stamp());

    {
      m_p_producer_microscheduler->notify(*trans, delay, m_p_cpu);
    }
  }
  
  class
  cpu
  : public 
    vppnoc_process<producer, vppnoc_transaction> 
  {
	public:
    vppnoc_sync_enum_type rtrn;
    int i;
	public:
	  cr_context;
		cpu( 
      VPPNOCMicroScheduler<producer>**	ma
    , producer* ownr) 
    : vppnoc_process<producer, vppnoc_transaction>(ma, ownr, false) 
		{
			cr_init();
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& trans
    ) 
    {
		  sc_core::sc_time t = sc_core::sc_time(5, sc_core::SC_NS);
      

      (trans).set_payload_kind(MESSAGE_PAYLOAD);
      (trans).set_transaction_command(WRITE);
		  hdr(payload, &trans, m_value).set_field(1);



//		  std::cout << "I am master " << std::endl;
      vppnoc_phase phase = BEGIN_REQ;
 			sc_time delay = sc_core::sc_time(1, sc_core::SC_NS);
      
      cr_start();
      i = 0;
      while (i < 2) {
		    rtrn = owner->m_outPort2Consumer->nb_transport_fw(trans, phase, delay);
        if (rtrn == VPPNOC_REJECTED)
        {
          wait(trans, delay, this);
          cr_return(1);
        } else
        {
          
          i++;
        }
      }
      cr_end();
      return 1;
    }
  };  
  
public:
  typedef VPPNOCMicroScheduler<producer>                            producer_microscheduler;
  vppnoc_master_socket<producer, 1>                                 m_outPort2Consumer;
  vppnoc_mm<vppnoc_payload_base, payload>                           m_mm;	
  VPPNOCMicroScheduler<producer, vppnoc_transaction>*	              m_p_producer_microscheduler;
  vppnoc_process<producer, vppnoc_transaction>*                     m_p_cpu;
};

}

#endif /*__PRODUCER_H__*/





