#pragma once
#ifndef PHYSICAL_LAYER_H_
#define PHYSICAL_LAYER_H_


#include "interface.h"
#include "NoRC/protocol_data_units/include/noc_payload.h"

#include "string"
#include "model/model.h"

using namespace std;
extern std::ofstream  trafficLogFile;
extern std::ofstream  fatal_error_log;

class PhysicalLayer : public NoCProtocolLayer {
public:


	SC_HAS_PROCESS(PhysicalLayer);

	PhysicalLayer(sc_core::sc_module_name name, int id, TileType ty) :
		NoCProtocolLayer(name, id), mPhysicalLayerDelay(1, sc_core::SC_NS), m_type(ty) {

		m_ni_manager = new tlm_noc::NoCNIManager<NoCProtocolLayer>("PhysicalLayerNoCNIManager", id);
		m_channel_state = new ChannelStateType[NR_OF_TRANSPORT_LAYER_CHANNEL];
		m_input_buffer  = new std::list<transaction_type*>[NR_OF_TRANSPORT_LAYER_CHANNEL];
		m_input_buffer_length = new int[NR_OF_TRANSPORT_LAYER_CHANNEL];
		for (int i = 0; i < NR_OF_TRANSPORT_LAYER_CHANNEL; i++) {
			m_input_buffer_length[i] = PHY_INPUT_BUFFER_LENGTH;
			m_channel_state[i] = CHANNEL_IDLE;
		}
		
		

		m_action_label[PHYSICAL_MAIN_ACTION_WAIT] = m_ni_manager->register_activity(&NoCProtocolLayer::function_36); //phydownfw_cb
		m_action_label[PHYSICAL_WAIT_LIST_ACTIVE] = m_ni_manager->register_activity(NULL);
		m_action_label[PHYSICAL_DOWN_STREAM_FW]   = m_ni_manager->register_activity(&NoCProtocolLayer::function_31); //phydownfw_cb
		m_action_label[PHYSICAL_DOWN_STREAM_BW]   = m_ni_manager->register_activity(&NoCProtocolLayer::function_32); //phydownbw_cb
		m_action_label[PHYSICAL_UP_STREAM_FW]     = m_ni_manager->register_activity(&NoCProtocolLayer::function_33); //phyupfw_cb
		m_action_label[PHYSICAL_UP_STREAM_BW]     = m_ni_manager->register_activity(&NoCProtocolLayer::function_34); //phyupbw_cb
		m_action_label[PHYSICAL_INPUT_BUFF]       = m_ni_manager->register_activity(&NoCProtocolLayer::function_35); //inputbuff_upstream_cb
		m_action_label[PHYSICAL_DUMMY]            = m_ni_manager->register_activity(NULL); //

	}

		~PhysicalLayer() {
			delete[] m_ni_manager;
			delete[] m_channel_state;
			delete[] m_input_buffer;
			delete[] m_input_buffer_length;
		}
	//*********************************************************************************
	// 					Interfaces for receiving messages or packets
	//*********************************************************************************


