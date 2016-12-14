#pragma once
#ifndef ERROR_CONTROL_LAYER_H_
#define ERROR_CONTROL_LAYER_H_

#include <list>
#include <systemc>
#include "vppnoc_core.h"
#include "include/nocapp_utilities.h"
#include "tasks/include/taskmanager.h"
#include "protocol_data_unit/nocapp_message_payload.h"
using namespace std;
using namespace sc_core;


extern std::ofstream outputLogFile;
extern std::ofstream taskMappingPatternLogFile;
extern std::ofstream resultOutputFile;

namespace nocapp {


class TransportLayer : 
	public vppnoc::NoCProtocolLayer {

	
	
public:
	friend class downstream_fw;
	
	class receiver_process2 : public vppnoc::vppnoc_process<TransportLayer, vppnoc::vppnoc_transaction> {
	public:
		receiver_process2( vppnoc::VPPNOCMicroScheduler<TransportLayer>**	ma = 0,  TransportLayer* const ownr = 0) : vppnoc::vppnoc_process<TransportLayer, vppnoc::vppnoc_transaction>(ma, ownr) {}
		virtual int operator()(int socket_id, vppnoc::vppnoc_transaction* p_trans, sc_core::sc_time & t) {
			std::cout << " receiver_process2 " << std::endl;
			wait(0, p_trans, t, owner->m_downstream_fw);
			return 1; 
		}
	};



	TransportLayer(sc_core::sc_module_name name,
		           int id,
				   vppnoc::TileTypeEnum ty) :
		vppnoc::NoCProtocolLayer(name, id),
		mTransportLayerDelay(1, sc_core::SC_NS),
		tile_id(id)
	{
		m_ni_manager = new (vppnoc::VPPNOCMicroScheduler<TransportLayer>)("TransportLayerNoCNIManager", id);

		m_downstream_fw = new downstream_fw(&m_ni_manager, this);
		m_receiver2 = new receiver_process2(&m_ni_manager, this);

		for (int i = 0; i < NO_OF_RECONFIGURABLE_CORE; i++) {
			reconfigurable_area[i].task = NULL;
			reconfigurable_area[i].activity = CORE_FREE;
		}


		m_input_buffer  = new std::list<vppnoc::vppnoc_transaction*>[NR_OF_INPUT_CHANNEL_CONST];

	}


	~TransportLayer() {
		delete[] m_ni_manager;
		delete[] m_input_buffer;

	}

	//*********************************************************************************
	// 					Interfaces for receiving messages or packets
	//*********************************************************************************
	vppnoc::vppnoc_sync_enum_type  nb_transport_fw(int socket_id, vppnoc::vppnoc_transaction* p_message_trans, sc_core::sc_time & t) {
#ifdef DEBUG
		trafficLogFile << name() << " downstream_nb_transport_fw PORT = " << initiator_id << std::endl;
#endif
		p_message_trans->acquire();
		m_ni_manager->notify(socket_id, p_message_trans, t, m_downstream_fw);
		std::cout << " I am nb transport fw of transport layer" << std::endl;
		return vppnoc::VPPNOC_ACCEPTED;
	}

	virtual vppnoc::vppnoc_sync_enum_type downstream_nb_transport_bw(int initiator_id, vppnoc::vppnoc_transaction& r_response_message_trans, vppnoc::vppnoc_phase& phase, sc_core::sc_time& t) {


		return vppnoc::VPPNOC_ACCEPTED;
	}


	virtual vppnoc::vppnoc_sync_enum_type upstream_nb_transport_fw(int initiator_id, vppnoc::vppnoc_transaction& r_request_packet_trans, vppnoc::vppnoc_phase& phase, sc_core::sc_time& t) {

#ifdef DEBUG
		fatal_error_log << name() << " upstream_nb_transport_fw PORT = " << initiator_id; //  << std::endl;

		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		r_request_packet_trans.get_extension(p_packet_trans_ext);
		fatal_error_log << " m_message_id = " << noc_gp_hdr(p_packet_trans_ext, m_message_id)
			           << " m_packet_id  = " << noc_gp_hdr(p_packet_trans_ext, m_packet_id)
					   << " " << p_packet_trans_ext << " "  << &r_request_packet_trans
					   << std::endl;
#endif		

//		r_request_packet_trans.acquire();

		if (phase == vppnoc::BEGIN_REQ) {
//			m_ni_manager->notify((TransportLayer*)this, initiator_id, r_request_packet_trans, t, (int*)NULL, initiator_id, m_action_label[TRANSPORT_UP_STREAM_FW]);
			phase = vppnoc::END_REQ;
			t = sc_core::SC_ZERO_TIME;
//			r_request_packet_trans.set_response_status(vppnoc::VPP_NOC_OK_RESPONSE);
		//	r_request_packet_trans.release();
			return vppnoc::VPPNOC_UPDATED;
		} else if (phase == vppnoc::END_RESP) {

		} else {
			assert(0); exit(1);
		}

	    return vppnoc::VPPNOC_COMPLETED;  //unreachable code
	}

class downstream_fw : public vppnoc::vppnoc_process<TransportLayer, vppnoc::vppnoc_transaction> {
	public:
		downstream_fw( vppnoc::VPPNOCMicroScheduler<TransportLayer>**	ma, TransportLayer* ownr) : 
		               vppnoc::vppnoc_process<TransportLayer, vppnoc::vppnoc_transaction>(ma, ownr) {}
		virtual int operator()(int socket_id, vppnoc::vppnoc_transaction* p_message_trans, sc_core::sc_time & r_delay) {
			std::cout << " downstream_fw " << std::endl;
			wait(0, p_message_trans, r_delay, owner->m_receiver2);
			
			p_message_trans->acquire();

			nocapp::MessageCmd tmp = headerfield(nocapp::nocapp_message_payload, p_message_trans, m_cmd).get_field();
			if (headerfield(nocapp::nocapp_message_payload, p_message_trans, m_cmd).get_field() == nocapp::APPLICATION_MAPPED) {
//				m_external_port->nb_transport_bw(tile_id, *p_trans, phase, delay);
//				return 1;
			}

			int next_tile       = -1;
			int previous_tile   = -1;
			int port_selects[4] = {0};
			int tile_id = owner->get_tile_id();
			nocapp::Volunteer* vl = headerfield(nocapp::nocapp_message_payload, p_message_trans, m_volunteer);
			int rand_route;
			while(next_tile == -1 && !(port_selects[0]*port_selects[1]*port_selects[2]*port_selects[3])) {
				rand_route = rand()%4;
				port_selects[rand_route] = 1;
				switch (rand_route) {
				case 0:   //RIGHT
					if (tile_id+1 < NO_OF_ROW_CONST*NO_OF_COL_CONST && !find_volunteer(vl, tile_id+1) && (tile_id % NO_OF_COL_CONST) )
						next_tile = tile_id+1;
					break;
				case 1: // UP
					if (tile_id-NO_OF_COL_CONST > 0 && !find_volunteer(vl, tile_id-NO_OF_COL_CONST) )
						next_tile = tile_id-NO_OF_COL_CONST;
					break;
				case 2: // LEFT
					if (tile_id-1 > 0 && !find_volunteer(vl, tile_id-1) && ((tile_id%NO_OF_COL_CONST) - 1))
						next_tile = tile_id-1;
					break;
				case 3: // DOWN
					if (tile_id+NO_OF_COL_CONST < NO_OF_ROW_CONST*NO_OF_COL_CONST && !find_volunteer(vl, tile_id+NO_OF_COL_CONST))
						next_tile = tile_id+NO_OF_COL_CONST;
					break;
				}
			}
			if (next_tile == -1) {
				std::cout << "===================Loop Detected Return===================" << std::endl;
//				next_tile = vl->tile_id;
			}
			unsigned int packet_id = 0;
		
			do {
				vppnoc::vppnoc_transaction* p_packet_trans;
				
				p_packet_trans = owner->m_mm.allocate();

				(*(nocapp::nocapp_message_payload*)p_message_trans) >> *(nocapp::nocapp_packet_payload*)p_packet_trans;

				headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_packet_id).set_field(packet_id);
				unsigned int message_id = headerfield(nocapp::nocapp_message_payload, p_message_trans, m_message_id).get_field();;
				headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_packet_id).set_field(message_id);
				headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_crc).set_field(0);
				headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_cmd).set_field(REQ_PACKET);
				unsigned int src_core = headerfield(nocapp::nocapp_message_payload, p_message_trans, m_src_core).get_field();
				headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_src_core).set_field(src_core);
				unsigned int dst_tile = headerfield(nocapp::nocapp_message_payload, p_message_trans, m_dst_tile).get_field();
				headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_dst_tile).set_field(dst_tile);
				unsigned int src_tile = headerfield(nocapp::nocapp_message_payload, p_message_trans, m_src_tile).get_field();
				headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_src_tile).set_field(src_tile);
				CastingType cast     = headerfield(nocapp::nocapp_message_payload, p_message_trans, m_cast).get_field();
				headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_cast).set_field(cast);
				if (cast == nocapp::UNI_CAST && 
				   (cast == nocapp::ACK_SEEK || 
				    cast == nocapp::TASK_ACTIVE_REQ || 
					cast == nocapp::TASK_ACTIVE_ACK ||
					cast == nocapp::TASK_CMD_START  ||
					cast == nocapp::WRITE_DATA) ||
					cast == nocapp::TASK_RLS_REQ || 
					cast == nocapp::TASK_RLS_ACK ) 
				{
					headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_routing).set_field(nocapp::XY);
				}
				MessageCmd msg_cmd = headerfield(nocapp::nocapp_message_payload, p_message_trans, m_cmd).get_field();
				if (cast == nocapp::BRD_CAST && msg_cmd == nocapp::TASK_SEEK) {
					headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_routing).set_field(nocapp::XY);//DIR_FLOODING;//PUR_FLOODING;
					headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_dst_tile).set_field(next_tile);
				}
				if (packet_id == 0) {
					headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_packet).set_field(nocapp::FIRST_PACKET);
				} else if (((nocapp::nocapp_message_payload*)p_message_trans)->stream_tail == 0) {
					headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_packet).set_field(nocapp::LAST_PACKET);
				} else {
					headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_packet).set_field(nocapp::MIDDLE_PACKET);
				}


