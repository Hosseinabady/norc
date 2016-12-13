
#ifndef ROUTER_H_
#define ROUTER_H_


#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include <math.h>
#include <cstdlib>
#include <cassert>
#include <queue>
#include <time.h>
#include "model/model.h"

#include "model/modules/include/noc_connector.h"


extern std::ofstream trafficLogFile;
extern std::ofstream trafficInRouterLogFile;
extern std::ofstream fatal_error_log;
#define ROUTER_TEMPLATE_PARAMETER_IN NR_OF_INPUT_CHANNEL,\
        NR_OF_OUTPUT_CHANNEL,\
        INPUT_CHANNEL_CAPACITY,\
        OUTPUT_CHANNEL_CAPACITY,\
        TLM_NOC_INPUT_PAYLOAD,\
        TLM_NOC_OUTPUT_PAYLOAD,\
        TLM_GENERIC_PAYLOAD,\
        TLM_PHASE,\
        TLM_SYNC_ENUM

template <int NR_OF_INPUT_CHANNEL      = 1,
		  int NR_OF_OUTPUT_CHANNEL     = 1,
		  int INPUT_CHANNEL_CAPACITY   = 1,
		  int OUTPUT_CHANNEL_CAPACITY  = 1,
		  class TLM_NOC_INPUT_PAYLOAD  = tlm_noc::noc_phit_payload,
		  class TLM_NOC_OUTPUT_PAYLOAD = tlm_noc::noc_phit_payload,
		  class TLM_GENERIC_PAYLOAD    = tlm::tlm_generic_payload,
		  class TLM_PHASE              = tlm::tlm_phase,
		  class TLM_SYNC_ENUM          = tlm::tlm_sync_enum>
class Router : public NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
											  NR_OF_OUTPUT_CHANNEL,
								              INPUT_CHANNEL_CAPACITY,
								              OUTPUT_CHANNEL_CAPACITY,
								              TLM_NOC_INPUT_PAYLOAD,
								              TLM_NOC_OUTPUT_PAYLOAD,
								              TLM_GENERIC_PAYLOAD,
								              TLM_PHASE,
								              TLM_SYNC_ENUM> {

public:

	Router(sc_core::sc_module_name name, int id) :
		NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
					     NR_OF_OUTPUT_CHANNEL,
					     INPUT_CHANNEL_CAPACITY,
						 OUTPUT_CHANNEL_CAPACITY,
					     TLM_NOC_INPUT_PAYLOAD,
					     TLM_NOC_OUTPUT_PAYLOAD,
					     TLM_GENERIC_PAYLOAD,
					     TLM_PHASE,
					     TLM_SYNC_ENUM>(name, id)
	{
		char txt[20];
		sprintf(txt, (const char*)name);

		m_router_core = new tlm_noc::NoCRouterManager<Router<ROUTER_TEMPLATE_PARAMETER> >(txt, this, id);

		m_input_buffer  = new std::list<transaction_type*>[NR_OF_INPUT_CHANNEL*NR_OF_ROUTER_PORT_CONST];
		m_output_buffer = new std::list<transaction_type*>[NR_OF_INPUT_CHANNEL*NR_OF_ROUTER_PORT_CONST];
		m_directions_list = new std::list<int>[NR_OF_INPUT_CHANNEL*NR_OF_ROUTER_PORT_CONST];
		m_used_channel = new bool[NR_OF_ROUTER_PORT_CONST*NR_OF_INPUT_CHANNEL];
		m_output_channel_state = new ChannelStateType[NR_OF_ROUTER_PORT_CONST*NR_OF_INPUT_CHANNEL];
		m_input_channel_state = new ChannelStateType[NR_OF_ROUTER_PORT_CONST*NR_OF_INPUT_CHANNEL];

		for(int i = 0; i < NR_OF_ROUTER_PORT_CONST*NR_OF_INPUT_CHANNEL; i++) {
			m_used_channel[i] = false;
			m_switch[i] = NULL;
			m_output_channel_state[i] = IDLE;
			m_input_channel_state[i] = IDLE;
		}

		//---- Two special label 0 = OUTPORT_WAIT & 1 = OUTPORT_WAIT_LIST_ACTIVE
		m_action_label[OUTPORT_WAIT]             = m_router_core->register_activity(&Router<ROUTER_TEMPLATE_PARAMETER_IN>::outputlcnt_cb);
		m_action_label[OUTPORT_WAIT_LIST_ACTIVE] = m_router_core->register_activity(NULL);
		m_action_label[INPUT_CNT]                = m_router_core->register_activity(&Router<ROUTER_TEMPLATE_PARAMETER_IN>::inputlcnt_cb);
		m_action_label[INPUT_BUFF]               = m_router_core->register_activity(&Router<ROUTER_TEMPLATE_PARAMETER_IN>::inputbuff_cb);
		m_action_label[ROUTING_ALGORITHM]        = m_router_core->register_activity(&Router<ROUTER_TEMPLATE_PARAMETER_IN>::routalg_cb);
		m_action_label[ARBITER]                  = m_router_core->register_activity(&Router<ROUTER_TEMPLATE_PARAMETER_IN>::arbiter_cb);
		m_action_label[SWITCHING]                = m_router_core->register_activity(&Router<ROUTER_TEMPLATE_PARAMETER_IN>::swt_cb);
		m_action_label[OUTPUT_BUFF]              = m_router_core->register_activity(&Router<ROUTER_TEMPLATE_PARAMETER_IN>::outputbuff_cb);
		m_action_label[OUTPUT_CNT]               = m_router_core->register_activity(&Router<ROUTER_TEMPLATE_PARAMETER_IN>::outputlcnt_cb);
	}
//============================================================================================
//===                 INPUT SOCKETS                                                        ===
//============================================================================================

	sync_enum_type nb_transport_fw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " router_nb_transport_fw PORT = " << initiator_id <<" time = "<< sc_core::sc_time_stamp();
		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		r_phit_trans.get_extension(p_phit_trans_ext);
	
		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_phit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_phit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_flit_id)
					   << " m_phit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_phit_id) << std::endl;
