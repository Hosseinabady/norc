#ifndef ERROR_CONTROL_LAYER_H_
#define ERROR_CONTROL_LAYER_H_


#include "interface.h"
#include "NoRC/protocol_data_units/include/noc_payload.h"
#include <map>
#include "model/model.h"
#include <stdlib.h>
#include <systemc>
#include "model/modules/include/noc_connector.h"
#include "tasks/include/taskmanager.h"

using namespace std;


extern std::ofstream outputLogFile;
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
		  int INPUT_CHANNEL_CAPACITY = 1,
		  int OUTPUT_CHANNEL_CAPACITY = 1,
		  class TLM_NOC_INPUT_PAYLOAD = tlm::tlm_base_protocol_types,
		  class TLM_NOC_OUTPUT_PAYLOAD = tlm_noc::noc_packet_payload,
		  class TLM_GENERIC_PAYLOAD = tlm::tlm_generic_payload,
		  class TLM_PHASE= tlm::tlm_phase,
		  class TLM_SYNC_ENUM = tlm::tlm_sync_enum
		  >
class TransportLayer : public NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> {
public:
	TransportLayer(sc_core::sc_module_name name,
		           int id,
				   tlm_noc::NoCNIManager<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>,
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
								              TLM_SYNC_ENUM	>* ni_ma,
				   TileTypeEnum ty) :
		NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>(name, id),
		mTransportLayerDelay(1, sc_core::SC_NS),
		m_ni_manager(ni_ma),
		tile_id(id)
	{
		for (int i = 0; i < NO_OF_RECONFIGURABLE_CORE; i++) {
			reconfigurable_area[i].task = NULL;
			reconfigurable_area[i].activity = FREE;
		}

		m_input_buffer  = new std::list<transaction_type*>[NR_OF_INPUT_CHANNEL];

		m_action_label[TRANSPORT_MAIN_ACTION_WAIT] = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_11); //downstreamfw_cb
		m_action_label[TRANSPORT_WAIT_LIST_ACTIVE] = m_ni_manager->register_activity(NULL);
		m_action_label[TRANSPORT_DOWN_STREAM_FW]   = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_11); //downstreamfw_cb
		m_action_label[TRANSPORT_DOWN_STREAM_BW]   = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_12); //downstreambw_cb
		m_action_label[TRANSPORT_UP_STREAM_FW]     = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_13); //upstreamfw_cb
		m_action_label[TRANSPORT_UP_STREAM_BW]     = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_14); //upnstreambw_cb
		m_action_label[TRANSPORT_INPUT_BUFF]       = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_15); //
		m_action_label[TRANSPORT_INPUT_RELEASE]    = m_ni_manager->register_activity(&MANAGER_TYPE_CAST::function_16); //
	}

	//*********************************************************************************
	// 					Interfaces for receiving messages or packets
	//*********************************************************************************

	virtual sync_enum_type downstream_nb_transport_fw(int initiator_id, transaction_type& r_request_message_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " downstream_nb_transport_fw PORT = " << initiator_id << std::endl;
#endif
		r_request_message_trans.acquire();

		if (phase == tlm::BEGIN_REQ) {
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_request_message_trans, phase, t,initiator_id, m_action_label[TRANSPORT_DOWN_STREAM_FW]);
			r_request_message_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}
		return tlm::TLM_ACCEPTED;
	}

	virtual sync_enum_type downstream_nb_transport_bw(int initiator_id, transaction_type& r_response_message_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " downstream_nb_transport_bw PORT = " << initiator_id << std::endl;
#endif
		r_response_message_trans.acquire();

		if (phase == tlm::BEGIN_RESP) {
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_response_message_trans, phase, t,initiator_id, m_action_label[TRANSPORT_DOWN_STREAM_BW]);
			phase = tlm::END_RESP;
			t = sc_core::SC_ZERO_TIME;
			r_response_message_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

		return tlm::TLM_ACCEPTED;
	}


	virtual sync_enum_type upstream_nb_transport_fw(int initiator_id, transaction_type& r_request_packet_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " upstream_nb_transport_fw PORT = " << initiator_id << std::endl;
#endif
		r_request_packet_trans.acquire();


		if (phase == tlm::BEGIN_REQ) {
			m_ni_manager->notify((MANAGER_TYPE_CAST*)this, r_request_packet_trans, phase, t,initiator_id, m_action_label[TRANSPORT_UP_STREAM_FW]);
			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_request_packet_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

	    return tlm::TLM_COMPLETED;  //unreachable code
	}

	virtual sync_enum_type upstream_nb_transport_bw(int initiator_id, transaction_type& r_response_packet_trans, phase_type& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " upstream_nb_transport_bw PORT = " << initiator_id << std::endl;
#endif
		r_response_packet_trans.acquire();

		if (phase == tlm::BEGIN_RESP) {
			m_ni_manager->notify((MANAGET_TYPE_CAST*)this, r_response_packet_trans, phase, t,initiator_id, m_action_label[TRANSPORT_UP_STREAM_BW]);
			phase = tlm::END_RESP;
			t = sc_core::SC_ZERO_TIME;
			r_response_packet_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

	    return tlm::TLM_COMPLETED;  //unreachable code
	}




	virtual sync_enum_type nb_external_transport_fw(char* FName) {
		// EXTERNAL TASK REQUEST
		outputLogFile << "Third Layer function call " << FName << " tile_id= " << tile_id << std::endl;
		NoCTask* tsk = new TaskManager;
		tlm_noc::Volunteer* vl = new tlm_noc::Volunteer;
		tsk->set_id(0);
		tsk->set_fileName(FName);
		Reconfiguration(tsk, vl);
		if (vl->core_id != -1) { 
			reconfigurable_area[vl->core_id].activity = ACTIVE;
			reconfigurable_area[vl->core_id].task     = tsk;
		
			tlm::tlm_generic_payload* p_message_trans;
    		p_message_trans = m_mm.allocate();
			p_message_trans->acquire();
		
			tlm_noc::noc_message_payload* p_message_trans_ext;
			p_message_trans_ext = new tlm_noc::noc_message_payload;
		
			p_message_trans->set_extension(p_message_trans_ext);

			noc_gp_hdr(p_message_trans_ext, m_cmd) = tlm_noc::TASK_ACTIVE_REQ;
			noc_gp_hdr(p_message_trans_ext, m_req_task) = tsk;
			noc_gp_hdr(p_message_trans_ext, m_src_core) = vl->core_id;
			noc_gp_hdr(p_message_trans_ext, m_dst_core) = vl->core_id;
			noc_gp_hdr(p_message_trans_ext, m_volunteer) = NULL;

			sc_core::sc_time           t(sc_core::SC_ZERO_TIME);
			phase_type phase = tlm::BEGIN_REQ ;
			p_message_trans->set_address((sc_dt::uint64)(vl->core_id+1));
			std::cout << "Task Manager Tile id =" << tile_id << std::endl;
			std::cout << "Task Manager Core id =" << vl->core_id << std::endl;
			switch (up_layer->upstream_nb_transport_fw(0, *p_message_trans, phase, t)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
				p_message_trans->release();
				break;
			case tlm::TLM_ACCEPTED:
				UNDER_CONSTRUCTION();
				break;
			default:
				std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
				assert(false); exit(1);
			}
			return tlm::TLM_COMPLETED;  
		} else
			return tlm::TLM_UPDATED;  // task manager cannot be mapped
		
	}
private:
	void function_11(transaction_type* p_trans, unsigned int input_ch_no){
#ifdef DEBUG
		trafficLogFile << name() << " function_11 PORT = " << input_ch_no << std::endl;
#endif
		phase_type phase;
		sc_core::sc_time delay;
		tlm_noc::noc_message_payload* p_message_trans_ext;
		p_trans->get_extension(p_message_trans_ext);

		if (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::APPLICATION_MAPPED) {
			m_external_port->nb_transport_bw(tile_id, *p_trans, phase, delay);
			return;
		}

		int next_tile = -1;
		int previous_tile = -1;
		int port_selects[4] = {0};
		tlm_noc::Volunteer* vl = noc_gp_hdr(p_message_trans_ext,  m_volunteer);
		int rand_route;
		while(next_tile == -1 && !(port_selects[0]*port_selects[1]*port_selects[2]*port_selects[3])) {
			rand_route = rand()%4;
			port_selects[rand_route] = 1;


			switch (rand_route) {
			case 0:
				if (tile_id+1 < NO_OF_ROW_CONST*NO_OF_COL_CONST && !find(vl, tile_id+1) && (tile_id % 8) )
					next_tile = tile_id+1;
				break;
			case 1:
				if (tile_id-NO_OF_COL_CONST > 0 && !find(vl, tile_id-NO_OF_COL_CONST) )
					next_tile = tile_id-NO_OF_COL_CONST;
				break;
			case 2:
				if (tile_id-1 > 0 && !find(vl, tile_id-1) && ((tile_id%8) - 1))
					next_tile = tile_id-1;
				break;
			case 3:
				if (tile_id+NO_OF_COL_CONST < NO_OF_ROW_CONST*NO_OF_COL_CONST && !find(vl, tile_id+NO_OF_COL_CONST))
					next_tile = tile_id+NO_OF_COL_CONST;
				break;
			}
		}
//#ifdef DEBUG
		    outputLogFile << "tile_id = " << tile_id << " rand_route = " << rand_route << "next_tile_id = " << next_tile << std::endl;
//#endif
		if (next_tile == -1) {
			outputLogFile << "===================Loop Detected Return===================" << std::endl;
			next_tile = vl->tile_id;
		}
		int packet_id = 0;
		
		do {
			transaction_type* p_packet_trans;
			p_packet_trans = m_mm.allocate();
			p_packet_trans->acquire();

			tlm_noc::noc_packet_payload* p_packet_trans_ext;
			p_packet_trans_ext = new tlm_noc::noc_packet_payload;

			p_message_trans_ext->noc_trans >> p_packet_trans_ext->noc_trans;
			p_packet_trans->set_extension(p_packet_trans_ext);

			noc_gp_hdr(p_packet_trans_ext, m_packet_id) = packet_id;
			noc_gp_hdr(p_packet_trans_ext, m_message_id) = noc_gp_hdr(p_message_trans_ext, m_message_id);
			noc_gp_hdr(p_packet_trans_ext, m_crc) = 0;
			noc_gp_hdr(p_packet_trans_ext, m_cmd) = tlm_noc::REQ_PACKET;

			noc_gp_hdr(p_packet_trans_ext, m_src_core) = noc_gp_hdr(p_message_trans_ext, m_src_core);;
			noc_gp_hdr(p_packet_trans_ext, m_cast)     = noc_gp_hdr(p_message_trans_ext, m_cast);
			noc_gp_hdr(p_packet_trans_ext, m_dst_tile) = noc_gp_hdr(p_message_trans_ext, m_dst_tile);
			noc_gp_hdr(p_packet_trans_ext, m_src_tile) = noc_gp_hdr(p_message_trans_ext, m_src_tile);
			noc_gp_hdr(p_packet_trans_ext, m_session) = noc_gp_hdr(p_message_trans_ext, m_dst_core);
			if (noc_gp_hdr(p_message_trans_ext, m_cast) == tlm_noc::UNI_CAST && 
			   (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::ACK_SEEK || 
			    noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_ACTIVE_REQ || 
				noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_ACTIVE_ACK ||
				noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_CMD_START  ||
				noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::WRITE_DATA) ||
				noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_RLS_REQ || 
				noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_RLS_ACK )
				noc_gp_hdr(p_packet_trans_ext, m_routing) = tlm_noc::XY;
			if (noc_gp_hdr(p_message_trans_ext, m_cast) == tlm_noc::BRD_CAST && noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_SEEK) {
				noc_gp_hdr(p_packet_trans_ext, m_routing) = tlm_noc::XY;//DIR_FLOODING;//PUR_FLOODING;
				noc_gp_hdr(p_packet_trans_ext,  m_dst_tile) = next_tile;
			}
			if (packet_id == 0) {
				noc_gp_hdr(p_packet_trans_ext,m_packet)    = tlm_noc::FIRST_PACKET;
			} else if (p_message_trans_ext->noc_trans.stream_tail == 0) {
				noc_gp_hdr(p_packet_trans_ext,m_packet)    = tlm_noc::LAST_PACKET;
			} else {
				noc_gp_hdr(p_packet_trans_ext,m_packet)    = tlm_noc::MIDDLE_PACKET;
			}
				PacketSignature key ={noc_gp_hdr(p_packet_trans_ext,m_src_core), packet_id};
/*			if (noc_gp_hdr(p_message_trans_ext, m_cmd) != tlm_noc::ACK_SEEK  &&
				noc_gp_hdr(p_message_trans_ext, m_cmd) != tlm_noc::TASK_SEEK )           //task requests do not need flow control
				m_request_packet_pending[key]= p_packet_trans;*/
			phase = tlm::BEGIN_REQ;
			delay = sc_core::sc_time(TRANSPORT_TO_NETWORK_DELAY, sc_core::SC_NS);
			int out_channel = noc_gp_hdr(p_packet_trans_ext, m_src_core);
			switch (down_layer->downstream_nb_transport_fw(out_channel, *p_packet_trans, phase, delay)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
				if (phase == tlm::END_REQ) {
					p_packet_trans->release();
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
			packet_id++;
		} while (p_message_trans_ext->noc_trans.stream_tail != 0);

		p_trans->release();
	}



	
	//upstreamfw_cb
	void function_13(transaction_type* p_packet_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_13 PORT = " << input_ch_no << std::endl;
#endif
		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		p_packet_trans->get_extension(p_packet_trans_ext);
		save_packet(p_packet_trans_ext, input_ch_no);
		p_packet_trans->release();
	}



	virtual void function_15(transaction_type* p_message_trans, unsigned int input_ch_no) {
#ifdef DEBUG
		trafficLogFile << name() << " inputbuff_upstream_cb PORT = " << input_ch_no << std::endl;
#endif
		sc_core::sc_time t = sc_core::sc_time(INPUT_BUFFER, sc_core::SC_NS);
        phase_type phase = tlm::BEGIN_REQ ; ///initialization needed
        tlm_noc::noc_message_payload* p_message_trans_ext;
        p_message_trans->get_extension(p_message_trans_ext);

        if (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_SEEK) {
            tlm_noc::noc_message_payload* p_message_trans_ext;
            p_message_trans->get_extension(p_message_trans_ext);
            noc_gp_hdr(p_message_trans_ext, m_message_id) = 10;
			int life_time = noc_gp_hdr(p_message_trans_ext, m_life_time);
			if (life_time == 1) {
				noc_gp_hdr(p_message_trans_ext, m_dst_tile) = noc_gp_hdr(p_message_trans_ext, m_src_tile);
				noc_gp_hdr(p_message_trans_ext, m_src_tile) = tile_id;
				noc_gp_hdr(p_message_trans_ext, m_dst_core) = noc_gp_hdr(p_message_trans_ext, m_src_core);

	            noc_gp_hdr(p_message_trans_ext, m_cmd)      = tlm_noc::ACK_SEEK;
                noc_gp_hdr(p_message_trans_ext, m_cast)     = tlm_noc::UNI_CAST;
			} else {			
				noc_gp_hdr(p_message_trans_ext, m_life_time) = life_time - 1;
			}
			#ifdef DEBUG
			outputLogFile << name() << " Reconfiguration "<< std::endl;
			trafficLogFile << name() << " Reconfiguration "<< std::endl;
			#endif
			ReconfigurationController(p_message_trans, 1); // 1: for mapping a task
            
            sc_core::sc_time           delay(sc_core::SC_ZERO_TIME);
            m_ni_manager->notify((MANAGER_TYPE_CAST*)this, *p_message_trans, phase, t, input_ch_no, m_action_label[TRANSPORT_DOWN_STREAM_FW]);
		} else if  (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::ACK_SEEK ||
			        noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_RLS_ACK) {

			p_message_trans->set_address((sc_dt::uint64)(noc_gp_hdr(p_message_trans_ext, m_dst_core)+1));
			switch (up_layer->upstream_nb_transport_fw(input_ch_no, *p_message_trans, phase, t)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
				//p_flit_trans->free_all_extensions();
				p_message_trans->release();
				break;
			case tlm::TLM_ACCEPTED:
				UNDER_CONSTRUCTION();
				break;
			default:
				std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
				assert(false); exit(1);
			}
			
		} else if  (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_ACTIVE_REQ) {

			int i = noc_gp_hdr(p_message_trans_ext, m_dst_core);
			trafficLogFile << name() << " TASK ACTIVATION AGAIN = "  << i << " ACTIVITY = " << reconfigurable_area[i].activity << std::endl;
			if (reconfigurable_area[i].activity == FREE) {
				trafficLogFile << name() << " TASK ACTIVATION AGAIN = " << i << " ACTIVITY = " << reconfigurable_area[i].activity << std::endl;
				reconfigurable_area[i].activity = ACTIVE;
				reconfigurable_area[i].task     = noc_gp_hdr(p_message_trans_ext, m_req_task);
	

				trafficLogFile << name() << " TASK ACTIVATION AGAIN m_dst_core= " << noc_gp_hdr(p_message_trans_ext, m_dst_core) << std::endl;
				p_message_trans->set_address((sc_dt::uint64)(i/*noc_gp_hdr(p_message_trans_ext, m_dst_core)*/+1));
				
				switch (up_layer->upstream_nb_transport_fw(input_ch_no, *p_message_trans, phase, t)) {
				case tlm::TLM_COMPLETED:
				case tlm::TLM_UPDATED:
					p_message_trans->release();
					break;
				case tlm::TLM_ACCEPTED:
					UNDER_CONSTRUCTION();
					break;
				default:
					std::cout << "Error: '" << name() << "': Illegal phase received from initiator." << std::endl;
					assert(false); exit(1);
				}
				return;
			} else {
				std::cout << "Error: '" << name() << "'core is not FREE " << std::endl;
				assert(false); exit(1);
			}
			std::cout << "Error: '" << name() << "': Task Activation Error." << std::endl;
			assert(false); exit(1);
		} else if  (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_ACTIVE_ACK  
			     || noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_CMD_START
				 || noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::WRITE_DATA) {
			trafficLogFile << name() << " Received TASK_ACTIVE_ACK" << std::endl;

			
			//p_message_trans->set_address((sc_dt::uint64)RTR_CORE_0_INTERFACE_ADD);
			p_message_trans->set_address((sc_dt::uint64)(noc_gp_hdr(p_message_trans_ext, m_dst_core)+1));

			switch (up_layer->upstream_nb_transport_fw(input_ch_no, *p_message_trans, phase, t)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
				if (phase == tlm::END_REQ) {
					p_message_trans->release();
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
		} else if (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_RLS_REQ) {
            tlm_noc::noc_message_payload* p_message_trans_ext;
            p_message_trans->get_extension(p_message_trans_ext);
            noc_gp_hdr(p_message_trans_ext, m_message_id) = 10;
		
#ifdef DEBUG
			outputLogFile << name() << " Reconfiguration "<< std::endl;
			trafficLogFile << name() << " Reconfiguration "<< std::endl;
#endif
			ReconfigurationController(p_message_trans, 0); // 0: for release a mapped task
            
            sc_core::sc_time           delay(sc_core::SC_ZERO_TIME);
			if (noc_gp_hdr(p_message_trans_ext, m_req_task)->get_id() == 0) {
				noc_gp_hdr(p_message_trans_ext, m_cmd) = tlm_noc::APPLICATION_MAPPED_FAILED;
				m_external_port->nb_transport_bw(tile_id, *p_message_trans, phase, delay);
			} else
	            m_ni_manager->notify((MANAGER_TYPE_CAST*)this, *p_message_trans, phase, t, input_ch_no, m_action_label[TRANSPORT_DOWN_STREAM_FW]);
		}
	}


    virtual void function_16(transaction_type* p_message_trans, unsigned int index) {
#ifdef DEBUG
		trafficLogFile << name() << " Reconfiguration Release PORT = " << index << std::endl;
#endif
		if (reconfigurable_area[index].activity == RESERVED)
			reconfigurable_area[index].activity = FREE;
	}



	bool CheckCRC(tlm_noc::noc_packet_payload* p_packet_trans_ext) {
		return true;
	}
	void ReconfigurationController(transaction_type* p_message_trans, int action) {
		tlm_noc::noc_message_payload* p_message_trans_ext;
		p_message_trans->get_extension(p_message_trans_ext);
		int cost = 0;
		NoCTask* tsk = noc_gp_hdr(p_message_trans_ext, m_req_task);
		if (action == 1) {
			phase_type phase;
			tlm_noc::Volunteer* vl;
			vl = new tlm_noc::Volunteer;
			vl->next = NULL;
			sc_core::sc_time t = sc_core::sc_time(RECONFIGURATION_RELEASE_WAIT, sc_core::SC_NS);
			
			Reconfiguration(tsk, vl);
			if (vl->core_id != -1) { 
				vl->tile_id = tile_id;
			} else {
				vl->tile_id = tile_id;
				vl->tile_status = tlm_noc::TILE_BUSY;
			}
			if (noc_gp_hdr(p_message_trans_ext, m_volunteer) != NULL) 
				vl->next = noc_gp_hdr(p_message_trans_ext, m_volunteer);
			noc_gp_hdr(p_message_trans_ext, m_volunteer) = vl;
		} else if (action == 0) {
			for (int i = 0; i < NO_OF_RECONFIGURABLE_CORE; i++) 
				if ((reconfigurable_area[i].task == tsk) && reconfigurable_area[i].activity == ACTIVE) {
					reconfigurable_area[i].activity = FREE;
				}
			noc_gp_hdr(p_message_trans_ext, m_dst_tile) = noc_gp_hdr(p_message_trans_ext, m_src_tile);
			noc_gp_hdr(p_message_trans_ext, m_src_tile) = tile_id;
			noc_gp_hdr(p_message_trans_ext, m_dst_core) = noc_gp_hdr(p_message_trans_ext, m_src_core);
			noc_gp_hdr(p_message_trans_ext, m_cmd)      = tlm_noc::TASK_RLS_ACK;
		}
	}


	void Reconfiguration(NoCTask* tsk, tlm_noc::Volunteer* vl) {
		vl->core_id = -1;
		for (int i = 0; i < NO_OF_RECONFIGURABLE_CORE; i++) {
			if (reconfigurable_area[i].activity == FREE) {
				if (reconfigurable_area[i].task) {
					if ((reconfigurable_area[i].task->get_id() == tsk->get_id())) {
						vl->tile_id = tile_id;
						vl->tile_status = tlm_noc::TILE_READY;
						vl->core_id = i;
						return;
					}
				} else {
					vl->core_id = i;
					vl->tile_status = tlm_noc::TILE_FREE;
				}
			}
			trafficLogFile << name() << " reconfigurable_area[" << i<< "].activity = " << reconfigurable_area[i].activity << std::endl; 
		}
	}
	void save_packet(tlm_noc::noc_packet_payload* p_packet_trans_ext, unsigned int input_ch_no) {
		//std::cout << name() << " save_packet = " << input_ch_no << std::endl;

		if (noc_gp_hdr(p_packet_trans_ext, m_packet_id) == 0) {
			transaction_type* p_message_trans;
			p_message_trans = m_mm.allocate();
			p_message_trans->acquire();
			tlm_noc::noc_message_payload* p_message_trans_ext;

			p_message_trans_ext = new tlm_noc::noc_message_payload;
			p_message_trans->set_extension(p_message_trans_ext);

			p_message_trans_ext->noc_trans << p_packet_trans_ext->noc_trans;

			m_input_buffer[input_ch_no].push_back(p_message_trans);

		} else if (noc_gp_hdr(p_packet_trans_ext, m_packet) == tlm_noc::LAST_PACKET) {
			sc_core::sc_time t = sc_core::sc_time(INPUT_BUFFER, sc_core::SC_NS);
			phase_type phase ; ///initialization needed
			transaction_type* p_message_trans;
			p_message_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_message_payload* p_message_trans_ext;
			p_message_trans->get_extension(p_message_trans_ext);
			p_message_trans_ext->noc_trans << p_packet_trans_ext->noc_trans;
			if (m_input_buffer[input_ch_no].size() > 0) {
				m_ni_manager->notify((MANAGER_TYPE_CAST*)this, *p_message_trans, phase, t, input_ch_no, m_action_label[TRANSPORT_INPUT_BUFF]);
				m_input_buffer[input_ch_no].pop_back();
			}

		} else {
			transaction_type* p_message_trans;
			p_message_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_message_payload* p_message_trans_ext;
			p_message_trans->get_extension(p_message_trans_ext);
			p_message_trans_ext->noc_trans << p_packet_trans_ext->noc_trans;
		}
	}

	
private:

	bool find(tlm_noc::Volunteer* vl, int tid) {
	tlm_noc::Volunteer* vl_tmp = vl;
	while(vl_tmp != NULL) {
		if (vl_tmp->tile_id == tid)
			return 1;
		vl_tmp = vl_tmp->next;
	}
	return 0;
}


	std::list<transaction_type*>*                           m_input_buffer;
	enum TRANSPORT_NIPartEnum {TRANSPORT_DOWN_STREAM_FW,
							 TRANSPORT_DOWN_STREAM_BW,
							 TRANSPORT_UP_STREAM_FW,
							 TRANSPORT_UP_STREAM_BW,
							 TRANSPORT_MAIN_ACTION_WAIT,
							 TRANSPORT_WAIT_LIST_ACTIVE,
							 TRANSPORT_INPUT_BUFF,
							 TRANSPORT_INPUT_RELEASE
							};
	typedef TRANSPORT_NIPartEnum transport_ni_part_type;

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


	std::map<transport_ni_part_type, unsigned int>                m_action_label;
	//OLD variables

	struct PacketSignature {
		int session;
		int packet_id;
		friend bool operator<(const PacketSignature& lv, const PacketSignature& rv){
			return lv.session < rv.session;
		}
	};

	struct MessageSignature {
		int session;
		friend bool operator<(const MessageSignature& lv, const MessageSignature& rv){
			return lv.session < rv.session;
		}
	};

	sc_core::sc_time                                mTransportLayerDelay;


	
	typedef typename std::vector<MessageSignature>::iterator m_session_iterator;

	std::map<NoCTask*, int >	m_respond_task_session; //task, session



public:	
struct DynamicTask {
		NoCTask*        task;
		ActivityType    activity;
	};
	DynamicTask reconfigurable_area[NO_OF_RECONFIGURABLE_CORE];

	int tile_id;
	tlm_noc::NoRCMM       m_mm;
public:
	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>,  NR_OF_CHANNEL_CONST >*  m_external_port;

};



#endif /*ERROR_CONTROL_LAYER_H_*/
