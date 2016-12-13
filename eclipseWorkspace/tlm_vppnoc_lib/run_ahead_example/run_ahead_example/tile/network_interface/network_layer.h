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
#include "tasks/include/taskmanager.h"
#include "protocol_data_unit/nocapp_message_payload.h"

namespace nocapp {

#define INF_COST 10000
#include <map>
using namespace std;

extern std::ofstream  trafficLogFile;
extern std::ofstream  resultOutputFile;



class NetworkLayer : public vppnoc::NoCProtocolLayer {
public:

	NetworkLayer(sc_core::sc_module_name name,
		int id,
		vppnoc::TileType ty) :
		vppnoc::NoCProtocolLayer(name, id),
	    mNetworkLayerDelay(1, sc_core::SC_NS),
		m_tile_id(id),
		m_tile_type(ty)
	{
		
		m_inPort(*this);
		m_ni_manager = new (vppnoc::VPPNOCMicroScheduler<NetworkLayer>)("NetworkLayerNoCNIManager", id);

		m_input_buffer  = new std::list<vppnoc::vppnoc_transaction*>[NR_OF_NETWORK_LAYER_CHANNEL];
//		m_channel_state = new ChannelStateType[NR_OF_NETWORK_LAYER_CHANNEL];
//		m_sent_flit_output_state = new SendFlitOutStateType[NR_OF_NETWORK_LAYER_CHANNEL];
//		m_p_packet_out_trans = new transaction_type*[NR_OF_NETWORK_LAYER_CHANNEL];
//		m_input_buffer_length = new int[NR_OF_NETWORK_LAYER_CHANNEL];

		for (int i = 0; i < NR_OF_NETWORK_LAYER_CHANNEL; i++) {
//			m_channel_state[i] = CHANNEL_IDLE;
//			m_sent_flit_output_state[i] = SEND_OUT_IDLE;
//			m_input_buffer_length[i] = NETWORK_INPUT_BUFFER_LENGTH;
		}
	}

		~NetworkLayer() {
			delete[] m_ni_manager;
//			delete[] m_input_buffer;
//			delete[] m_channel_state;
//			delete[] m_sent_flit_output_state;
//			delete[] m_p_packet_out_trans;
//			delete[] m_input_buffer_length;

		}
	//*********************************************************************************
	// 					Interfaces for receiving messages or packets
	//*********************************************************************************

	vppnoc::vppnoc_sync_enum_type  nb_transport_fw(int socket_id, vppnoc::vppnoc_transaction* p_trans, sc_core::sc_time & t) {
#ifdef DEBUG
		trafficLogFile << name() << " input_nb_transport_fw PORT = " << initiator_id << std::endl;
		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_packet_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_packet_trans_ext, m_packet_id)
					   << std::endl;
#endif