//				PacketSignature key ={noc_gp_hdr(p_packet_trans_ext,m_src_core), packet_id};

				sc_core::sc_time delay = sc_core::sc_time(TRANSPORT_TO_NETWORK_DELAY, sc_core::SC_NS);
				int out_channel = headerfield(nocapp::nocapp_packet_payload, p_packet_trans, m_src_core).get_field();
				switch (owner->m_outPort.nb_transport(out_channel, p_packet_trans, delay)) {
				case vppnoc::VPPNOC_COMPLETED:
				case vppnoc::VPPNOC_UPDATED:
					break;
				case vppnoc::VPPNOC_ACCEPTED:
					break;
				default:
					std::cout << "Error: '" << owner->name() << "': Illegal phase received from initiator." << std::endl;
					assert(false); exit(1);
				}
				packet_id++;
			} while (/*p_message_trans_ext->stream_tail != */0);
//			p_trans->release();
			return 1; 
		}
	private: //Functions
		bool find_volunteer(nocapp::Volunteer* vl, int tid) {
			nocapp::Volunteer* vl_tmp = vl;
			for (int i = 0; i <TASK_SEEK_LIFE_TIME; i++ ) {
				if (vl_tmp->tile_id.get_field() == tid)
					return 1;
				vl_tmp = vl_tmp++;
			}
			return 0;
		}
	};




