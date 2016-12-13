 #ifndef PHYSICAL_LAYER_H_
#define PHYSICAL_LAYER_H_


#include "interface.h"
#include "NoRC/protocol_data_units/include/noc_payload.h"

#include "string"
#include "model/model.h"

using namespace std;
extern std::ofstream  trafficLogFile;

#define MANAGET_TYPE_CAST NoCProtocolLayer<NR_OF_INPUT_CHANNEL,\
						 NR_OF_OUTPUT_CHANNEL,\
						 INPUT_CHANNEL_CAPACITY,\
						 OUTPUT_CHANNEL_CAPACITY,\
						 tlm::tlm_base_protocol_types,\
						 tlm::tlm_base_protocol_types,\
						 TLM_GENERIC_PAYLOAD,\
						 TLM_PHASE,\
						 TLM_SYNC_ENUM>

template <int NR_OF_INPUT_CHANNEL = 1,
		  int NR_OF_OUTPUT_CHANNEL = 1,
		  int INPUT_CHANNEL_CAPACITY = 1,
		  int OUTPUT_CHANNEL_CAPACITY = 1,
		  class TLM_NOC_INPUT_PAYLOAD = tlm_noc::noc_flit_payload,
		  class TLM_NOC_OUTPUT_PAYLOAD = tlm_noc::noc_phit_payload,
		  class TLM_GENERIC_PAYLOAD = tlm::tlm_generic_payload,
		  class TLM_PHASE= tlm::tlm_phase,
		  class TLM_SYNC_ENUM = tlm::tlm_sync_enum
		  >
class PhysicalLayer : public NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> {
public:


	SC_HAS_PROCESS(PhysicalLayer);

	PhysicalLayer(sc_core::sc_module_name name, int id, tlm_noc::NoCNIManager<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>,
											  transaction_type,
											  tlm::tlm_base_protocol_types,
											  NR_OF_INPUT_CHANNEL,
											  NR_OF_OUTPUT_CHANNEL,
								              INPUT_CHANNEL_CAPACITY,
								              OUTPUT_CHANNEL_CAPACITY,
								              tlm::tlm_base_protocol_types,
											  tlm::tlm_base_protocol_types,
								              TLM_GENERIC_PAYLOAD,
								              TLM_PHASE,
								              TLM_SYNC_ENUM	>* ni_ma, TileType ty) :
		NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>(name, id),
		m_ni_manager(ni_ma),
        mPhysicalLayerDelay(1, sc_core::SC_NS),
		m_type(ty)
	{

		m_channel_state = new ChannelStateType[NR_OF_OUTPUT_CHANNEL];
		m_input_buffer  = new std::list<transaction_type*>[NR_OF_INPUT_CHANNEL];
		for (int i = 0; i < NR_OF_OUTPUT_CHANNEL; i++)
			m_channel_state[i] = IDLE;

		m_action_label[PHYSICAL_MAIN_ACTION_WAIT] = m_ni_manager->register_activity(&MANAGET_TYPE_CAST::function_31); //phydownfw_cb
		m_action_label[PHYSICAL_WAIT_LIST_ACTIVE] = m_ni_manager->register_activity(NULL);
		m_action_label[PHYSICAL_DOWN_STREAM_FW]   = m_ni_manager->register_activity(&MANAGET_TYPE_CAST::function_31); //phydownfw_cb
		m_action_label[PHYSICAL_DOWN_STREAM_BW]   = m_ni_manager->register_activity(&MANAGET_TYPE_CAST::function_32); //phydownbw_cb
		m_action_label[PHYSICAL_UP_STREAM_FW]     = m_ni_manager->register_activity(&MANAGET_TYPE_CAST::function_33); //phyupfw_cb
		m_action_label[PHYSICAL_UP_STREAM_BW]     = m_ni_manager->register_activity(&MANAGET_TYPE_CAST::function_34); //phyupbw_cb
		m_action_label[PHYSICAL_INPUT_BUFF]       = m_ni_manager->register_activity(&MANAGET_TYPE_CAST::function_35); //inputbuff_upstream_cb
		m_action_label[PHYSICAL_DUMMY]            = m_ni_manager->register_activity(NULL); //

	}
	//*********************************************************************************
	// 					Interfaces for receiving messages or packets
	//*********************************************************************************


	sync_enum_type downstream_nb_transport_fw(int initiator_id, transaction_type& r_flit_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " downstream_nb_transport_fw PORT = " << initiator_id << std::endl;
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		r_flit_trans.get_extension(p_flit_trans_ext);

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << std::endl;
#endif
		r_flit_trans.acquire();

		if (phase == tlm::BEGIN_REQ) {
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_flit_trans, phase, t, initiator_id, m_action_label[PHYSICAL_DOWN_STREAM_FW]);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_flit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

		return tlm::TLM_ACCEPTED;
	}

