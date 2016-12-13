//******************************************************************************
//			Electrical Engineering Departmenet
//			University of Bristol
//
//			Author:   Mohammad Hosseinabady
//			Email:    M.Hosseinabady@bristol.ac.uk
//
//			Date:     12 September 2008
//******************************************************************************
#ifndef NETWORK_LAYER_H_
#define NETWORK_LAYER_H_

#include "interface.h"
#include "model/modules/include/noc_tile.h"
#include "NoRC/protocol_data_units/include/noc_payload.h"
#include "model/modules/include/noc_protocol_layer.h"
#include "model/model.h"

#define INF_COST 10000
#include <map>
using namespace std;

extern std::ofstream  trafficLogFile;

#define MANAGER_TYPE_CAST NoCProtocolLayer<NR_OF_INPUT_CHANNEL,\
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
		  int  INPUT_CHANNEL_CAPACITY = 1,
		  int  OUTPUT_CHANNEL_CAPACITY = 1,
		  class TLM_NOC_INPUT_PAYLOAD = tlm::tlm_base_protocol_types,
		  class TLM_NOC_OUTPUT_PAYLOAD = tlm::tlm_base_protocol_types,
		  class TLM_GENERIC_PAYLOAD = tlm::tlm_generic_payload,
		  class TLM_PHASE= tlm::tlm_phase,
		  class TLM_SYNC_ENUM = tlm::tlm_sync_enum
		  >
class NetworkLayer : public NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> {
public:

	NetworkLayer(sc_core::sc_module_name name,
		int id,
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
								              TLM_SYNC_ENUM	>* ni_ma, TileType ty) :
		NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>(name, id),
	    m_ni_manager(ni_ma),
		mNetworkLayerDelay(1, sc_core::SC_NS),
		m_tile_id(id),
		m_tile_type(ty)
	{

		m_input_buffer  = new std::list<transaction_type*>[NR_OF_INPUT_CHANNEL];

		for (int i = 0; i < NO_OF_RECONFIGURABLE_CORE; i++) {
			m_core_container[i].session = 1;
			m_core_container[i].status = FREE;
		}

		m_action_label[NETWORK_MAIN_ACTION_WAIT] = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_21); //downstreamfw_cb
		m_action_label[NETWORK_WAIT_LIST_ACTIVE] = m_ni_manager->register_activity(NULL);
		m_action_label[NETWORK_DOWN_STREAM_FW]   = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_21); //downstreamfw_cb
		m_action_label[NETWORK_DOWN_STREAM_BW]   = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_22); //downstreambw_cb
		m_action_label[NETWORK_UP_STREAM_FW]     = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_23); //upstreamfw_cb
		m_action_label[NETWORK_UP_STREAM_BW]     = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_24); //upnstreambw_cb
		m_action_label[NETWORK_INPUT_BUFF]       = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_25); //
		m_action_label[NETWORK_DUMMY]            = m_ni_manager->register_activity(NULL); //
	}

	//*********************************************************************************
	// 					Interfaces for receiving messages or packets
	//*********************************************************************************

	sync_enum_type downstream_nb_transport_fw(int initiator_id, transaction_type& r_packet_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " input_nb_transport_fw PORT = " << initiator_id << std::endl;
		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		r_packet_trans.get_extension(p_packet_trans_ext);

		trafficLogFile << " m_message_id = " << noc_gp_hdr(p_packet_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_packet_trans_ext, m_packet_id)
					   << std::endl;
#endif

		r_packet_trans.acquire();

		if (phase == tlm::BEGIN_REQ) {
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_packet_trans, phase, t, initiator_id, m_action_label[NETWORK_DOWN_STREAM_FW]);
			r_packet_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;

			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}
		return tlm::TLM_ACCEPTED;
	}

	sync_enum_type downstream_nb_transport_bw(int initiator_id, transaction_type& r_packet_trans, phase_type& phase, sc_core::sc_time& t) {

		r_packet_trans.acquire();

/*		if (phase == tlm::BEGIN_RESP) {
			PacketKey key = {noc_gp_hdr(r_packet_trans.get_extension<tlm_noc::noc_packet_payload>(),m_session)};
			if (noc_gp_hdr(r_packet_trans.get_extension<tlm_noc::noc_packet_payload>(),m_packet) == tlm_noc::FIRST_PACKET) {
				mPendingPacketTransIterator it = m_pending_response_packet_trans.find(key);
				if(it == m_pending_response_packet_trans.end()) {
					std::vector<transaction_type*>* p_packet_vector;
					p_packet_vector = new std::vector<transaction_type*>;
					m_pending_response_packet_trans[key] = p_packet_vector;
				} else if (noc_gp_hdr(r_packet_trans.get_extension<tlm_noc::noc_packet_payload>(),m_cmd) == tlm_noc::NAK_PACKET ||
					noc_gp_hdr(r_packet_trans.get_extension<tlm_noc::noc_packet_payload>(),m_cmd) == tlm_noc::ACK_PACKET ) {
				} else {

				}
			}
			m_pending_response_packet_trans[key]->push_back(&r_packet_trans);
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_packet_trans, phase, t, initiator_id, m_action_label[NETWORK_DOWN_STREAM_BW]);
			phase = tlm::END_RESP;
			t = sc_core::SC_ZERO_TIME;
			r_packet_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}*/
		return tlm::TLM_ACCEPTED;
	}


	sync_enum_type upstream_nb_transport_fw(int initiator_id, transaction_type& r_flit_trans, phase_type& phase, sc_core::sc_time& t) {

		r_flit_trans.acquire();

#ifdef DEBUG
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		r_flit_trans.get_extension(p_flit_trans_ext);
		trafficLogFile << name() << " inputbuff_upstream_cb PORT = " << initiator_id ;
		trafficLogFile << " message = " << noc_gp_hdr(p_flit_trans_ext, m_message_id) 
					   << " packet = " << noc_gp_hdr(p_flit_trans_ext, m_packet_id)           
			           << " flit = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id)
					   << " time = " << sc_core::sc_time_stamp();
		trafficLogFile << " OK " << std:: endl;

#endif
		if (phase == tlm::BEGIN_REQ) {
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_flit_trans, phase, t, initiator_id, m_action_label[NETWORK_UP_STREAM_FW]);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_flit_trans.set_response_status(tlm::TLM_OK_RESPONSE);

			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

		return tlm::TLM_ACCEPTED;  //unreachable code
	}

	sync_enum_type upstream_nb_transport_bw(int initiator_id, transaction_type& r_flit_trans, phase_type& phase, sc_core::sc_time& t) {
		r_flit_trans.acquire();

		if (phase == tlm::BEGIN_RESP) {
			FlitKey key = {initiator_id};
			if(m_pending_response_flit_trans.find(key) != m_pending_response_flit_trans.end()) {
				if(m_pending_response_flit_trans[key]->empty()) {
					std::vector<transaction_type*>* p_flit_vector;
					p_flit_vector = new std::vector<transaction_type*>;
					m_pending_response_flit_trans[key] = p_flit_vector;
				}
			} else {
				std::vector<transaction_type*>* p_flit_vector;
				p_flit_vector = new std::vector<transaction_type*>;
				m_pending_response_flit_trans[key] = p_flit_vector;
			}
			m_pending_response_flit_trans[key]->push_back(&r_flit_trans);
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_flit_trans, phase, t, initiator_id, m_action_label[NETWORK_UP_STREAM_BW]);
			phase = tlm::END_RESP;
			t = sc_core::SC_ZERO_TIME;
			r_flit_trans.set_response_status(tlm::TLM_OK_RESPONSE);

			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}
		return tlm::TLM_COMPLETED;
	}