	sync_enum_type downstream_nb_transport_fw(int initiator_id, transaction_type& r_flit_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " downstream_nb_transport_fw PORT = " << initiator_id ;
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		r_flit_trans.get_extension(p_flit_trans_ext);

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << " " << p_flit_trans_ext << " "  << &r_flit_trans
					   << std::endl;
#endif
//		r_flit_trans.acquire();

		if (phase == tlm::BEGIN_REQ) {
			if (m_input_buffer_length[initiator_id] > 0) {
				m_input_buffer_length[initiator_id]--;
				m_ni_manager->notify((PhysicalLayer*)this, initiator_id, r_flit_trans, t, (int*)NULL, initiator_id, m_action_label[PHYSICAL_DOWN_STREAM_FW]);
				r_flit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			} else {
				r_flit_trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
				std::cout << "Physical Channel Buffer FULL" << std::endl;
			}

			t = sc_core::SC_ZERO_TIME;
			phase = tlm::END_REQ;
			//r_flit_trans.release();
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

		return tlm::TLM_ACCEPTED;
	}

	sync_enum_type downstream_nb_transport_bw(int initiator_id, transaction_type& r_flit_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		fatal_error_log << name() << " downstream_nb_transport_bw PORT = " << initiator_id << std::endl;
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		r_flit_trans.get_extension(p_flit_trans_ext);

		fatal_error_log << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << std::endl;
#endif
	//	r_flit_trans.acquire();

		if (phase == tlm::BEGIN_RESP) {
			m_ni_manager->notify((PhysicalLayer*)this, initiator_id, r_flit_trans, mPhysicalLayerDelay, NULL, initiator_id, m_action_label[PHYSICAL_DOWN_STREAM_BW]);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_flit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			//r_flit_trans.release();
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}
		return tlm::TLM_ACCEPTED;
	}

	sync_enum_type upstream_nb_transport_fw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t)
	{
#ifdef DEBUG
		trafficLogFile << name() << " upstream_nb_transport_fw PORT = " << initiator_id ;
		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		r_phit_trans.get_extension(p_phit_trans_ext);

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_phit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_phit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_flit_id)
					   << " m_phit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_phit_id) 
					   << " address = " << p_phit_trans_ext << std::endl;
#endif
	//	r_phit_trans.acquire();
		if (phase == tlm::BEGIN_REQ) {
			m_ni_manager->notify((PhysicalLayer*)this, initiator_id, r_phit_trans, mPhysicalLayerDelay, NULL, initiator_id, m_action_label[PHYSICAL_UP_STREAM_FW]);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_phit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			//r_phit_trans.release();
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {
		} else {
			assert(0); exit(1);
		}

		return tlm::TLM_UPDATED;;
	}

	sync_enum_type upstream_nb_transport_bw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t)
	{
#ifdef DEBUG
		trafficLogFile << name() << " upstream_nb_transport_bw PORT = " << initiator_id << std::endl;
		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		r_phit_trans.get_extension(p_phit_trans_ext);

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_phit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_phit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_flit_id)
					   << " m_phit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_phit_id) << std::endl;