	sync_enum_type downstream_nb_transport_bw(int initiator_id, transaction_type& r_flit_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " downstream_nb_transport_bw PORT = " << initiator_id << std::endl;
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		r_flit_trans.get_extension(p_flit_trans_ext);

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_flit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << std::endl;
#endif
		r_flit_trans.acquire();

		if (phase == tlm::BEGIN_RESP) {
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_flit_trans, phase, mPhysicalLayerDelay, initiator_id, m_action_label[PHYSICAL_DOWN_STREAM_BW]);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_flit_trans.set_response_status(tlm::TLM_OK_RESPONSE);

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
		trafficLogFile << name() << " upstream_nb_transport_fw PORT = " << initiator_id << std::endl;
		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		r_phit_trans.get_extension(p_phit_trans_ext);

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_phit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_phit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_flit_id)
					   << " m_phit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_phit_id) << std::endl;
#endif
		r_phit_trans.acquire();
			if (phase == tlm::BEGIN_REQ) {
				m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_phit_trans, phase, mPhysicalLayerDelay, initiator_id, m_action_label[PHYSICAL_UP_STREAM_FW]);
				phase = tlm::END_REQ;
				t = sc_core::SC_ZERO_TIME;
				r_phit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
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
		r_phit_trans.acquire();

		if (phase == tlm::BEGIN_RESP) {
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_phit_trans, phase, mPhysicalLayerDelay, initiator_id, m_action_label[PHYSICAL_UP_STREAM_BW]);
			phase = tlm::END_RESP;
			t = sc_core::SC_ZERO_TIME;
			r_phit_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

		return tlm::TLM_ACCEPTED;
	}

	//phydownfw_cb
	virtual void function_31 (transaction_type* p_flit_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_31 PORT = " << input_ch_no << std::endl;
#endif
		
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);

	
		DownStreamFW(p_flit_trans, input_ch_no);
	}
	//phydownbw_cb
	virtual void function_32 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}
	//phyupfw_cb
	virtual void function_33 (transaction_type* p_phit_trans, unsigned int input_ch_no) {
		tlm_noc::noc_phit_payload* p_phit_trans_ext;
		p_phit_trans->get_extension(p_phit_trans_ext);
#ifdef DEBUG
		trafficLogFile << name() << " phyupfw_cb PORT = " << input_ch_no << std::endl;


		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_phit_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_phit_trans_ext, m_packet_id)
					   << " m_flit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_flit_id)
					   << " m_phit_id    = " << noc_gp_hdr(p_phit_trans_ext, m_phit_id) << std::endl;