#endif

		r_phit_trans.acquire();
		if (phase == tlm::BEGIN_REQ) {
			m_router_core->notify(r_phit_trans, phase, t, initiator_id, m_action_label[INPUT_CNT]);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_phit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			r_phit_trans.acquire();
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {
			UNDER_CONSTRUCTION();
		} else {
			//std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
			assert(false); exit(1);
		}
		return tlm::TLM_ACCEPTED;
	}

	sync_enum_type nb_transport_bw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t)
	{
#ifdef DEBUG
		trafficLogFile << name() << " router_nb_transport_bw PORT = " << initiator_id << " time = "<< sc_core::sc_time_stamp()<< std::endl;
		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		r_phit_trans.get_extension(p_phit_trans_ext);

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_phit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_phit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_flit_id)
					   << " m_phit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_phit_id) << std::endl;

#endif
		r_phit_trans.acquire();
		if (phase == tlm::BEGIN_RESP) {
			m_output_channel_state[initiator_id] = FREE;
			m_router_core->notify(r_phit_trans, initiator_id, m_action_label[OUTPORT_WAIT_LIST_ACTIVE]);
			phase = tlm::END_RESP;
			t = sc_core::SC_ZERO_TIME;
			r_phit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {
			UNDER_CONSTRUCTION();
		} else {
			//std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
			assert(false); exit(1);
		}

		return tlm::TLM_ACCEPTED;

	}



//============================================================================================
//===                 CALL BACK FUNCTIONS                                                  ===
//============================================================================================
	void inputlcnt_cb(transaction_type* p_phit_trans, unsigned int input_ch_no) {
		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		p_phit_trans->get_extension(p_phit_trans_ext);

#ifdef DEBUG
		trafficLogFile << name() << " inputlcnt_cb PORT = " << input_ch_no << std::endl;
	//		for (int i = 0; i < NR_OF_INPUT_CHANNEL*NR_OF_ROUTER_PORT_CONST; i++) {
	//			trafficLogFile << name() << " m_output_buffer[" << i << "].size() = " << m_output_buffer[i].size() << std::endl;  
	//			trafficLogFile << name() << "  m_input_buffer[" << i << "].size() = " << m_input_buffer[i].size() << std::endl;
	//		}

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_phit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_phit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_flit_id)
					   << " m_phit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_phit_id) << std::endl;