#endif
//		r_phit_trans.acquire();

		if (phase == tlm::BEGIN_RESP) {
			m_ni_manager->notify((PhysicalLayer*)this, initiator_id, r_phit_trans, mPhysicalLayerDelay, NULL, initiator_id, m_action_label[PHYSICAL_UP_STREAM_BW]);
			phase = tlm::END_RESP;
			t = sc_core::SC_ZERO_TIME;
			r_phit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			//r_phit_trans.release();
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

		return tlm::TLM_ACCEPTED;
	}

	//phydownfw_cb
	virtual void function_31 (transaction_type* p_flit_trans, int* signal, int value, int channel_no) {
	//	p_flit_trans->acquire();
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
#ifdef DEBUG
		trafficLogFile << name() << " function_31 PORT = " << channel_no;

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << " " << p_flit_trans_ext << " "  << p_flit_trans
					   << std::endl;
#endif
		int input_ch_no = channel_no;

		sc_core::sc_time t = sc_core::sc_time(PHY_WAIT_FOR_EMPTY_SPACE_DELAY, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_REQ ; 

		if (m_channel_state[input_ch_no] == CHANNEL_IDLE) {
			send_req_phit(p_flit_trans_ext, input_ch_no);
			m_channel_state[input_ch_no] = CHANNEL_WAIT;
			m_ni_manager->notify((PhysicalLayer*)this, input_ch_no, *p_flit_trans, (int* )&m_channel_state[input_ch_no], CHANNEL_FREE, m_action_label[PHYSICAL_MAIN_ACTION_WAIT]);
			m_input_buffer_length[channel_no]++;
		} else {
			m_ni_manager->notify((PhysicalLayer*)this, input_ch_no, *p_flit_trans, (int* )&m_channel_state[input_ch_no], CHANNEL_IDLE, m_action_label[PHYSICAL_DOWN_STREAM_FW]);
		}
		p_flit_trans->release();

	}

	virtual void function_32 (transaction_type* p_flit_trans, int* signal, int value, int channel_no) {
	//	p_flit_trans->acquire();

		p_flit_trans->release();
	}

	virtual void function_33 (transaction_type* p_phit_trans, int* signal, int value, int channel_no) {
	//	p_phit_trans->acquire();
		int input_ch_no = channel_no;
		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		p_phit_trans->get_extension(p_phit_trans_ext);
#ifdef DEBUG
		trafficLogFile << name() << " phyupfw_cb PORT = " << input_ch_no;


		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_phit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_phit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_flit_id)
					   << " m_phit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_phit_id) 
					   << " address = " << p_phit_trans_ext << std::endl;

#endif
		if (noc_gp_hdr(p_phit_trans_ext, m_phit_id) == 0  && noc_gp_hdr(p_phit_trans_ext, m_data_phit) == false) { //First phit which is control
			noc_gp_hdr(p_phit_trans_ext, m_ack) = true;
			send_ack_phit(p_phit_trans, input_ch_no);
		} else if (noc_gp_hdr(p_phit_trans_ext, m_data_phit) == true) { // First phit which is data
			save_flit(p_phit_trans_ext, input_ch_no);
		}
		p_phit_trans->release();
	}
	//phyupbw_cb
	virtual void function_34(transaction_type* p_phit_trans, int* signal, int value, int channel_no) {
	//	p_phit_trans->acquire();
#ifdef DEBUG
		trafficLogFile << name() << " function_34 PORT = " << channel_no << std::endl;
#endif
		int input_ch_no = channel_no;
		UpStreamBW(p_phit_trans, input_ch_no);
	//	p_phit_trans->release();
	}

	// PHYSICAL_INPUT_BUFF
	// A complete flit has received 
	virtual void function_35(transaction_type* p_flit_trans, int* signal, int value, int channel_no) {
		//p_flit_trans->acquire();
#ifdef DEBUG
		trafficLogFile << name() << " function_35 PORT = " << channel_no << std::endl;
#endif
		int input_ch_no = channel_no;
		sc_core::sc_time t = sc_core::sc_time(NETWORK_LAYER_UPSTREAM_INPUT_BUFFER_DELAY, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_REQ ; ///initialization needed
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);

		p_flit_trans = m_input_buffer[input_ch_no].front();
		switch (up_layer->upstream_nb_transport_fw(input_ch_no, *p_flit_trans, phase, t)) {
		case tlm::TLM_COMPLETED:
		case tlm::TLM_UPDATED:
			if (phase == tlm::END_REQ) {
				if (p_flit_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {
					UNDER_CONSTRUCTION();
				} else if (p_flit_trans->get_response_status() == tlm::TLM_OK_RESPONSE) {
					p_flit_trans->release();
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
		m_input_buffer[input_ch_no].pop_front();
		if (m_input_buffer[input_ch_no].size() > 0)
			m_ni_manager->notify((PhysicalLayer*)this, input_ch_no, *p_flit_trans, t, (int*)NULL, input_ch_no, m_action_label[PHYSICAL_INPUT_BUFF]);
	}

	void send_out(unsigned int input_ch_no);

	//*********************************************************************************
	// 					Threads for processing received Message or Packets
	//*********************************************************************************


	void function_36(transaction_type* p_flit_trans, int* signal, int value, int channel_no){

	//	p_flit_trans->acquire();
	//	p_flit_trans->acquire();
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
#ifdef DEBUG
		trafficLogFile << name() << " function_36 PORT = " << channel_no;

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << " " << p_flit_trans_ext << " "  << p_flit_trans
					   << std::endl;
#endif
		int input_ch_no = channel_no;

		
		if (m_channel_state[input_ch_no] == CHANNEL_FREE) {
			send_data_phit(p_flit_trans_ext, input_ch_no);
			m_input_buffer_length[input_ch_no]++;
			m_channel_state[input_ch_no] = CHANNEL_IDLE;
			m_ni_manager->notify((PhysicalLayer*)this, input_ch_no, (int*)&m_channel_state[input_ch_no]);
			p_flit_trans->release();
		} else {
			std::cout << "Fatal Error Code 1021: " << std::endl;
		}
	}


	void DownStreamBW(transaction_type* p_trans, unsigned int input_ch_no) {

	}

	//inputbuff_upstream_cb

	void UpStreamBW(transaction_type* p_phit_trans, unsigned int input_ch_no) {
		
#ifdef DEBUG
		trafficLogFile << name() << " UpStreamBW " << input_ch_no << std::endl;
#endif
		p_phit_trans->acquire();
		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		p_phit_trans->get_extension(p_phit_trans_ext);


		m_channel_state[input_ch_no] = CHANNEL_FREE;

		//I have to add an if statement for this
		m_ni_manager->notify((PhysicalLayer*)this, input_ch_no, (int*)&m_channel_state[input_ch_no]);
	//	p_phit_trans->acquire();
	}


	void send_req_phit(tlm_noc::noc_flit_payload* p_flit_trans_ext, int input_ch_no) {
		transaction_type* p_phit_trans;
		p_phit_trans = m_mm.allocate();
	//	p_phit_trans->acquire();

		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		p_phit_trans_ext = new tlm_noc::noc_phit_payload;

		p_phit_trans->set_extension(p_phit_trans_ext);


		noc_gp_hdr(p_phit_trans_ext, m_b_last_phit) = false;
		noc_gp_hdr(p_phit_trans_ext, m_phit_id) = 0;
		noc_gp_hdr(p_phit_trans_ext, m_data_phit) = false;
		noc_gp_hdr(p_phit_trans_ext, m_rls_phit) = false;

		sc_core::sc_time delay = sc_core::sc_time(PHYSICAL_TO_PHYSICAL_DELAY, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_REQ;
		switch (down_layer->downstream_nb_transport_fw(input_ch_no, *p_phit_trans, phase, delay)) {
		case tlm::TLM_COMPLETED:
		case tlm::TLM_UPDATED:
			if (phase == tlm::END_REQ) {
				if (p_phit_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {

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


	void send_data_phit(tlm_noc::noc_flit_payload* p_flit_trans_ext, int input_ch_no) {
//#ifdef DEBUG
		trafficLogFile << name() << " send_data_phit PORT = " << input_ch_no;

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << " " << p_flit_trans_ext 
					   << std::endl;
//#endif
		transaction_type* p_phit_trans;
		phase_type phase;
		sc_core::sc_time delay;

		int phit_no = 0;
		do {
			p_phit_trans = m_mm.allocate();
		//	p_phit_trans->acquire();

			tlm_noc::noc_phit_payload* p_phit_trans_ext;
			p_phit_trans_ext = new tlm_noc::noc_phit_payload;

			p_phit_trans->set_extension(p_phit_trans_ext);


			*p_flit_trans_ext >> *p_phit_trans_ext;

			noc_gp_hdr(p_phit_trans_ext, m_phit_id) = phit_no;
			noc_gp_hdr(p_phit_trans_ext, m_flit_id) = noc_gp_hdr(p_flit_trans_ext,  m_flit_id);
			noc_gp_hdr(p_phit_trans_ext, m_packet_id) = noc_gp_hdr(p_flit_trans_ext,  m_packet_id);
			noc_gp_hdr(p_phit_trans_ext, m_message_id) = noc_gp_hdr(p_flit_trans_ext,  m_message_id);
			noc_gp_hdr(p_phit_trans_ext, m_rls_phit) = false;


			noc_gp_hdr(p_phit_trans_ext, m_data_phit) = true;

			if (p_flit_trans_ext->stream_tail == 0) {
				noc_gp_hdr(p_phit_trans_ext, m_b_last_phit) = true;
			if (noc_gp_hdr(p_flit_trans_ext,  m_rls_flit) == true)
				noc_gp_hdr(p_phit_trans_ext, m_rls_phit) = true;
			} else {
				noc_gp_hdr(p_phit_trans_ext, m_b_last_phit) = false;
			}
			p_phit_trans->set_address(phit_no);


			delay = sc_core::sc_time(PHYSICAL_TO_PHYSICAL_DELAY, sc_core::SC_NS);
			phase = tlm::BEGIN_REQ;
			switch (down_layer->downstream_nb_transport_fw(input_ch_no, *p_phit_trans, phase, delay)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
				if (phase == tlm::END_REQ) {
					if (p_phit_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {
						//std::cout << name() << "  " << sc_core::sc_time_stamp() << " next input port does not have anough space - 3" << std::endl;
						assert(false); exit(1);


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


		} while (p_flit_trans_ext->stream_tail != 0);
	}




	void send_ack_phit(transaction_type* p_phit_trans, int input_ch_no) {
	//	p_phit_trans->acquire();
		sc_core::sc_time delay = sc_core::sc_time(PHYSICAL_TO_PHYSICAL_DELAY, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_RESP;
		switch (down_layer->downstream_nb_transport_bw(input_ch_no, *p_phit_trans, phase, delay)) {
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
		
		if (noc_gp_hdr(p_phit_trans_ext, m_phit_id) == 0) {
			transaction_type* p_flit_trans;
			p_flit_trans = m_mm.allocate();
		//	p_flit_trans->acquire();
			tlm_noc::noc_flit_payload* p_flit_trans_ext;

			p_flit_trans_ext = new tlm_noc::noc_flit_payload;
			p_flit_trans->set_extension(p_flit_trans_ext);

			*p_flit_trans_ext << *p_phit_trans_ext;

			m_input_buffer[input_ch_no].push_back(p_flit_trans);

		} else if ( noc_gp_hdr(p_phit_trans_ext, m_b_last_phit) == true) {
			sc_core::sc_time t = sc_core::sc_time(NETWORK_LAYER_UPSTREAM_INPUT_BUFFER_DELAY, sc_core::SC_NS);
			phase_type phase ; ///initialization needed
			transaction_type* p_flit_trans;
			p_flit_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_flit_payload* p_flit_trans_ext;
			p_flit_trans->get_extension(p_flit_trans_ext);
			*p_flit_trans_ext << *p_phit_trans_ext;
					
			

			if (m_input_buffer[input_ch_no].size() == 1) {
				m_ni_manager->notify((PhysicalLayer*)this, input_ch_no, *p_flit_trans, t, (int*)NULL, input_ch_no, m_action_label[PHYSICAL_INPUT_BUFF]);
			}
		} else {
			transaction_type* p_flit_trans;
			p_flit_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_flit_payload* p_flit_trans_ext;
			p_flit_trans->get_extension(p_flit_trans_ext);
			*p_flit_trans_ext << *p_phit_trans_ext;
		}
	}

 
private:

	enum PHYSICAL_NIPartEnum {PHYSICAL_DOWN_STREAM_FW,
				 PHYSICAL_DOWN_STREAM_BW,
	             PHYSICAL_UP_STREAM_BW,
				 PHYSICAL_UP_STREAM_FW,
				 PHYSICAL_MAIN_ACTION_WAIT,
				 PHYSICAL_WAIT_LIST_ACTIVE,
				 PHYSICAL_INPUT_BUFF,
				 PHYSICAL_DUMMY
				};
	typedef PHYSICAL_NIPartEnum physical_ni_part_type;

	tlm_noc::NoCNIManager<NoCProtocolLayer>*	m_ni_manager;

	TileType                                                m_type;
	tlm_noc::NoRCMM                                         m_mm;
	sc_core::sc_time                                        mPhysicalLayerDelay;
	

	
	

	ChannelStateType*                                       m_channel_state;
	std::list<transaction_type*>*                           m_input_buffer;
	std::map<physical_ni_part_type, unsigned int>           m_action_label;
	int*                                                    m_input_buffer_length;
};




#endif /*PHYSICAL_LAYER_H_*/