private:
	///downstreamfw_cb
	void function_21(transaction_type* p_trans, unsigned int input_ch_no){
#ifdef DEBUG
		trafficLogFile << name() << " function_21 PORT = " << input_ch_no << std::endl;
#endif
		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		p_trans->get_extension(p_packet_trans_ext);

		if (noc_gp_hdr(p_packet_trans_ext,  m_dst_tile) == m_tile_id) {
			
			input_ch_no = noc_gp_hdr(p_packet_trans_ext,  m_session);
			sc_core::sc_time t = sc_core::sc_time(INPUT_BUFFER, sc_core::SC_NS);
			phase_type phase ; ///initialization needed
			m_input_buffer[input_ch_no].push_back(p_trans);
			if (m_input_buffer[input_ch_no].size() > 0) {
				p_trans = m_input_buffer[input_ch_no].front();
				m_ni_manager->notify((MANAGER_TYPE_CAST*)this, *p_trans, phase, t, input_ch_no, m_action_label[NETWORK_INPUT_BUFF]);
				m_input_buffer[input_ch_no].pop_front();
			}
		} else {
			SendPacket(p_trans, input_ch_no);
			p_trans->release();
		}
	}

	///downstreambw_cb
	void function_22(transaction_type* p_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_22 PORT = " << input_ch_no << std::endl;
#endif

	}

	///upstreamfw_cb
	void function_23(transaction_type* p_flit_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_23 PORT = " << input_ch_no << std::endl;
#endif
		tlm_noc::noc_flit_payload* p_flit_trans_ext;
		p_flit_trans->get_extension(p_flit_trans_ext);
		save_flit(p_flit_trans_ext, input_ch_no);
		p_flit_trans->release();
	}

	///upstreambw_cb
	void function_24(transaction_type* p_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_24 PORT = " << input_ch_no << std::endl;
#endif
	}


	
	void function_25(transaction_type* p_packet_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " inputbuff_upstream_cb PORT = " << input_ch_no << std::endl;
#endif
		sc_core::sc_time t = sc_core::sc_time(INPUT_BUFFER, sc_core::SC_NS);
		phase_type phase = tlm::BEGIN_REQ ; ///initialization needed
		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		p_packet_trans->get_extension(p_packet_trans_ext);

		//p_packet_trans = m_input_buffer[input_ch_no].front();
		switch (up_layer->upstream_nb_transport_fw(input_ch_no, *p_packet_trans, phase, t)) {
		case tlm::TLM_COMPLETED:
		case tlm::TLM_UPDATED:
			if (phase == tlm::END_REQ) {
				if (p_packet_trans->get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {

				} else if (p_packet_trans->get_response_status() == tlm::TLM_OK_RESPONSE) {
					p_packet_trans->release();
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
		//m_input_buffer[input_ch_no].pop_front();
		if (m_input_buffer[input_ch_no].size() > 0) {
			p_packet_trans = m_input_buffer[input_ch_no].front();
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, *p_packet_trans, phase, t, input_ch_no, m_action_label[NETWORK_INPUT_BUFF]);
			m_input_buffer[input_ch_no].pop_front();
		}
	}

	void SendPacket(transaction_type* p_packet_trans, unsigned int ch_no) {
		sc_core::sc_time	delay;
		phase_type 			phase;
		int 				channel_no;

		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		p_packet_trans->get_extension(p_packet_trans_ext);

		int flit_id = 0;

		do {
			transaction_type* 	p_flit_trans;

			p_flit_trans= m_mm.allocate();
			p_flit_trans->acquire();

			tlm_noc::noc_flit_payload* p_flit_trans_ext = new tlm_noc::noc_flit_payload;
			p_flit_trans->set_extension(p_flit_trans_ext);
			p_packet_trans_ext->noc_trans >> p_flit_trans_ext->noc_trans;

			noc_gp_hdr(p_flit_trans_ext,  m_cmd) = tlm_noc::NORMAL_FLIT;
			noc_gp_hdr(p_flit_trans_ext,  m_flit_id) = flit_id;
			noc_gp_hdr(p_flit_trans_ext,  m_packet_id) = noc_gp_hdr(p_packet_trans_ext,  m_packet_id);
			noc_gp_hdr(p_flit_trans_ext,  m_message_id) = noc_gp_hdr(p_packet_trans_ext,  m_message_id);
			noc_gp_hdr(p_flit_trans_ext,  m_cast) = noc_gp_hdr(p_packet_trans_ext, m_cast);
			noc_gp_hdr(p_flit_trans_ext,  m_tile_id) =  m_tile_id;
			noc_gp_hdr(p_flit_trans_ext,  m_routing) = noc_gp_hdr(p_packet_trans_ext,  m_routing);
			noc_gp_hdr(p_flit_trans_ext,  m_dst_tile) = noc_gp_hdr(p_packet_trans_ext,  m_dst_tile);
			noc_gp_hdr(p_flit_trans_ext,  m_session) = noc_gp_hdr(p_packet_trans_ext,  m_session);

			noc_gp_hdr(p_flit_trans_ext,m_rls_flit)    = false;
			if (flit_id == 0) {
				noc_gp_hdr(p_flit_trans_ext,m_flit)    = tlm_noc::FIRST_FLIT;
				noc_gp_hdr(p_flit_trans_ext,  m_head_flit) = true;
			} else if (p_packet_trans_ext->noc_trans.stream_tail == 0) {
				noc_gp_hdr(p_flit_trans_ext,m_flit)    = tlm_noc::LAST_FLIT;
				noc_gp_hdr(p_flit_trans_ext,m_rls_flit)    = true;                   //LAST FLIT in a PACKET to release the wormhole switching 
				noc_gp_hdr(p_flit_trans_ext,  m_head_flit) = false;
			} else {
				noc_gp_hdr(p_flit_trans_ext,m_flit)    = tlm_noc::MIDDLE_FLIT;
				noc_gp_hdr(p_flit_trans_ext,  m_head_flit) = false;
			}
			phase = tlm::BEGIN_REQ;
			delay = sc_core::SC_ZERO_TIME;

			channel_no = ch_no;

			phase = tlm::BEGIN_REQ;
			delay = sc_core::sc_time(NETWORK_TO_PHYSICAL_DELAY, sc_core::SC_NS);
			switch (down_layer->downstream_nb_transport_fw(channel_no, *p_flit_trans, phase, delay)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
				p_flit_trans->release();
				break;
			case tlm::TLM_ACCEPTED:
				UNDER_CONSTRUCTION();
			default:
				std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
				assert(false); exit(1);
			}
			flit_id++;
		}while (p_packet_trans_ext->noc_trans.stream_tail != 0);
		
	}

private:
	sc_core::sc_time           mNetworkLayerDelay;
	int cost (int i_task_cost) {
		return 0;
	}


	void save_flit(tlm_noc::noc_flit_payload* p_flit_trans_ext, unsigned int input_ch_no) {
		//input_ch_no = 0;
#ifdef DEBUG
		trafficLogFile << name() << " save_flit = " << " noc_gp_hdr(p_flit_trans_ext, m_flit_id) = " << noc_gp_hdr(p_flit_trans_ext, m_flit_id) <<
			" noc_gp_hdr(p_flit_trans_ext, m_flit) = " << noc_gp_hdr(p_flit_trans_ext, m_flit) << 
			" noc_gp_hdr(p_flit_trans_ext, m_message_id)=" << noc_gp_hdr(p_flit_trans_ext, m_message_id) << " p_flit_trans_ext = " << p_flit_trans_ext << std::endl;
#endif

		input_ch_no = noc_gp_hdr(p_flit_trans_ext, m_session);
		if (noc_gp_hdr(p_flit_trans_ext, m_flit_id) == 0) {
			transaction_type* p_packet_trans;
			p_packet_trans = m_mm.allocate();
			p_packet_trans->acquire();
			tlm_noc::noc_packet_payload* p_packet_trans_ext;

			p_packet_trans_ext = new tlm_noc::noc_packet_payload;
			p_packet_trans->set_extension(p_packet_trans_ext);

			p_packet_trans_ext->noc_trans << p_flit_trans_ext->noc_trans;
			m_input_buffer[input_ch_no].push_back(p_packet_trans);

		} else if (noc_gp_hdr(p_flit_trans_ext, m_flit) == tlm_noc::LAST_FLIT) {
			sc_core::sc_time t = sc_core::sc_time(INPUT_BUFFER, sc_core::SC_NS);
			phase_type phase ; ///initialization needed
			trafficLogFile << " m_input_buffer[input_ch_no].size() = " << m_input_buffer[input_ch_no].size() << std::endl;
			transaction_type* p_packet_trans;
			p_packet_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_packet_payload* p_packet_trans_ext;
			p_packet_trans->get_extension(p_packet_trans_ext);
			p_packet_trans_ext->noc_trans << p_flit_trans_ext->noc_trans;
			
			if (m_input_buffer[input_ch_no].size() > 0) {
				p_packet_trans = m_input_buffer[input_ch_no].front();
				m_ni_manager->notify((MANAGER_TYPE_CAST*)this, *p_packet_trans, phase, t, input_ch_no, m_action_label[NETWORK_INPUT_BUFF]);
				m_input_buffer[input_ch_no].pop_front();
			}

		} else {
			transaction_type* p_packet_trans;
			trafficLogFile << " m_input_buffer[input_ch_no].size() = " << m_input_buffer[input_ch_no].size() << std::endl;
			p_packet_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_packet_payload* p_packet_trans_ext;
			p_packet_trans->get_extension(p_packet_trans_ext);
			p_packet_trans_ext->noc_trans << p_flit_trans_ext->noc_trans;
		}
	}

private:
	std::list<transaction_type*>*                           m_input_buffer;


	enum NETWORK_NIPartEnum {NETWORK_DOWN_STREAM_FW,
							 NETWORK_DOWN_STREAM_BW,
							 NETWORK_UP_STREAM_FW,
							 NETWORK_UP_STREAM_BW,
							 NETWORK_MAIN_ACTION_WAIT,
							 NETWORK_WAIT_LIST_ACTIVE,
							 NETWORK_INPUT_BUFF,
							 NETWORK_DUMMY
							};
	typedef NETWORK_NIPartEnum network_ni_part_type;

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


	std::map<network_ni_part_type, unsigned int>                m_action_label;


///OLD variables


	struct FlitKey {
		int channel_number;
//		int flit_id;
		friend bool operator<(const FlitKey& lv, const FlitKey& rv){
			return lv.channel_number < rv.channel_number;
		}
	};



	typedef typename std::map<FlitKey,   std::vector<transaction_type*>*  >::iterator mPendingFlitTransIterator;

	std::map<int, transaction_type*>	 m_received_request_packet_trans; //packet from received flits


	std::map<FlitKey, std::vector<transaction_type*>* >	m_pending_request_flit_trans;
	std::map<FlitKey, std::vector<transaction_type*>* >	m_pending_response_flit_trans;


	int				m_tile_id;
	TileType		m_tile_type;

	enum CoreStatus {FREE, RESERVED, ACTIVE, IDEL};
	struct  ReconfigurableCore {
		void* 		function;
		CoreStatus	status;
		int			session;
	};

	ReconfigurableCore m_core_container[NO_OF_RECONFIGURABLE_CORE];

	std::map<int, int>				m_channel_to_session;//m_response_channel_table;
	tlm_noc::NoRCMM                                      m_mm;
};


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