/*
	
	//upstreamfw_cb
	virtual void function_13(vppnoc::transaction_type* p_packet_trans, int* signal, int value, int channel_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_13 PORT = " << channel_no << std::endl;
#endif
	//	p_packet_trans->acquire();
		int input_ch_no = channel_no;
		tlm_noc::noc_packet_payload* p_packet_trans_ext;
		p_packet_trans->get_extension(p_packet_trans_ext);
		save_packet(p_packet_trans_ext, input_ch_no);
		p_packet_trans->release();
	}



	virtual void function_15(vppnoc::transaction_type* p_message_trans, int* signal, int value, int channel_no) {
#ifdef DEBUG
		trafficLogFile << name() << " function_15 PORT = " << channel_no << std::endl;
#endif
	//	p_message_trans->acquire();
		int input_ch_no = channel_no;
		sc_core::sc_time t = sc_core::sc_time(TRANSPORT_LAYER_UPSTREAM_INPUT_BUFFER_DELAY, sc_core::SC_NS);
        phase_type phase = tlm::BEGIN_REQ ; ///initialization needed
        tlm_noc::noc_message_payload* p_message_trans_ext;
        p_message_trans->get_extension(p_message_trans_ext);
	

		
		if (noc_gp_hdr(p_message_trans_ext, m_cmd) == -842150451)
			resultOutputFile << " ****************noc_gp_hdr(p_message_trans_ext, m_cmd) = " << noc_gp_hdr(p_message_trans_ext, m_cmd);

        if (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_SEEK) {
            tlm_noc::noc_message_payload* p_message_trans_ext;
            p_message_trans->get_extension(p_message_trans_ext);
            noc_gp_hdr(p_message_trans_ext, m_message_id) = 20;
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
			outputLogFile << name() << "  Reconfiguration "<< std::endl;
			trafficLogFile << name() << " Reconfiguration "<< std::endl;
#endif
			
			ReconfigurationController(p_message_trans, 1); // 1: for mapping a task
//            noc_gp_hdr(p_message_trans_ext, m_src_core) = RETURN_CHANNEL_NUMBER;
            sc_core::sc_time           delay(sc_core::SC_ZERO_TIME);
            m_ni_manager->notify((TransportLayer*)this, input_ch_no, *p_message_trans, t, (int*)NULL, input_ch_no, m_action_label[TRANSPORT_DOWN_STREAM_FW]);
			p_message_trans->release();
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
			//trafficLogFile << name() << " TASK ACTIVATION AGAIN = "  << i << " ACTIVITY = " << reconfigurable_area[i].activity << std::endl;
			if (reconfigurable_area[i].activity == CORE_FREE) {
#ifdef TASK_MAPPING_PATTERN_LOG
				taskMappingPatternLogFile << "        <Task  Id=\"" <<  noc_gp_hdr(p_message_trans_ext, m_req_task)->getTaskId() << "\">" << std::endl;
				taskMappingPatternLogFile << "            <Location CoreId=\"" << i << "\"  TileId=\"" << getId() << "\" Status=\"" << "mapped" << "\"/>" << std::endl;
				taskMappingPatternLogFile << "            <SimulationTime Value=\"" <<  sc_core::sc_time_stamp().value() << "\"/>" << std::endl;
				taskMappingPatternLogFile << "        </Task>" <<  std::endl;
#endif
				reconfigurable_area[i].activity = CORE_ACTIVE;
				reconfigurable_area[i].task     = noc_gp_hdr(p_message_trans_ext, m_req_task);
	

				//trafficLogFile << name() << " TASK ACTIVATION AGAIN m_dst_core= " << noc_gp_hdr(p_message_trans_ext, m_dst_core) << std::endl;
				p_message_trans->set_address((sc_dt::uint64)(i+1));
				
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
			//trafficLogFile << name() << " Received TASK_ACTIVE_ACK" << std::endl;

			
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
			if (noc_gp_hdr(p_message_trans_ext, m_req_task)->getTaskType() == -1) { //task manager
#ifdef TASK_MAPPING_PATTERN_LOG
				taskMappingPatternLogFile << "    </Application>" << std::endl;
#endif
				noc_gp_hdr(p_message_trans_ext, m_cmd) = tlm_noc::APPLICATION_MAPPED_FAILED;
				m_external_port->nb_transport_bw(tile_id, *p_message_trans, phase, delay);
		//		p_message_trans->acquire();
			} else {
	            m_ni_manager->notify((TransportLayer*)this, input_ch_no, *p_message_trans, t, (int*)NULL, input_ch_no, m_action_label[TRANSPORT_DOWN_STREAM_FW]);
		//		p_message_trans->acquire();
			}
		}
	}


    virtual void function_16(vppnoc::transaction_type* p_message_trans, int* signal, int value, int channel_no) {
#ifdef DEBUG
		trafficLogFile << name() << " Reconfiguration Release PORT = " << channel_no << std::endl;
#endif
	//	p_message_trans->acquire();
		int index = channel_no;
		if (reconfigurable_area[index].activity == CORE_RESERVED)
			reconfigurable_area[index].activity = CORE_FREE;
		p_message_trans->release();
	}



	bool CheckCRC(nocapp::noc_packet_payload* p_packet_trans_ext) {
		return true;
	}
	void ReconfigurationController(vppnoc::transaction_type* p_message_trans, int action) {
		nocapp::noc_message_payload* p_message_trans_ext;
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
				if ((reconfigurable_area[i].task == tsk) && reconfigurable_area[i].activity == CORE_ACTIVE) {
					reconfigurable_area[i].activity = CORE_FREE;
#ifdef TASK_MAPPING_PATTERN_LOG
					taskMappingPatternLogFile << "        <Task  Id=\"" <<  noc_gp_hdr(p_message_trans_ext, m_req_task)->getTaskId() << "\">" << std::endl;
					taskMappingPatternLogFile << "            <Location CoreId=\"" << i << "\"  TileId=\"" << getId() << "\" Status=\"" << "released" << "\"/>" << std::endl;
					taskMappingPatternLogFile << "            <SimulationTime Value=\"" <<  sc_core::sc_time_stamp().value() << "\"/>" << std::endl;
					taskMappingPatternLogFile << "        </Task>" <<  std::endl;
#endif
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
			if (reconfigurable_area[i].activity == CORE_FREE) {
				vl->core_id = i;
				vl->tile_status = tlm_noc::TILE_FREE;
				if (reconfigurable_area[i].task) {
					if ((reconfigurable_area[i].task->getTaskType() == tsk->getTaskType())) {
						vl->tile_id = tile_id;
						vl->tile_status = tlm_noc::TILE_READY;
						vl->core_id = i;
						return;
					}
				}
			}
			//trafficLogFile << name() << " reconfigurable_area[" << i<< "].activity = " << reconfigurable_area[i].activity << std::endl; 
		}
	}
	void save_packet(vppnoc::transaction_type* p_packet_trans_ext, unsigned int input_ch_no) {
		//std::cout << name() << " save_packet = " << input_ch_no << std::endl;
//#ifdef DEBUG
//		fatal_error_log << name() << " save_packet PORT = " << input_ch_no ;
//
		//fatal_error_log << " m_message_id = " << noc_gp_hdr(p_packet_trans_ext, m_message_id)
//			           << " m_packet_id  = " << noc_gp_hdr(p_packet_trans_ext, m_packet_id)
//					   << " " << p_packet_trans_ext 
//					   << std::endl;
//#endif		

		if (noc_gp_hdr(p_packet_trans_ext, m_packet_id) == 0) {
			transaction_type* p_message_trans;
			p_message_trans = m_mm.allocate();
		//	p_message_trans->acquire();
			tlm_noc::noc_message_payload* p_message_trans_ext;

			p_message_trans_ext = new tlm_noc::noc_message_payload;
			p_message_trans->set_extension(p_message_trans_ext);

			*p_message_trans_ext << *p_packet_trans_ext;

			m_input_buffer[input_ch_no].push_back(p_message_trans);

		} else if (noc_gp_hdr(p_packet_trans_ext, m_packet) == tlm_noc::LAST_PACKET) {
			sc_core::sc_time t = sc_core::sc_time(TRANSPORT_LAYER_UPSTREAM_INPUT_BUFFER_DELAY, sc_core::SC_NS);
			phase_type phase ; ///initialization needed
			transaction_type* p_message_trans;
			p_message_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_message_payload* p_message_trans_ext;
			p_message_trans->get_extension(p_message_trans_ext);
			*p_message_trans_ext << *p_packet_trans_ext;
			if (m_input_buffer[input_ch_no].size() > 0) {
				p_message_trans = m_input_buffer[input_ch_no].front();
				if (noc_gp_hdr(p_message_trans_ext, m_cmd) == -842150451)
					resultOutputFile << " ****************noc_gp_hdr(p_message_trans_ext, m_cmd) = " << noc_gp_hdr(p_message_trans_ext, m_cmd);
				m_ni_manager->notify((TransportLayer*)this, input_ch_no, *p_message_trans, t, (int* )NULL, input_ch_no, m_action_label[TRANSPORT_INPUT_BUFF]);
				m_input_buffer[input_ch_no].pop_front();
				p_message_trans->release();
			}

		} else {
			transaction_type* p_message_trans;
			p_message_trans = m_input_buffer[input_ch_no].back();
			tlm_noc::noc_message_payload* p_message_trans_ext;
			p_message_trans->get_extension(p_message_trans_ext);
			*p_message_trans_ext << *p_packet_trans_ext;
		}
	}
	*/
	
