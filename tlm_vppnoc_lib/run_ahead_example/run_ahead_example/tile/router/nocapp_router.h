#ifndef  __NOCAPP_ROUTER_H__
#define  __NOCAPP_ROUTER_H__

#include "vppnoc_core.h"
#include "include/nocapp_utilities.h"
#include "protocol_data_unit/nocapp_flit_payload.h"
#include <fstream>

extern std::ofstream dumprouterlayerfile;
                              
namespace nocapp {
using namespace std;
using namespace vppnoc;
using namespace sc_core;
using namespace sc_dt;


class 
nocapp_router 
: public vppnoc_module {

public:
  friend class input_buffer;
  nocapp_router 
  ( sc_module_name name
	, int id
	, TileType ty
	) : vppnoc_module(name, id, ty)
	{
    m_router_microscheduler      = new (router_microscheduler)("RouterMng", id, this);
    
    for (int i = 0; i < NO_ROUTER_PORT; i++)
    {
      m_output_port_ready[i] =new vppnoc_signal;
      m_output_port_free[i]  = new vppnoc_signal;
      *(m_output_port_ready[i]) = true;
      *(m_output_port_free[i]) = true;
      for (int j = 0; j < NO_ROUTER_PORT; j++){
        m_switch[i][j] = false;  
      }
    }

     setLocation(NoCPosition(0,0));	

		for (int i = 0; i < NO_ROUTER_PORT; i++) {
      m_routing[i] = new routing(&m_router_microscheduler, this, i);
      m_buffering[i] = new buffering(&m_router_microscheduler, this, i);
      m_acknowledging[i] = new acknowledging(&m_router_microscheduler, this, i);
      m_inPort[i].register_nb_transport_fw(this, &nocapp_router::nb_transport_fw, i);
      m_outPort[i].register_nb_transport_bw(this, &nocapp_router::nb_transport_bw, i);
      m_input_buffer[i] = new input_buffer;
      

      for (int j = 0; j < ROUTER_BUFFER_LENGTH; j++)
        m_input_buffer[i]->buffer[j] = m_flit_mm.allocate();
    }
	}

	~nocapp_router() 
  {
	
	}

  vppnoc_sync_enum_type  
  nb_transport_fw
  ( int socket_id
  , vppnoc_transaction& flit_trans
  , vppnoc_phase & phase
  , sc_time & delay
  ) 
  {
#ifdef DEBUG_DUMP_ROUTER
      dumprouterlayerfile << name()
           << " packet_id = " 
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
    if (!*(m_input_buffer[socket_id]->full)) {
      m_input_buffer[socket_id]->push(flit_trans);
      m_router_microscheduler->notify(flit_trans, delay, m_buffering[socket_id]);
   

      return VPPNOC_ACCEPTED;
    } else {
      return VPPNOC_REJECTED;
    }
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
#ifdef DEBUG_DUMP_ROUTER
      dumprouterlayerfile << name()
           << " nb_transport_bw "
           << " packet_id = " 
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
    m_router_microscheduler->notify(flit_trans, delay, m_acknowledging[socket_id]);
		return VPPNOC_ACCEPTED;
	}

  virtual void start_of_simulation() {
    sc_time t = SC_ZERO_TIME;
  

    vppnoc_payload_base* p_flit_payload = m_flit_mm.allocate();
    for (int i=0; i < NO_ROUTER_PORT; i++)
      m_router_microscheduler->notify(*p_flit_payload, t, m_routing[i]);
  }

  class
  buffering
  : public 
    vppnoc_process<nocapp_router, vppnoc_transaction> 
  {
	public:
    NoCPosition desPos;
     unsigned int socket_id;
     int j;
	public:
		cr_context;
		buffering( 
      VPPNOCMicroScheduler<nocapp_router>**	ma
    , nocapp_router* ownr, unsigned int id) 
    : vppnoc_process<nocapp_router, vppnoc_transaction>(ma, ownr, true) 
		{
			cr_init();
      socket_id = id;
      j = 0;
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& flit_trans
    ) 
    {
//      cout << owner->name() << " \t" << j++ << endl; 
      sc_time delay = sc_time(1, sc_core::SC_NS);
      vppnoc_phase  phase;
      flit_trans.set_status(PAYLOAD_OK);
      owner->m_router_microscheduler->notify(owner->m_input_buffer[socket_id]->empty, delay);
      if (*(owner->m_input_buffer[socket_id]->head_update))
        owner->m_router_microscheduler->notify(owner->m_input_buffer[socket_id]->head_update, delay);
      owner->m_inPort[socket_id]->nb_transport_bw(flit_trans, phase, delay);  
      return 1;
		}
	};


  class
  routing
  : public 
    vppnoc_process<nocapp_router, vppnoc_transaction> 
  {
	public:
    NoCPosition desPos;
     unsigned int socket_id;
	public:
		cr_context;
		routing( 
      VPPNOCMicroScheduler<nocapp_router>**	ma
    , nocapp_router* ownr, unsigned int id) 
    : vppnoc_process<nocapp_router, vppnoc_transaction>(ma, ownr, false) 
		{
			cr_init();
      socket_id = id;
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& flit_trans
    ) 
    {
      sc_time delay = sc_time(1, sc_core::SC_NS);
      sc_time now;
      
      vppnoc_phase  phase;
      unsigned int dest_inx;
      flit_type    flit_type;
      vppnoc_transaction* p_flit_trans;
      vppnoc_sync_enum_type rtn;
			cr_start();
      while(1)
      {
        if (*(owner->m_input_buffer[socket_id]->empty)) {
          wait(flit_trans, delay, owner->m_input_buffer[socket_id]->empty, false, this);
          cr_return(1);
        }
        if (!*(owner->m_input_buffer[socket_id]->head_update)) {
          wait(flit_trans, delay, owner->m_input_buffer[socket_id]->head_update, true, this);
          cr_return(1);
        }
        if (!*(owner->m_input_buffer[socket_id]->empty))
        {
          now = sc_time_stamp();
          p_flit_trans = owner->m_input_buffer[socket_id]->read();
          p_flit_trans->set_time_stamp(sc_time_stamp());

          flit_type = hdr(nocapp_flit_payload, p_flit_trans, m_flit).get_field();
          if (flit_type == FIRST_FLIT)
          {
            dest_inx = hdr(nocapp_flit_payload, p_flit_trans, m_dst_tile).get_field();
              
            int i;
            for (i = 0; i < NO_ROUTER_PORT; i++) 
              if (owner->m_switch[i][dest_inx] == true)
                break;
            if ( i == NO_ROUTER_PORT) { // out port is free
              rtn = owner->m_outPort[dest_inx]->nb_transport_fw(*p_flit_trans, phase, delay);                
              if (rtn == VPPNOC_ACCEPTED) {
                *(owner->m_output_port_free[dest_inx]) = false;
                *(owner->m_output_port_ready[dest_inx]) = false;
                owner->m_switch[socket_id][dest_inx] = true;
                *(owner->m_input_buffer[socket_id]->head_update) = false;
              } else {
                *(owner->m_output_port_ready[dest_inx]) = false;
                wait(flit_trans, delay, owner->m_output_port_ready[dest_inx], true, this);
                cr_return(1); 
              }
            } else {
              wait(flit_trans, delay, owner->m_output_port_free[0], true, this);
              cr_return(1);
            }
          } else if (flit_type == MIDDLE_FLIT || flit_type == LAST_FLIT)
          {
            int i;
            for(i = 0; i < NO_ROUTER_PORT; i++) 
              if (owner->m_switch[socket_id][i] == true)
                break;
            if (i != NO_ROUTER_PORT) 
            {
              rtn = owner->m_outPort[i]->nb_transport_fw(*p_flit_trans, phase, delay);
              if (rtn == VPPNOC_ACCEPTED) {
                *(owner->m_output_port_ready[socket_id]) = true;
                *(owner->m_input_buffer[socket_id]->head_update) = false;
              } else {
                *(owner->m_output_port_ready[i]) = false;
                wait(flit_trans, delay, owner->m_output_port_ready[i], true, this);
                cr_return(1); 
              } 
            } else { 
              cout << owner->name() << " Routing Error at function " << __FUNCTION__ << " at line " << __LINE__ << endl; 
            }
          } else {
            cout << owner->name() << " Routing Error at function " << __FUNCTION__ << " at line " << __LINE__ << endl;
          }
        }
			}
      return 1;
			cr_end();
		}
	};



class
  acknowledging
  : public 
    vppnoc_process<nocapp_router, vppnoc_transaction> 
  {
	public:
    NoCPosition desPos;
     unsigned int socket_id;
     int j;
	public:
		cr_context;
		acknowledging( 
      VPPNOCMicroScheduler<nocapp_router>**	ma
    , nocapp_router* ownr, unsigned int id) 
    : vppnoc_process<nocapp_router, vppnoc_transaction>(ma, ownr, false) 
		{
			cr_init();
      socket_id = id;
      j = 0;
		}
		
    virtual 
    int 
    operator()
    ( vppnoc_transaction& flit_trans
    ) 
    {
      int i;
      if (flit_trans.get_status() == PAYLOAD_RESOURCE_FREE) {
        *(owner->m_output_port_ready[socket_id]) = true;
        owner->m_router_microscheduler->notify(owner->m_output_port_ready[socket_id]);
  //    m_inPort[i]->nb_transport_bw(flit_trans, phase, delay);  
      } else {
        for (i = 0; i < NO_ROUTER_PORT; i++)
          if (owner->m_switch[i][socket_id] == true)
            break;                                                         
        if (i != NO_ROUTER_PORT) {
          if (socket_id == 0) {
            *(owner->m_output_port_ready[socket_id]) = true;
            owner->m_router_microscheduler->notify(owner->m_output_port_ready[socket_id]);
          }
          if (flit_trans.get_status() == PAYLOAD_OK) {        
            owner->m_input_buffer[i]->pop();
            if (*(owner->m_input_buffer[i]->head_update))
              owner->m_router_microscheduler->notify(owner->m_input_buffer[i]->head_update);
            flit_type flit_t = hdr(nocapp_flit_payload, &flit_trans, m_flit).get_field();
            if (flit_t == LAST_FLIT)  {
                owner->m_switch[i][socket_id] = false;
                *(owner->m_output_port_free)[socket_id] = true;
                owner->m_router_microscheduler->notify(owner->m_output_port_free[socket_id]);
            }
            } else {
              cout << owner->name() << " NOK received" << endl;
              owner->m_switch[i][socket_id] = false;
            }
            vppnoc_phase phase;
            sc_time delay = sc_time(1, SC_NS);
            flit_trans.set_status(PAYLOAD_RESOURCE_FREE);
            owner->m_inPort[i]->nb_transport_bw(flit_trans, phase, delay);  
        } else { cout << "Error Router" << endl;}
      }

      return 1;
		}
	};


public:
  typedef VPPNOCMicroScheduler<nocapp_router>                   router_microscheduler;
  
  router_microscheduler*                                        m_router_microscheduler;
  vppnoc_slave_socket<nocapp_router, NO_OF_CHANNELS>            m_inPort[NO_ROUTER_PORT];
	vppnoc_master_socket<nocapp_router, NO_OF_CHANNELS>           m_outPort[NO_ROUTER_PORT];
  vppnoc_process<nocapp_router, vppnoc_transaction>*            m_routing[NO_ROUTER_PORT];
  vppnoc_process<nocapp_router, vppnoc_transaction>*            m_buffering[NO_ROUTER_PORT];
  vppnoc_process<nocapp_router, vppnoc_transaction>*            m_acknowledging[NO_ROUTER_PORT];
  vppnoc_signal*                                                m_output_port_ready[NO_ROUTER_PORT]; // ready for next flit
  vppnoc_signal*                                                m_output_port_free[NO_ROUTER_PORT];  // free for next packet
  bool                                                          m_switch[NO_ROUTER_PORT][NO_ROUTER_PORT];
  vppnoc_mm<vppnoc_payload_base, nocapp_flit_payload>           m_flit_mm;  
  struct input_buffer {
    input_buffer():head(0), tail(0)
    { 
      full = new vppnoc_signal;
      *full = false;
      empty = new vppnoc_signal;
      *empty = true;
      head_update = new  vppnoc_signal;
      *head_update = false;
    }
    int push(vppnoc_transaction& flit_trans) {
      if (head == tail)
        *head_update = true;
      *((nocapp_flit_payload*)(buffer[tail]->m_pdu)) = *((nocapp_flit_payload*)flit_trans.m_pdu);
      buffer[tail]->set_payload_kind(flit_trans.get_payload_kind());
      buffer[tail]->set_transaction_command(flit_trans.get_transaction_command());
      tail = (tail+1)%ROUTER_BUFFER_LENGTH;
      *empty = false;
      if (tail == head)
        *full = true;
      return 1;
    }
    vppnoc_transaction* read() {
      return buffer[head];
    }

    vppnoc_transaction* pop() {
      *full = false;
      
      int head_read = head;
      head = (head+1)%ROUTER_BUFFER_LENGTH;
      if (head == tail) {
        *empty = true;
      } else {
        *head_update = true;
      }
      return buffer[head_read];
    }
    
    vppnoc_transaction* buffer[ROUTER_BUFFER_LENGTH];
    int                     head;
    int                     tail;
    vppnoc_signal*          full;
    vppnoc_signal*          empty;
    vppnoc_signal*          head_update;
  };
  input_buffer*                                                 m_input_buffer[NO_ROUTER_PORT];
  
};

}
#endif //__NOCAPP_ROUTER_H__