#endif
		if (noc_gp_hdr(p_phit_trans_ext, m_phit_id) == 0  && noc_gp_hdr(p_phit_trans_ext, m_data_phit) == false) { //First phit which is control
#ifdef DEBUG
			trafficLogFile << name() << " inputlcnt_cb PORT = " << input_ch_no << " CONTROL FLIT " << std::endl;
#endif
			if (m_input_buffer[input_ch_no].size() < INPUT_CHANNEL_CAPACITY) {
				m_input_channel_state[input_ch_no] = IDLE;
				send_ack_phit(p_phit_trans, input_ch_no);
			} else {
#ifdef DEBUG
				trafficLogFile << name() << " m_input_channel_state[" << input_ch_no << "] = WAIT " << input_ch_no << std::endl;
#endif
				m_input_channel_state[input_ch_no] = WAIT;
			}
		}

		if (noc_gp_hdr(p_phit_trans_ext, m_data_phit) == true) { // First phit which is data
			trafficLogFile << name() << " inputlcnt_cb PORT = " << input_ch_no << " DATA FLIT " << std::endl;
			if (m_input_buffer[input_ch_no].size() > INPUT_CHANNEL_CAPACITY) {
				fatal_error_log << name() << " inputlcnt_cb there is no space in input buffer " << input_ch_no << std::endl;
				assert(false); exit(1);
			}
			save_flit(p_phit_trans_ext, input_ch_no);
			p_phit_trans->release();
		}
	}
	void inputbuff_cb(transaction_type* p_flit_trans, unsigned int input_ch_no) {

		p_flit_trans = m_input_buffer[input_ch_no].front();
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
#ifdef DEBUG
			trafficLogFile << name() << " inputbuff_cb PORT = " << input_ch_no ;//<< std::endl;
			trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
				           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
						   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id) << std::endl;