private:




	std::list<vppnoc::vppnoc_transaction*>*                           m_input_buffer;
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

	
	bool                                        m_buffer_used[NR_OF_TRANSPORT_LAYER_CHANNEL];



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


	
	typedef  std::vector<MessageSignature>::iterator m_session_iterator;

	std::map<NoCTask*, int >	m_respond_task_session; //task, session



public:	
struct DynamicTask {
		NoCTask*        task;
		ActivityType    activity;
	};
	DynamicTask reconfigurable_area[NO_OF_RECONFIGURABLE_CORE];

	int tile_id;


public: //Functions
	int get_tile_id() {
		return get_id();
	}



public:
	 /* Ports */
	vppnoc::vppnoc_master_socket<32>                                               m_outPort;
	vppnoc::VPPNOCMicroScheduler<TransportLayer, vppnoc::vppnoc_transaction>*	   m_ni_manager;
	/* Functors */
	vppnoc::vppnoc_process<TransportLayer, vppnoc::vppnoc_transaction>*            m_downstream_fw;
	vppnoc::vppnoc_process<TransportLayer, vppnoc::vppnoc_transaction>*            m_receiver2;
	vppnoc::vppnoc_mm<vppnoc_payload_base, nocapp::nocapp_packet_payload>                               m_mm;
};

}

#endif /*ERROR_CONTROL_LAYER_H_*/