		//r_packet_trans.acquire();

/*		if (phase == vppnoc::BEGIN_REQ) {
			m_ni_manager->notify((NetworkLayer*)this, initiator_id, r_packet_trans, t, (int*)NULL, initiator_id, m_action_label[NETWORK_DOWN_STREAM_FW]);
			r_packet_trans.set_response_status(vppnoc::VPPNOC_OK_RESPONSE);
			phase = vppnoc::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_packet_trans.release();
			return vppnoc::VPPNOC_UPDATED;
		} else if (phase == vppnoc::END_RESP) {

		} else {
			assert(0); exit(1);
		}*/
		std::cout << " I am nb transport fw of network layer" << std::endl;
		return vppnoc::VPPNOC_ACCEPTED;
	}

/*	sync_enum_type downstream_nb_transport_bw(int initiator_id, transaction_type& r_packet_trans, phase_type& phase, sc_core::sc_time& t) {

		return tlm::TLM_ACCEPTED;
	}


	sync_enum_type upstream_nb_transport_fw(int initiator_id, transaction_type& r_flit_trans, phase_type& phase, sc_core::sc_time& t) {

		//r_flit_trans.acquire();

#ifdef DEBUG
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		
		trafficLogFile << name() << " upstream_nb_transport_fw = " << " time = " << sc_core::sc_time_stamp() << " noc_gp_hdr(p_flit_trans_ext, m_flit_id) = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id) <<
			" noc_gp_hdr(p_flit_trans_ext, m_flit) = " << noc_gp_hdr(p_flit_trans_ext, m_flit) << 
			" noc_gp_hdr(p_flit_trans_ext, m_message_id)=" << noc_gp_hdr(p_flit_trans_ext, m_message_id) << " p_flit_trans_ext = " << p_flit_trans_ext << std::endl;

#endif
		if (phase == tlm::BEGIN_REQ) {
			m_ni_manager->notify((NetworkLayer*)this, initiator_id, r_flit_trans, t, (int*)NULL, initiator_id, m_action_label[NETWORK_UP_STREAM_FW]);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_flit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			//r_flit_trans.release();
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

		return tlm::TLM_ACCEPTED;  //unreachable code
	}

	sync_enum_type upstream_nb_transport_bw(int initiator_id, transaction_type& r_flit_trans, phase_type& phase, sc_core::sc_time& t) {

		return tlm::TLM_COMPLETED;
	}


private:
	///downstreamfw_cb
	void function_21(transaction_type* p_trans, int* signal, int value, int channel_no){
#ifdef DEBUG
		trafficLogFile << name() << " function_21 PORT = " << channel_no << std::endl;
#endif
		//p_trans->acquire();
		int input_ch_no = channel_no;
		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		

		if (noc_gp_hdr(p_packet_trans_ext,  m_dst_tile) == m_tile_id) {
			input_ch_no = noc_gp_hdr(p_packet_trans_ext,  m_session);
			sc_core::sc_time t = sc_core::sc_time(NETWORK_LAYER_UPSTREAM_INPUT_BUFFER_DELAY, sc_core::SC_NS);
			phase_type phase ; ///initialization needed
			m_input_buffer[input_ch_no].push_back(p_trans);
			if (m_input_buffer[input_ch_no].size() > 0) {
				p_trans = m_input_buffer[input_ch_no].front();
				m_ni_manager->notify((NetworkLayer*)this, input_ch_no, *p_trans, t, (int*)NULL, input_ch_no, m_action_label[NETWORK_INPUT_BUFF]);
		//		p_trans->release();
				m_input_buffer[input_ch_no].pop_front();
			} else {
				UNDER_CONSTRUCTION();
			}
		} else {
			if (m_channel_state[input_ch_no] == CHANNEL_IDLE) {
				m_channel_state[input_ch_no] = CHANNEL_WAIT;
				sc_core::sc_time t = sc_core::sc_time(sc_core::SC_ZERO_TIME);
				m_ni_manager->notify((NetworkLayer*)this, input_ch_no, *p_trans, t, (int*)NULL, input_ch_no, m_action_label[NETWORK_SEND_FLIT_OUT]);
		//		p_trans->release();
			} else {
				m_ni_manager->notify((NetworkLayer*)this, input_ch_no, *p_trans, (int*)&m_channel_state[input_ch_no], CHANNEL_IDLE, m_action_label[NETWORK_DOWN_STREAM_FW]);
			//	p_trans->release();
			}
		}
	}

	///downstreambw_cb
	void function_22(transaction_type* p_trans, int* signal, int value, int channel_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_22 PORT = " << channel_no << std::endl;
#endif
		//p_trans->acquire();

//		p_trans->release();

	}

	///upstreamfw_cb
	void function_23(transaction_type* p_flit_trans, int* signal, int value, int channel_no) {
		//p_flit_trans->acquire();
		int input_ch_no = channel_no;
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		
#ifdef DEBUG
		trafficLogFile << name() << " function_23 = " << " time = " << sc_core::sc_time_stamp() << " noc_gp_hdr(p_flit_trans_ext, m_flit_id) = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id) <<
			" noc_gp_hdr(p_flit_trans_ext, m_flit) = " << noc_gp_hdr(p_flit_trans_ext, m_flit) << 
			" noc_gp_hdr(p_flit_trans_ext, m_message_id)=" << noc_gp_hdr(p_flit_trans_ext, m_message_id) << " p_flit_trans_ext = " << p_flit_trans_ext << std::endl;
#endif

		save_flit(p_flit_trans, input_ch_no);
	//	p_flit_trans->release();
	}

	///upstreambw_cb

	
	void function_25(transaction_type* p_packet_trans, int* signal, int value, int channel_no) {
#ifdef DEBUG
		{
			fatal_error_log << name() << " function_25 = " << channel_no;// << std::endl;
			tlm_noc::noc_packet_payload* p_packet_trans_ext;
			
			fatal_error_log << " m_message_id = " << noc_gp_hdr(p_packet_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_packet_trans_ext, m_packet_id)
					   << " " << p_packet_trans_ext << " "  << p_packet_trans
					   << std::endl;
		}
#endif
		//p_packet_trans->acquire();
		int input_ch_no = channel_no;
		sc_core::sc_time t = sc_core::sc_time(NETWORK_LAYER_UPSTREAM_INPUT_BUFFER_DELAY, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_REQ ; ///initialization needed
		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		

		//p_packet_trans = m_input_buffer[input_ch_no].front();
		switch (up_layer->upstream_nb_transport_fw(input_ch_no, *p_packet_trans, phase, t)) {
		case tlm::TLM_COMPLETED:
		case tlm::TLM_UPDATED:
			if (phase == tlm::END_REQ) {
				if (p_packet_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {
					UNDER_CONSTRUCTION();
				} else if (p_packet_trans->get_response_status() == tlm::TLM_OK_RESPONSE) {
		//			p_packet_trans->release();
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
		t = sc_core::sc_time(NETWORK_LAYER_UPSTREAM_INPUT_BUFFER_DELAY, sc_core::SC_NS);
		//m_input_buffer[input_ch_no].pop_front();

	}
//SendPacket
	void function_26(transaction_type* p_trans, int* signal, int value, int channel_no) {
		sc_core::sc_time	delay;
		phase_type 			phase;
		int flit_id;
		//p_trans->acquire();
		m_channel_state[channel_no] = CHANNEL_WAIT;
		
		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		
		if (m_sent_flit_output_state[channel_no] == SEND_OUT_IDLE) {
			flit_id = 0;
			m_p_packet_out_trans[channel_no] = p_trans;
//			m_p_packet_out_trans[channel_no]->get_extension(p_packet_trans_ext);
		} else {
			tlm_noc::noc_flit_payload* p_flit_trans_ext;
			p_trans->get_extension(p_flit_trans_ext);
			flit_id = noc_gp_hdr(p_flit_trans_ext,  m_flit_id);
		}

		do {
			transaction_type* 	p_flit_trans;

			if (m_sent_flit_output_state[channel_no] == SEND_OUT_IDLE) {
				p_flit_trans= m_mm.allocate();
				//p_flit_trans->acquire();

				tlm_noc::noc_flit_payload* p_flit_trans_ext = new tlm_noc::noc_flit_payload;
				p_flit_trans->set_extension(p_flit_trans_ext);
				*p_packet_trans_ext >> *p_flit_trans_ext;

				noc_gp_hdr(p_flit_trans_ext,  m_cmd) = tlm_noc::NORMAL_FLIT;
				noc_gp_hdr(p_flit_trans_ext,  m_flit_id) = flit_id;
				noc_gp_hdr(p_flit_trans_ext,  m_packet_id) = noc_gp_hdr(p_packet_trans_ext,  m_packet_id);
				noc_gp_hdr(p_flit_trans_ext,  m_message_id) = noc_gp_hdr(p_packet_trans_ext,  m_message_id);
				noc_gp_hdr(p_flit_trans_ext,  m_cast) = noc_gp_hdr(p_packet_trans_ext, m_cast);
				noc_gp_hdr(p_flit_trans_ext,  m_routing) = noc_gp_hdr(p_packet_trans_ext,  m_routing);
				noc_gp_hdr(p_flit_trans_ext,  m_dst_tile) = noc_gp_hdr(p_packet_trans_ext,  m_dst_tile);
				noc_gp_hdr(p_flit_trans_ext,  m_session) = noc_gp_hdr(p_packet_trans_ext,  m_session);

				noc_gp_hdr(p_flit_trans_ext,m_rls_flit)    = false;
				if (flit_id == 0) {
					noc_gp_hdr(p_flit_trans_ext,m_flit)    = tlm_noc::FIRST_FLIT;
					noc_gp_hdr(p_flit_trans_ext,  m_head_flit) = true;
				} else if (p_packet_trans_ext->stream_tail == 0) {
					noc_gp_hdr(p_flit_trans_ext,m_flit)    = tlm_noc::LAST_FLIT;
					noc_gp_hdr(p_flit_trans_ext,m_rls_flit)    = true;                   //LAST FLIT in a PACKET to release the wormhole switching 
					noc_gp_hdr(p_flit_trans_ext,  m_head_flit) = false;
				} else {
					noc_gp_hdr(p_flit_trans_ext,m_flit)    = tlm_noc::MIDDLE_FLIT;
					noc_gp_hdr(p_flit_trans_ext,  m_head_flit) = false;
				}
			} else {
				p_flit_trans = p_trans;
				m_p_packet_out_trans[channel_no]->get_extension(p_packet_trans_ext);
			}
			phase = tlm::BEGIN_REQ;
			delay = sc_core::SC_ZERO_TIME;

			phase = tlm::BEGIN_REQ;
			delay = sc_core::sc_time(NETWORK_TO_PHYSICAL_DELAY, sc_core::SC_NS);
			switch (down_layer->downstream_nb_transport_fw(channel_no, *p_flit_trans, phase, delay)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
				if (phase == tlm::END_REQ) {
					if (p_flit_trans->get_response_status() == tlm::TLM_OK_RESPONSE) {
						m_sent_flit_output_state[channel_no] = SEND_OUT_IDLE;
					//	p_flit_trans->release();
					} else if (p_flit_trans->get_response_status() == tlm::TLM_GENERIC_ERROR_RESPONSE) {
						m_sent_flit_output_state[channel_no] = SEND_OUT_WAIT;
						sc_core::sc_time t = sc_core::sc_time(sc_core::sc_time(10, sc_core::SC_NS));
						m_ni_manager->notify((NetworkLayer*)this, channel_no, *p_flit_trans, t, (int*)NULL, channel_no, m_action_label[NETWORK_SEND_FLIT_OUT]);
					//	p_flit_trans->release();
						//std::cout << "sdf" << std::endl;
						return;
					} 
				}
				break;
			case tlm::TLM_ACCEPTED:
				UNDER_CONSTRUCTION();
			default:
				std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
				assert(false); exit(1);
			}
			flit_id++;
		}while (p_packet_trans_ext->stream_tail != 0);
		m_channel_state[channel_no] = CHANNEL_IDLE;
		m_ni_manager->notify((NetworkLayer*)this, channel_no, (int*)&m_channel_state[channel_no]);
		if (p_packet_trans_ext->stream_tail == 0){
		//	p_trans->release();
		}
	}


	
	int cost (int i_task_cost) {
		return 0;
	}


	void save_flit(transaction_type* p_trans, unsigned int input_ch_no) {
		//input_ch_no = 0;
		//p_trans->acquire();
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_trans->get_extension(p_flit_trans_ext);
//#ifdef DEBUG
		fatal_error_log << name() << " Channel = " << input_ch_no << " save_flit = " << " time = " << sc_core::sc_time_stamp() << 
			" noc_gp_hdr(p_flit_trans_ext, m_flit) = " << noc_gp_hdr(p_flit_trans_ext, m_flit) << 
			" noc_gp_hdr(p_flit_trans_ext, m_message_id)=" << noc_gp_hdr(p_flit_trans_ext, m_message_id) << " p_flit_trans_ext = " << p_flit_trans_ext << std::endl;
//#endif



		input_ch_no = noc_gp_hdr(p_flit_trans_ext, m_session);
		if (noc_gp_hdr(p_flit_trans_ext, m_flit_id) == 0) {
			transaction_type* p_packet_trans;
			p_packet_trans = m_mm.allocate();
			p_packet_trans->acquire();
	//		tlm_noc::noc_packet_payload* p_packet_trans_ext;

			p_packet_trans_ext = new tlm_noc::noc_packet_payload;
			p_packet_trans->set_extension(p_packet_trans_ext);

			*p_packet_trans_ext << *p_flit_trans_ext;
			m_input_buffer[input_ch_no].push_back(p_packet_trans);

		} else if (noc_gp_hdr(p_flit_trans_ext, m_flit) == tlm_noc::LAST_FLIT) {
			sc_core::sc_time t = sc_core::sc_time(NETWORK_LAYER_UPSTREAM_INPUT_BUFFER_DELAY, sc_core::SC_NS);
			phase_type phase ; ///initialization needed
			//trafficLogFile << " m_input_buffer[input_ch_no].size() = " << m_input_buffer[input_ch_no].size() << std::endl;
			transaction_type* p_packet_trans;
			p_packet_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_packet_payload* p_packet_trans_ext;
			p_packet_trans->get_extension(p_packet_trans_ext);
			*p_packet_trans_ext << *p_flit_trans_ext;
			
			if (m_input_buffer[input_ch_no].size() > 0) {
				p_packet_trans = m_input_buffer[input_ch_no].front();
				tlm_noc::noc_packet_payload* p_packet_trans_ext;
				p_packet_trans->get_extension(p_packet_trans_ext);
				if (noc_gp_hdr(p_packet_trans_ext, m_cmd) == -842150451)
					resultOutputFile << " ****************noc_gp_hdr(p_packet_trans_ext, m_cmd) = " << noc_gp_hdr(p_packet_trans_ext, m_cmd);
				m_ni_manager->notify((NetworkLayer*)this, input_ch_no, *p_packet_trans, t, (int*)NULL, input_ch_no, m_action_label[NETWORK_INPUT_BUFF]);
				m_input_buffer[input_ch_no].pop_front();
			//	p_packet_trans->release();
			}

		} else {
			transaction_type* p_packet_trans;
//			trafficLogFile << " m_input_buffer[input_ch_no].size() = " << m_input_buffer[input_ch_no].size() << std::endl;
			p_packet_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_packet_payload* p_packet_trans_ext;
			p_packet_trans->get_extension(p_packet_trans_ext);
			*p_packet_trans_ext << *p_flit_trans_ext;
		}
	//	p_trans->release();
	}
	*/
private:
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
	

	sc_core::sc_time           mNetworkLayerDelay;
	vppnoc::VPPNOCMicroScheduler<NetworkLayer>*	m_ni_manager;
	std::list<vppnoc::vppnoc_transaction*>*                       m_input_buffer;
	std::map<network_ni_part_type, unsigned int>        m_action_label;
	int				m_tile_id;
	vppnoc::TileType		m_tile_type;
	SendFlitOutStateType*                               m_sent_flit_output_state;
	//ChannelStateType*                                   m_channel_state;
	vppnoc::vppnoc_transaction**                                  m_p_packet_out_trans;
	std::map<int, int>				                    m_channel_to_session;//m_response_channel_table;
//	tlm_noc::NoRCMM                                     m_mm;
	int*                                                m_input_buffer_length;
public:
	vppnoc::vppnoc_slave_socket<32>  m_inPort;
};

}
#endif /*NETWORK_LAYER_H_*/

//**************************************************************************************************
//*                                       BUGS                                                     *
//**************************************************************************************************
// 1- if all channels are full then  some bad pointers exist in m_peq_request_packet_trans
//    which should be handled in future version.
// 2- received inorder flits should be handled in future
// 3-
//
//**************************************************************************************************