#endif
		sc_core::sc_time t = sc_core::sc_time(INPUT_BUFFER, sc_core::SC_NS);
		phase_type phase ; ///initialization needed

		if (noc_gp_hdr(p_flit_trans_ext,  m_head_flit) == true) {
			m_router_core->notify(*p_flit_trans, phase, t, input_ch_no, m_action_label[ROUTING_ALGORITHM]);
		} else {
			m_router_core->notify(*p_flit_trans, phase, t, input_ch_no, m_action_label[SWITCHING]);
		}
	}
	void routalg_cb(transaction_type* p_flit_trans, unsigned int input_ch_no) {
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
#ifdef DEBUG
		trafficLogFile << name() << " routalg_cb PORT = " << input_ch_no << std::endl;

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << std::endl;
#endif

		if (noc_gp_hdr(p_flit_trans_ext, m_cast) == tlm_noc::BRD_CAST || noc_gp_hdr(p_flit_trans_ext, m_cast) == tlm_noc::UNI_CAST) {
			if (noc_gp_hdr(p_flit_trans_ext,  m_routing) == tlm_noc::XY) {
				int dst_tile = noc_gp_hdr(p_flit_trans_ext,  m_dst_tile);
				int y_dst = ((dst_tile-1)/NO_OF_COL_CONST);
				int x_dst = ((dst_tile-1)%NO_OF_COL_CONST);
				int y_loc = ((getId()-1)/NO_OF_COL_CONST);
				int x_loc = ((getId()-1)%NO_OF_COL_CONST);
				if (x_dst < x_loc)
					m_directions_list[input_ch_no].push_back(LEFT_PORT);
				else if (x_dst > x_loc)
					m_directions_list[input_ch_no].push_back(RIGHT_PORT);
				else if (y_loc < y_dst)
					m_directions_list[input_ch_no].push_back(DOWN_PORT);
				else if (y_loc > y_dst)
					m_directions_list[input_ch_no].push_back(UP_PORT);
				else
					m_directions_list[input_ch_no].push_back(LOCAL_PORT);
			} else if (noc_gp_hdr(p_flit_trans_ext,  m_routing) == tlm_noc::PUR_FLOODING) {
				if (input_ch_no/NR_OF_INPUT_CHANNEL == LOCAL_PORT) { //flooding start point so send the flit to all other port
					m_directions_list[input_ch_no].push_back(LEFT_PORT);
					m_directions_list[input_ch_no].push_back(RIGHT_PORT);
					m_directions_list[input_ch_no].push_back(UP_PORT);
					m_directions_list[input_ch_no].push_back(DOWN_PORT);
				} else if (input_ch_no/NR_OF_INPUT_CHANNEL == LEFT_PORT) {
					m_directions_list[input_ch_no].push_back(DOWN_PORT);
					m_directions_list[input_ch_no].push_back(RIGHT_PORT);
					m_directions_list[input_ch_no].push_back(UP_PORT);
					m_directions_list[input_ch_no].push_back(LOCAL_PORT);
				} else if (input_ch_no/NR_OF_INPUT_CHANNEL == RIGHT_PORT) {
					m_directions_list[input_ch_no].push_back(DOWN_PORT);
					m_directions_list[input_ch_no].push_back(LEFT_PORT);
					m_directions_list[input_ch_no].push_back(UP_PORT);
					m_directions_list[input_ch_no].push_back(LOCAL_PORT);
				} else if (input_ch_no/NR_OF_INPUT_CHANNEL == UP_PORT) {
					m_directions_list[input_ch_no].push_back(DOWN_PORT);
					m_directions_list[input_ch_no].push_back(LOCAL_PORT);
					m_directions_list[input_ch_no].push_back(LEFT_PORT);
					m_directions_list[input_ch_no].push_back(RIGHT_PORT);
				} else if (input_ch_no/NR_OF_INPUT_CHANNEL == DOWN_PORT) {
					m_directions_list[input_ch_no].push_back(UP_PORT);
					m_directions_list[input_ch_no].push_back(LOCAL_PORT);
					m_directions_list[input_ch_no].push_back(LEFT_PORT);
					m_directions_list[input_ch_no].push_back(RIGHT_PORT);
				}
			} else if (noc_gp_hdr(p_flit_trans_ext,  m_routing) == tlm_noc::DIR_FLOODING) {
				if (input_ch_no/NR_OF_INPUT_CHANNEL == LOCAL_PORT) { //flooding start point so send the flit to all other port
					m_directions_list[input_ch_no].push_back(LEFT_PORT);
					m_directions_list[input_ch_no].push_back(RIGHT_PORT);
					m_directions_list[input_ch_no].push_back(UP_PORT);
					m_directions_list[input_ch_no].push_back(DOWN_PORT);
				} else if (input_ch_no/NR_OF_INPUT_CHANNEL == LEFT_PORT) {
					m_directions_list[input_ch_no].push_back(DOWN_PORT);
					m_directions_list[input_ch_no].push_back(RIGHT_PORT);
					m_directions_list[input_ch_no].push_back(UP_PORT);
					m_directions_list[input_ch_no].push_back(LOCAL_PORT);
				} else if (input_ch_no/NR_OF_INPUT_CHANNEL == RIGHT_PORT) {
					m_directions_list[input_ch_no].push_back(DOWN_PORT);
					m_directions_list[input_ch_no].push_back(LEFT_PORT);
					m_directions_list[input_ch_no].push_back(UP_PORT);
					m_directions_list[input_ch_no].push_back(LOCAL_PORT);
				} else if (input_ch_no/NR_OF_INPUT_CHANNEL == UP_PORT) {
					m_directions_list[input_ch_no].push_back(DOWN_PORT);
					m_directions_list[input_ch_no].push_back(LOCAL_PORT);
				} else if (input_ch_no/NR_OF_INPUT_CHANNEL == DOWN_PORT) {
					m_directions_list[input_ch_no].push_back(UP_PORT);
					m_directions_list[input_ch_no].push_back(LOCAL_PORT);
				}
			}
			sc_core::sc_time t = sc_core::sc_time(ARBITER_DELAY, sc_core::SC_NS);
			phase_type phase;
			m_router_core->notify(*p_flit_trans, phase, t, input_ch_no, m_action_label[ARBITER]);
		}
	}
	void arbiter_cb(transaction_type* p_flit_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " arbiter_cb  PORT = " << input_ch_no << std::endl;
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << std::endl;
#endif
		std::list<int>::iterator it;
		bool channel_free_flag = true;
		int freeChannel;

		for ( it = m_directions_list[input_ch_no].begin(); it != m_directions_list[input_ch_no].end(); ) { //broadcast the flit or flooding
			if (FindFreeChannelInPort(*it, freeChannel)) { // there is a free channel in port number #i
				m_used_channel[freeChannel] = true;
				if (m_switch[input_ch_no] == NULL) {
					std::vector<int>* free_port = new std::vector<int>;
					m_switch[input_ch_no] = free_port;
				}
				m_switch[input_ch_no]->push_back(freeChannel);
				it = m_directions_list[input_ch_no].erase(it);
			} else {
				channel_free_flag = false;
				it++;
			}
		}
		if (channel_free_flag) {
			sc_core::sc_time t = sc_core::sc_time(SWITCHING_DELAY, sc_core::SC_NS);
			phase_type phase;
			m_router_core->notify(*p_flit_trans, phase, t, input_ch_no, m_action_label[SWITCHING]);
		} else {
			sc_core::sc_time t = sc_core::SC_ZERO_TIME;
			phase_type phase;
			m_router_core->notify(*p_flit_trans, phase, t, input_ch_no, m_action_label[ROUTING_ALGORITHM]);
		}
	}
	

	void swt_cb(transaction_type* p_flit_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		//if (strcmp(name(), "Router29") == 0)
		trafficLogFile << name() << " SWITCHING " << input_ch_no << " "; // << std::endl;
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
	//	if (strcmp(name(), "Router29") == 0)
		trafficLogFile << " Routing algorithm = " << noc_gp_hdr(p_flit_trans_ext,  m_routing);
		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << " time = " << sc_core::sc_time_stamp() << " p_flit_trans = " << p_flit_trans << std::endl;
#endif
		sc_core::sc_time t = sc_core::sc_time(OUTPUT_BUFF_DELAY, sc_core::SC_NS);
		phase_type phase;
		bool path_blocked = false;
		for (unsigned int i = 0; i < m_switch[input_ch_no]->size(); i++) {
			if (m_output_buffer[m_switch[input_ch_no]->at(i)].size() >= OUTPUT_CHANNEL_CAPACITY) {
				trafficLogFile << name() << " output buffer full " << m_switch[input_ch_no]->at(i) << std::endl;
				path_blocked = true;
			}
		}
		if (path_blocked == false) {
			for (unsigned int i = 0; i < m_switch[input_ch_no]->size(); i++)
				m_router_core->notify(*p_flit_trans, phase, t, m_switch[input_ch_no]->at(i), m_action_label[OUTPUT_BUFF]);
			
			m_input_buffer[input_ch_no].pop_front();
			trafficLogFile << name() << " m_input_buffer[" << input_ch_no << "].erase " <<  " m_input_buffer.size =" << m_input_buffer[input_ch_no].size() << std::endl;

			if (m_input_channel_state[input_ch_no] == WAIT) {
				m_input_channel_state[input_ch_no] = IDLE;

				transaction_type* p_phit_trans = m_mm.allocate();
				p_phit_trans->acquire();

				tlm_noc::noc_phit_payload* p_phit_trans_ext;
				p_phit_trans_ext = new tlm_noc::noc_phit_payload;

				p_phit_trans->set_extension(p_phit_trans_ext);

				send_ack_phit(p_phit_trans, input_ch_no);
				p_phit_trans->release();
			}

			if (m_input_buffer[input_ch_no].size() > 0) {
				sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
				m_router_core->notify(*p_flit_trans, phase, t, input_ch_no, m_action_label[INPUT_BUFF]);
			}
			tlm_noc::noc_flit_payload* p_flit_trans_ext;
			p_flit_trans->get_extension(p_flit_trans_ext);
			if (noc_gp_hdr(p_flit_trans_ext, m_rls_flit) == true) {
				trafficLogFile << name() << "RELAEASE" << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << " time = " << sc_core::sc_time_stamp() << std::endl;
				for (unsigned int i = 0; i < m_switch[input_ch_no]->size(); i++)
					m_used_channel[m_switch[input_ch_no]->at(i)] = false;
				m_switch[input_ch_no]->clear();
				m_switch.erase(input_ch_no);		
			}
		} else {
			m_router_core->notify(*p_flit_trans, phase, t, input_ch_no, m_action_label[SWITCHING]);
		}
	}
	void outputbuff_cb(transaction_type* p_flit_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " output buffer " << input_ch_no ;//<< std::endl;
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << std::endl;
		

#endif
		sc_core::sc_time t = sc_core::sc_time(OUTPUT_BUFF_CNT_DELAY, sc_core::SC_NS);
		phase_type phase;
		m_output_buffer[input_ch_no].push_back(p_flit_trans);

		if (m_output_buffer[input_ch_no].size() == 1) {
			p_flit_trans = m_output_buffer[input_ch_no].front();
			m_router_core->notify(*(m_output_buffer[input_ch_no].front()), phase, t, input_ch_no, m_action_label[OUTPUT_CNT]);
		}



	}
	void outputlcnt_cb(transaction_type* p_flit_trans, unsigned int input_ch_no) {
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
#ifdef DEBUG
		trafficLogFile << name() << " output buffer controller " << input_ch_no << std::endl;

		trafficLogFile  << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << std::endl;
#endif

		sc_core::sc_time t = sc_core::sc_time(SWITCHING_DELAY, sc_core::SC_NS);
		phase_type phase;
		if (m_output_channel_state[input_ch_no] == IDLE) {
			send_req_phit(p_flit_trans_ext, input_ch_no);
			m_output_channel_state[input_ch_no] = WAIT;

			m_router_core->notify(*p_flit_trans, input_ch_no, m_action_label[OUTPORT_WAIT]);
#ifdef DEBUG
			trafficLogFile << name() << "11111111111111111111111 " << std::endl;
#endif
		} else if (m_output_channel_state[input_ch_no] == WAIT) {
			m_router_core->notify(*p_flit_trans, input_ch_no, m_action_label[OUTPORT_WAIT]);
#ifdef DEBUG
			trafficLogFile << name() << "222222222222222222222 " << std::endl;
#endif
		} else if (m_output_channel_state[input_ch_no] == FREE) {
			send_data_flit(p_flit_trans_ext, input_ch_no);
			m_output_channel_state[input_ch_no] = IDLE;
			m_output_buffer[input_ch_no].erase(m_output_buffer[input_ch_no].begin());
			if (m_output_buffer[input_ch_no].size() > 0) {
#ifdef DEBUG
				trafficLogFile << name() << " output buffer not empty" << std::endl;
#endif
				m_router_core->notify(*(m_output_buffer[input_ch_no].front()), phase, t, input_ch_no, m_action_label[OUTPUT_CNT]);
			}
#ifdef DEBUG
			trafficLogFile << name() << " 3333333333333333333 " << std::endl;
//			for (int i = 0; i < NR_OF_INPUT_CHANNEL*NR_OF_ROUTER_PORT_CONST; i++) {
//				trafficLogFile << name() << " m_output_buffer[" << i << "].size() = " << m_output_buffer[i].size() << std::endl;  
//				trafficLogFile << name() << "  m_input_buffer[" << i << "].size() = " << m_input_buffer[i].size() << std::endl;
//			}

#endif

		}
	}

