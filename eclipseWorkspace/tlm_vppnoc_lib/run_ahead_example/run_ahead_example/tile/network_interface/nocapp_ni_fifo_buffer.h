#ifndef __NOCAPP_NI_FIFO_BUFFER__
#define __NOCAPP_NI_FIFO_BUFFER__

#include <systemc>
#include "vppnoc_core.h"
#include "include/nocapp_utilities.h"
#include "protocol_data_unit/nocapp_message_payload.h"
#include "protocol_data_unit/nocapp_packet_payload.h"
#include "protocol_data_unit/nocapp_flit_payload.h"
#include "include/hr_time.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace vppnoc;

extern std::ofstream dumpreceivedmessagefile ;
extern CStopWatch  simulation_timer;
namespace nocapp {


class 
nocapp_ni_fifo_buffer 
: public vppnoc_module 
{
public:
  nocapp_ni_fifo_buffer
  ( sc_module_name name
  , int id
  , TileType typ)
  : vppnoc_module(name, id, typ)
  {
    m_inPort_from_trans(*this);//, &nocapp_ni_fifo_buffer::nb_transport_fw);
    m_inPort_from_net(*this);
    m_inPort_from_cpu(*this);

    for(int i = 0; i < NO_OF_CHANNELS; i++) 
    {
      head_downwards_buffer_index[i]  = 0;
      tail_downwards_buffer_index[i]  = 0;
      empty_downwards_buffer_index[i] = true;
      full_downwards_buffer_index[i]  = false;

      head_upwards_buffer_index[i]  = 0;
      tail_upwards_buffer_index[i]  = 0;
      empty_upwards_buffer_index[i] = true;
      full_upwards_buffer_index[i]  = false;

      downwards_dactive_packet_id[i] = 0;
      downwards_dactive_flit_id[i] = 0;

      upwards_channel_src_id[i] = -1; //channel free
    }
  }
  
  virtual
  vppnoc_sync_enum_type  
    nb_transport_fw
    (int socket_id                                                     
     , vppnoc_transaction& trans
     , vppnoc_phase & phase
     , sc_time & delay
     ) 
  {
    int dst_tile;
    int src_tile;

    int  channel_no = 0;
    int  head_downwards      = head_downwards_buffer_index [channel_no];
    int  tail_downwards      = tail_downwards_buffer_index [channel_no];
    
    bool full_downwards      = full_downwards_buffer_index [channel_no];

    switch ((trans).get_payload_kind()) 
    {
    case MESSAGE_PAYLOAD:
      if (trans.get_transaction_command() == READ) {
        for (int i = 0; i < NO_OF_CHANNELS; i++) {
          bool empty_downwards     = empty_downwards_buffer_index[channel_no];
          if (!empty_downwards) {
            trans.m_pdu = &downwards_buffer[channel_no][head_downwards];
            return VPPNOC_UPDATED;
          }
        }
          // Channel Buffer is empty
          return VPPNOC_REJECTED;
      } else if (trans.get_transaction_command() == WRITE) {
        if (!full_downwards) {
          downwards_buffer[channel_no][tail_downwards] = *(nocapp_message_payload*)(trans.m_pdu);
          tail_downwards_buffer_index[channel_no] = (tail_downwards_buffer_index[channel_no]+1)%NI_BUFFER_LENGTH;
          empty_downwards_buffer_index[channel_no] = false;
          if (tail_downwards_buffer_index[channel_no] == head_downwards_buffer_index[channel_no])
            full_downwards_buffer_index [channel_no] = true;
          return VPPNOC_UPDATED;
        } else {
          return VPPNOC_REJECTED;
        }
      } else if (trans.get_transaction_command() == POP) {
        for (int i = 0; i < NO_OF_CHANNELS; i++) {
          bool empty_downwards     = empty_downwards_buffer_index[channel_no];
          if (!empty_downwards) {
            full_downwards_buffer_index [channel_no] = false;
            downwards_buffer[channel_no][head_downwards].reset();
            head_downwards_buffer_index[channel_no] = (head_downwards_buffer_index[channel_no]+1)%NI_BUFFER_LENGTH;
            if (head_downwards_buffer_index[channel_no] == tail_downwards_buffer_index [channel_no])
              empty_downwards_buffer_index[channel_no] = true;
            return VPPNOC_UPDATED;
          }
        }
        return VPPNOC_REJECTED;
      } else {
        return VPPNOC_REJECTED;
      }

      break;
    case PACKET_PAYLOAD:
      if (trans.get_transaction_command() == READ) 
      {
        //head = (head)%NI_BUFFER_LENGTH;
        if ((downwards_buffer[channel_no][head_downwards]).stream_tail != -1) 
        {
          active_packet_downwards_buffer[channel_no].reset();
          active_packet_downwards_buffer[channel_no].view_as.pdu.hdr = (*(nocapp_packet_payload*)&trans).view_as.pdu.hdr;
          downwards_buffer[channel_no][head_downwards] >> active_packet_downwards_buffer[channel_no];
          downwards_dactive_packet_id[channel_no]++;
          dst_tile = downwards_buffer[channel_no][head_downwards].view_as.pdu.hdr.m_dst_tile.get_field();
          active_packet_downwards_buffer[channel_no].view_as.pdu.hdr.m_dst_tile.set_field(dst_tile);
            
          active_packet_downwards_buffer[channel_no].view_as.pdu.hdr.m_packet_id.set_field(downwards_dactive_packet_id[channel_no]);

          if (downwards_dactive_packet_id[channel_no] ==  1) active_packet_downwards_buffer[channel_no].view_as.pdu.hdr.m_packet.set_field(FIRST_PACKET);
          else if  ((downwards_buffer[channel_no][head_downwards]).stream_tail == -1) {
            active_packet_downwards_buffer[channel_no].view_as.pdu.hdr.m_packet.set_field(LAST_PACKET);
            downwards_dactive_packet_id[channel_no] = 0;
          }
          else active_packet_downwards_buffer[channel_no].view_as.pdu.hdr.m_packet.set_field(MIDDLE_PACKET);
          
          //delete trans.m_pdu;
          trans.m_pdu = &active_packet_downwards_buffer[channel_no];
          return VPPNOC_UPDATED;
        } else {
          return VPPNOC_REJECTED;
        }
      } else if (trans.get_transaction_command() == WRITE) {

        unsigned int src_core = hdr(nocapp_packet_payload, &trans, m_src_tile).get_field();
        int current_channel = -1;
        for (int i = 0; i < NO_OF_CHANNELS; i++)  {
          if (upwards_channel_src_id[i] == src_core ) {
            current_channel = i;
            break;
          }
        }
        if (current_channel != -1) {
          int  tail_upwards      = tail_upwards_buffer_index [current_channel];
/*           cout << " tile "
                 << get_id()
                 <<" received a message from " 
                 << active_packet_upwards_buffer[current_channel].view_as.pdu.hdr.m_src_tile.get_field()
                 << " packet_id = "
                 << active_packet_upwards_buffer[current_channel].view_as.pdu.hdr.m_packet_id.get_field()
                 << " at "
                 << sc_time_stamp()
                 << endl;
  */
          upwards_buffer[current_channel][tail_upwards] << active_packet_upwards_buffer[current_channel];
          *(nocapp_packet_payload*)(trans.m_pdu) = *(nocapp_packet_payload*)(&active_packet_upwards_buffer[current_channel]);
          if (active_packet_upwards_buffer[current_channel].view_as.pdu.hdr.m_packet.get_field() == LAST_PACKET) {
            upwards_channel_src_id[current_channel] = -1;
#ifdef  DEBUG_DUMP_BUFFER   
            /*dumpreceivedmessagefile*/cout  << " tile "
                 << get_id()
                 <<" received a message from " 
                 << upwards_buffer[current_channel][tail_upwards].view_as.pdu.hdr.m_src_tile.get_field()
                 << " at "
                 << sc_time_stamp()
                 << endl;
#endif
//             upwards_buffer[current_channel][tail_upwards].reset();
          }
          active_packet_upwards_buffer[current_channel].reset();
          return VPPNOC_UPDATED;
        } else {
            cout << "Buffer Error" << endl;
            return VPPNOC_REJECTED;
        }
      }
      break;
    case FLIT_PAYLOAD:
      if (trans.get_transaction_command() == READ) 
      {
        
        if ((active_packet_downwards_buffer[channel_no]).stream_tail != -1) 
        {
          active_flit_downwards_buffer[channel_no].view_as.pdu.hdr = (*(nocapp_flit_payload*)&trans).view_as.pdu.hdr;
          active_packet_downwards_buffer[channel_no] >> active_flit_downwards_buffer[channel_no];
          
          downwards_dactive_flit_id[channel_no]++;
          
          dst_tile = active_packet_downwards_buffer[channel_no].view_as.pdu.hdr.m_dst_tile.get_field();
          active_flit_downwards_buffer[channel_no].view_as.pdu.hdr.m_dst_tile.set_field(dst_tile);

          src_tile = active_packet_downwards_buffer[channel_no].view_as.pdu.hdr.m_src_tile.get_field();
          active_flit_downwards_buffer[channel_no].view_as.pdu.hdr.m_src_tile.set_field(src_tile);
           
          active_flit_downwards_buffer[channel_no].view_as.pdu.hdr.m_flit_id.set_field(downwards_dactive_flit_id[channel_no]);           

          active_flit_downwards_buffer[channel_no].view_as.pdu.hdr.m_packet_id.set_field(downwards_dactive_packet_id[channel_no]);           

          if (downwards_dactive_flit_id[channel_no] ==  1) active_flit_downwards_buffer[channel_no].view_as.pdu.hdr.m_flit.set_field(FIRST_FLIT);
          else if  ((active_packet_downwards_buffer[channel_no]).stream_tail == -1) {
            active_flit_downwards_buffer[channel_no].view_as.pdu.hdr.m_flit.set_field(LAST_FLIT);
            downwards_dactive_flit_id[channel_no] = 0;
          }
          else active_flit_downwards_buffer[channel_no].view_as.pdu.hdr.m_flit.set_field(MIDDLE_FLIT);
          
          //delete trans.m_pdu;
          trans.m_pdu = &active_flit_downwards_buffer[channel_no]; 

          return VPPNOC_UPDATED;
        } else {
          return VPPNOC_REJECTED;
        }
      } else if (trans.get_transaction_command() == WRITE) {
        unsigned int src_core = hdr(nocapp_flit_payload, &trans, m_src_tile).get_field();
        
        int current_channel = -1;
        for (int i = 0; i < NO_OF_CHANNELS; i++)  {
            if (upwards_channel_src_id[i] == src_core ) {
              current_channel = i;
              break;
            } else if (upwards_channel_src_id[i] == -1 ) {
              current_channel = i;
            }
          }
        if (current_channel != -1) {
          active_flit_upwards_buffer[current_channel] = *((nocapp_flit_payload*)(trans.m_pdu)) ;
          upwards_channel_src_id[current_channel] = src_core;
          active_packet_upwards_buffer[current_channel] << active_flit_upwards_buffer[current_channel];
          return VPPNOC_UPDATED;
        } else 
          return VPPNOC_REJECTED;
      } else if (trans.get_transaction_command() == CONTROL_FREE_CHANNEL) {
        unsigned int src_core = hdr(nocapp_flit_payload, &trans, m_src_tile).get_field();
        int current_channel = -1;
        for (int i = 0; i < NO_OF_CHANNELS; i++)  {
            if (upwards_channel_src_id[i] == src_core ) {
              current_channel = i;
              break;
            } else if (upwards_channel_src_id[i] == -1 ) {
              current_channel = i;
            }
          }
        if (current_channel != -1) {
          return VPPNOC_ACCEPTED;
        } else 
          return VPPNOC_REJECTED;
      } else {
        return VPPNOC_REJECTED;
      }
      break;
    default:
      return VPPNOC_REJECTED;
    }
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

public:
  vppnoc_slave_socket<nocapp_ni_fifo_buffer, NO_OF_CHANNELS>                  m_inPort_from_trans;
  vppnoc_slave_socket<nocapp_ni_fifo_buffer, NO_OF_CHANNELS>                  m_inPort_from_net;
  vppnoc_slave_socket<nocapp_ni_fifo_buffer, NO_OF_CHANNELS>                  m_inPort_from_cpu;

private:
  nocapp_message_payload  downwards_buffer              [NO_OF_CHANNELS][NI_BUFFER_LENGTH];
  int                     head_downwards_buffer_index   [NO_OF_CHANNELS];
  int                     tail_downwards_buffer_index   [NO_OF_CHANNELS];

  bool                    empty_downwards_buffer_index  [NO_OF_CHANNELS];
  bool                    full_downwards_buffer_index   [NO_OF_CHANNELS];

  nocapp_message_payload  upwards_buffer                [NO_OF_CHANNELS][NI_BUFFER_LENGTH];
  int                     head_upwards_buffer_index     [NO_OF_CHANNELS];
  int                     tail_upwards_buffer_index     [NO_OF_CHANNELS];

  bool                    empty_upwards_buffer_index    [NO_OF_CHANNELS];
  bool                    full_upwards_buffer_index     [NO_OF_CHANNELS];


  nocapp_packet_payload   active_packet_downwards_buffer[NO_OF_CHANNELS];
  nocapp_packet_payload   active_packet_upwards_buffer  [NO_OF_CHANNELS];

  nocapp_flit_payload     active_flit_downwards_buffer[NO_OF_CHANNELS];
  nocapp_flit_payload     active_flit_upwards_buffer  [NO_OF_CHANNELS];

  int                     downwards_dactive_packet_id[NO_OF_CHANNELS];
  int                     downwards_dactive_flit_id[NO_OF_CHANNELS];
  int                     upwards_channel_src_id[NO_OF_CHANNELS];

};
}

#endif //__NOCAPP_NI_FIFO_BUFFER__