#endif



		if (noc_gp_hdr(p_phit_trans_ext, m_phit_id) == 0  && noc_gp_hdr(p_phit_trans_ext, m_data_phit) == false) { //First phit which is control
			noc_gp_hdr(p_phit_trans_ext, m_ack) = true;

			send_ack_phit(p_phit_trans, input_ch_no);
		}
		if (noc_gp_hdr(p_phit_trans_ext, m_data_phit) == true) { // First phit which is data
			save_flit(p_phit_trans_ext, input_ch_no);
			p_phit_trans->release();
		}
	}
	//phyupbw_cb
	virtual void function_34(transaction_type* p_phit_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_34 PORT = " << input_ch_no << std::endl;
#endif
		UpStreamBW(p_phit_trans, input_ch_no);
	}

	// PHYSICAL_INPUT_BUFF
	// A complete flit has received 
	virtual void function_35(transaction_type* p_flit_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_35 PORT = " << input_ch_no << std::endl;
#endif
		sc_core::sc_time t = sc_core::sc_time(INPUT_BUFFER, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_REQ ; ///initialization needed
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);

		p_flit_trans = m_input_buffer[input_ch_no].front();
		switch (up_layer->upstream_nb_transport_fw(input_ch_no, *p_flit_trans, phase, t)) {
		case tlm::TLM_COMPLETED:
		case tlm::TLM_UPDATED:
			if (phase == tlm::END_REQ) {
				if (p_flit_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {

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
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, *p_flit_trans, phase, t, input_ch_no, m_action_label[PHYSICAL_INPUT_BUFF]);

	}

	void send_out(unsigned int input_ch_no);

	//*********************************************************************************
	// 					Threads for processing received Message or Packets
	//*********************************************************************************


	void DownStreamFW(transaction_type* p_flit_trans, unsigned int input_ch_no){
#ifdef DEBUG
		trafficLogFile << name() << " DownStreamFW PORT = " << input_ch_no << std::endl;
#endif

		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
		
		if (m_channel_state[input_ch_no] == IDLE) {
#ifdef DEBUG
			trafficLogFile << name() << " m_channel_state[input_ch_no] == IDLE DownStreamFW PORT = " << input_ch_no << std::endl;
#endif
			send_req_phit(p_flit_trans_ext, input_ch_no);
			m_channel_state[input_ch_no] = WAIT;
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, *p_flit_trans, input_ch_no, m_action_label[PHYSICAL_MAIN_ACTION_WAIT]);
		} else if (m_channel_state[input_ch_no] == WAIT) {
#ifdef DEBUG
			trafficLogFile << name() << " m_channel_state[input_ch_no] == WAIT DownStreamFW PORT = " << input_ch_no << std::endl;
#endif
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, *p_flit_trans, input_ch_no, m_action_label[PHYSICAL_MAIN_ACTION_WAIT]);
		} else if (m_channel_state[input_ch_no] == FREE) {
#ifdef DEBUG
			trafficLogFile << name() << " m_channel_state[input_ch_no] == FREE DownStreamFW PORT = " << input_ch_no << std::endl;
#endif
			send_data_phit(p_flit_trans_ext, input_ch_no);
			m_channel_state[input_ch_no] = IDLE;
			p_flit_trans->release();
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


		m_channel_state[input_ch_no] = FREE;

		//I have to add an if statement for this
		m_ni_manager->notify((MANAGET_TYPE_CAST*)this, *p_phit_trans, input_ch_no, m_action_label[PHYSICAL_WAIT_LIST_ACTIVE]);
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
		transaction_type* p_phit_trans;
		phase_type phase;
		sc_core::sc_time delay;

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


			delay = sc_core::sc_time(PHYSICAL_TO_PHYSICAL_DELAY, sc_core::SC_NS);
			phase = tlm::BEGIN_REQ;
			switch (down_layer->downstream_nb_transport_fw(input_ch_no, *p_phit_trans, phase, delay)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
				if (phase == tlm::END_REQ) {
					if (p_phit_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {
						std::cout << name() << "  " << sc_core::sc_time_stamp() << " next input port does not have anough space - 3" << std::endl;
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


		} while (p_flit_trans_ext->noc_trans.stream_tail != 0);
	}




	void send_ack_phit(transaction_type* p_phit_trans, int input_ch_no) {
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
				
				m_ni_manager->notify((MANAGET_TYPE_CAST*)this, *p_flit_trans, phase, t, input_ch_no, m_action_label[PHYSICAL_INPUT_BUFF]);
			}
		} else {
			transaction_type* p_flit_trans;
			p_flit_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_flit_payload* p_flit_trans_ext;
			p_flit_trans->get_extension(p_flit_trans_ext);
			p_flit_trans_ext->noc_trans << p_phit_trans_ext->noc_trans;
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

	tlm_noc::NoCNIManager<NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
											  NR_OF_OUTPUT_CHANNEL,
								              INPUT_CHANNEL_CAPACITY,
								              OUTPUT_CHANNEL_CAPACITY,
								              tlm::tlm_base_protocol_types,
                                              tlm::tlm_base_protocol_types,
								              TLM_GENERIC_PAYLOAD,
								              TLM_PHASE,
								              TLM_SYNC_ENUM>,
											  transaction_type,
											  tlm::tlm_base_protocol_types,
											  NR_OF_INPUT_CHANNEL,
											  NR_OF_OUTPUT_CHANNEL,
								              INPUT_CHANNEL_CAPACITY,
								              OUTPUT_CHANNEL_CAPACITY,
								              tlm::tlm_base_protocol_types,
                                              tlm::tlm_base_protocol_types,
								              TLM_GENERIC_PAYLOAD,
								              TLM_PHASE,
								              TLM_SYNC_ENUM >*	m_ni_manager;

	TileType                                                m_type;
	tlm_noc::NoRCMM                                         m_mm;
	sc_core::sc_time                                        mPhysicalLayerDelay;


	enum ChannelStateEnum  { IDLE,
							 WAIT,
							 FREE,
							 BLKED
						   };
	typedef ChannelStateEnum  ChannelStateType;

	ChannelStateType*                                       m_channel_state;
	std::list<transaction_type*>*                           m_input_buffer;
	std::map<physical_ni_part_type, unsigned int>           m_action_label;
};




#endif /*PHYSICAL_LAYER_H_*/