//============================================================================================
//===                    INTERNAL PRIVATE FUNCTION                                         ===
//============================================================================================
private:
	void send_ack_phit(transaction_type* p_phit_trans, int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " send_ack_phit " << input_ch_no << std::endl;

#endif
		sc_core::sc_time delay = sc_core::sc_time(PHYSICAL_TO_PHYSICAL_DELAY, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_RESP;

		switch (m_port[input_ch_no/NR_OF_INPUT_CHANNEL]->nb_transport_bw(input_ch_no%NR_OF_INPUT_CHANNEL, *p_phit_trans, phase, delay)) {
		case tlm::TLM_COMPLETED:
		case tlm::TLM_UPDATED:
			if (phase == tlm::END_RESP) {
				if (p_phit_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {
					//trafficLogFile << name() << "  " << sc_core::sc_time_stamp() << " next input port does not have anough space - 3" << std::endl;
				} else if (p_phit_trans->get_response_status() == tlm::TLM_OK_RESPONSE) {
					p_phit_trans->release();
				}
				break;
			} else {
				std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
				assert(false); exit(1);
			}
		case tlm::TLM_ACCEPTED:
			break;
		default:
			std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
			assert(false); exit(1);
		}
	}


	void save_flit(tlm_noc::noc_phit_payload* p_phit_trans_ext, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " save_flit " << input_ch_no << std::endl;

#endif
		if (noc_gp_hdr(p_phit_trans_ext, m_phit_id) == 0) {
			transaction_type* p_flit_trans;
			p_flit_trans = m_mm.allocate();
			p_flit_trans->acquire();
			tlm_noc::noc_flit_payload* p_flit_trans_ext;

			p_flit_trans_ext = new tlm_noc::noc_flit_payload;
			p_flit_trans->set_extension(p_flit_trans_ext);

			p_flit_trans_ext->noc_trans << p_phit_trans_ext->noc_trans;

			m_input_buffer[input_ch_no].push_back(p_flit_trans);

		} else if ( noc_gp_hdr(p_phit_trans_ext, m_b_last_phit) == true) {
			sc_core::sc_time t = sc_core::sc_time(INPUT_BUFFER, sc_core::SC_NS);
			phase_type phase ; ///initialization needed
			transaction_type* p_flit_trans;
			p_flit_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_flit_payload* p_flit_trans_ext;
			p_flit_trans->get_extension(p_flit_trans_ext);
			p_flit_trans_ext->noc_trans << p_phit_trans_ext->noc_trans;
			if (m_input_buffer[input_ch_no].size() == 1) {
				//p_flit_trans = m_input_buffer[input_ch_no].front();
				m_router_core->notify(*p_flit_trans, phase, t, input_ch_no, m_action_label[INPUT_BUFF]);
				//m_input_buffer[input_ch_no].pop_front();
			}

		} else {
			transaction_type* p_flit_trans;
			p_flit_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_flit_payload* p_flit_trans_ext;
			p_flit_trans->get_extension(p_flit_trans_ext);
			p_flit_trans_ext->noc_trans << p_phit_trans_ext->noc_trans;
		}
	}





	bool FindFreeChannelInPort(int prt, int& chnl) {
		for (int i = prt*NR_OF_INPUT_CHANNEL; i < (prt+1)*NR_OF_INPUT_CHANNEL; i++)
			if (m_used_channel[i] == false) {
				chnl = i;
				return true;
			}
		return false;
	}
	void send_req_phit(tlm_noc::noc_flit_payload* p_flit_trans_ext, int input_ch_no) {
		transaction_type* p_phit_trans;
		p_phit_trans = m_mm.allocate();
		p_phit_trans->acquire();

		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		p_phit_trans_ext = new tlm_noc::noc_phit_payload;

		p_phit_trans->set_extension(p_phit_trans_ext);


		noc_gp_hdr(p_phit_trans_ext, m_b_last_phit) = false;
		noc_gp_hdr(p_phit_trans_ext, m_phit_id) = 0;
		noc_gp_hdr(p_phit_trans_ext, m_data_phit) = false;
		noc_gp_hdr(p_phit_trans_ext, m_rls_phit) = false;

		noc_gp_hdr(p_phit_trans_ext, m_flit_id) = 0;
		noc_gp_hdr(p_phit_trans_ext, m_packet_id) = 0;
		noc_gp_hdr(p_phit_trans_ext, m_message_id) = 0;

		sc_core::sc_time t = sc_core::sc_time(PHYSICAL_TO_PHYSICAL_DELAY, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_REQ;
		switch (m_port[input_ch_no/NR_OF_INPUT_CHANNEL]->nb_transport_fw(input_ch_no%NR_OF_INPUT_CHANNEL, *p_phit_trans, phase, t)) {
		case tlm::TLM_COMPLETED:
		case tlm::TLM_UPDATED:
			if (phase == tlm::END_REQ) {
				if (p_phit_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {
					//trafficLogFile << name() << "  " << sc_core::sc_time_stamp() << " next input port does not have anough space - 3" << std::endl;
				} else if (p_phit_trans->get_response_status() == tlm::TLM_OK_RESPONSE) {
					//			trafficLogFile << name() << "  " << sc_core::sc_time_stamp() << " next input port OK" << std::endl;
					//p_phit_trans->free_all_extensions();
					p_phit_trans->release();
				}
				break;
			} else {
				std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
				assert(false); exit(1);
			}
		case tlm::TLM_ACCEPTED:
			break;
		default:
			std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
			assert(false); exit(1);
		}
	}


	void send_data_flit(tlm_noc::noc_flit_payload* p_flit_trans_ext, int input_ch_no) {
#ifdef DEBUG
			trafficLogFile << name() << " send_data_flit " << std::endl;
#endif

		transaction_type* p_phit_trans;
		phase_type phase;
		sc_core::sc_time t;

		int phit_no = 0;
		do {
			p_phit_trans = m_mm.allocate();
			p_phit_trans->acquire();

			tlm_noc::noc_phit_payload* p_phit_trans_ext;
			p_phit_trans_ext = new tlm_noc::noc_phit_payload;

			p_phit_trans->set_extension(p_phit_trans_ext);


			p_flit_trans_ext->noc_trans >> p_phit_trans_ext->noc_trans;

			noc_gp_hdr(p_phit_trans_ext, m_phit_id) = phit_no;
			noc_gp_hdr(p_phit_trans_ext, m_flit_id) = noc_gp_hdr(p_flit_trans_ext,  m_flit_id);
			noc_gp_hdr(p_phit_trans_ext, m_packet_id) = noc_gp_hdr(p_flit_trans_ext,  m_packet_id);
			noc_gp_hdr(p_phit_trans_ext, m_message_id) = noc_gp_hdr(p_flit_trans_ext,  m_message_id);
			noc_gp_hdr(p_phit_trans_ext, m_rls_phit) = false;

			noc_gp_hdr(p_phit_trans_ext, m_data_phit) = true;

			if (p_flit_trans_ext->noc_trans.stream_tail == 0) {
				noc_gp_hdr(p_phit_trans_ext, m_b_last_phit) = true;
			if (noc_gp_hdr(p_flit_trans_ext,  m_rls_flit) == true)
				noc_gp_hdr(p_phit_trans_ext, m_rls_phit) = true;
			} else {
				noc_gp_hdr(p_phit_trans_ext, m_b_last_phit) = false;
			}
			p_phit_trans->set_address(phit_no);


			t = sc_core::sc_time(PHYSICAL_TO_PHYSICAL_DELAY, sc_core::SC_NS);
			phase = tlm::BEGIN_REQ;
			switch (m_port[input_ch_no/NR_OF_INPUT_CHANNEL]->nb_transport_fw(input_ch_no%NR_OF_INPUT_CHANNEL, *p_phit_trans, phase, t)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
				if (phase == tlm::END_REQ) {
					if (p_phit_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {
						//trafficLogFile << name() << "  " << sc_core::sc_time_stamp() << " next input port does not have anough space - 3" << std::endl;

					} else if (p_phit_trans->get_response_status() == tlm::TLM_OK_RESPONSE) {
						p_phit_trans->release();
						phit_no++;
					}
					break;
				} else {
					std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
					assert(false); exit(1);
				}
			case tlm::TLM_ACCEPTED:
				break;
			default:
				std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
				assert(false); exit(1);
			}

		} while (p_flit_trans_ext->noc_trans.stream_tail != 0);
	}

	void send_data_phit(transaction_type* p_phit_trans, int input_ch_no) {
		sc_core::sc_time t = sc_core::sc_time(PHYSICAL_TO_PHYSICAL_DELAY, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_REQ;
		switch (m_port[input_ch_no/NR_OF_INPUT_CHANNEL]->nb_transport_fw(input_ch_no%NR_OF_INPUT_CHANNEL, *p_phit_trans, phase, t)) {
		case tlm::TLM_COMPLETED:
		case tlm::TLM_UPDATED:
			if (phase == tlm::END_REQ) {
				if (p_phit_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {
						//trafficLogFile << name() << "  " << sc_core::sc_time_stamp() << " next input port does not have anough space - 3" << std::endl;

				} else if (p_phit_trans->get_response_status() == tlm::TLM_OK_RESPONSE) {
					p_phit_trans->release();

				}
				break;
			} else {
				std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
				assert(false); exit(1);
			}
		case tlm::TLM_ACCEPTED:
			break;
		default:
			std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
			assert(false); exit(1);
		}
	}



public:

	void Report() {
		trafficLogFile << name() << " Report " << std::endl;
/*		for (int i = 0; i < NR_OF_INPUT_CHANNEL*NR_OF_ROUTER_PORT_CONST; i++) {
			trafficInRouterLogFile << name() << " m_output_buffer[" << i << "].size() = " << m_output_buffer[i].size() << std::endl;  
			trafficInRouterLogFile << name() << " m_output_channel_state[" << i << "] = " << m_output_channel_state[i] << std::endl;  
			
			trafficInRouterLogFile << name() << "  m_input_buffer[" << i << "].size() = " << m_input_buffer[i].size() << std::endl;
			trafficInRouterLogFile << name() << "  m_input_channel_state[" << i << "] = " << m_input_channel_state[i] << std::endl;  
		}

		for(int i = 0; i < NR_OF_ROUTER_PORT_CONST*NR_OF_INPUT_CHANNEL; i++) {
			if (m_switch[i] != NULL) {
				for (int j = 0; j < m_switch[i]->size(); i++)
					trafficInRouterLogFile << " m_switch[" << i <<  "] paths = " << m_switch[i]->at(j);
				trafficInRouterLogFile << std::endl;
			}
			trafficInRouterLogFile << " m_switch[" << i <<  "] paths = EMPTY " << std::endl;
		}*/
	}

public:
	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>,  NR_OF_CHANNEL_CONST >*  m_port[NR_OF_ROUTER_PORT_CONST];



	enum RouterPartEnum {	INPUT_CNT,
							INPUT_BUFF,
							ROUTING_ALGORITHM,
							ARBITER,
							FLIT2PHIT,
							SWITCHING,
							OUTPUT_BUFF,
							OUTPUT_CNT,
							OUTPORT_WAIT,
							OUTPORT_WAIT_LIST_ACTIVE
				 };
	typedef RouterPartEnum router_part_type;

	enum ChannelStateEnum  { IDLE,
							 WAIT,
							 FREE,
							 BLKED
						   };
	typedef ChannelStateEnum  ChannelStateType;

	tlm_noc::NoCRouterManager<Router<ROUTER_TEMPLATE_PARAMETER> >*  m_router_core;
	std::list<transaction_type*>*								  m_input_buffer;
	std::list<transaction_type*>*								  m_output_buffer;
	tlm_noc::NoRCMM                                               m_mm;
	std::map<int, std::vector<int>* >                             m_switch;
	bool*											              m_used_channel; // a flag which shows the free/used channel
	ChannelStateType*                                             m_output_channel_state;
	ChannelStateType*                                             m_input_channel_state;
	std::list<int>*												  m_directions_list;
	int                                                           m_flit_signature;
	std::map<router_part_type, unsigned int>                      m_action_label;
};




#endif /*ROUTER_H_*/
