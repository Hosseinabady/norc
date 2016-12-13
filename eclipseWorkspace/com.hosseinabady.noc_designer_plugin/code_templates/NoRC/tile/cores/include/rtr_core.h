#ifndef RTR_CORE_H_
#define RTR_CORE_H_

#include <tlm.h>
#include <systemc>
#include "model/modules/include/noc_rtr_core.h"
#include "NoRC/protocol_data_units/include/noc_payload.h"
#include "tlm_utils/peq_with_get.h"
#include "NoRC/tile/network_interface/include/interface.h"
#include "NoRC/utilities/include/utilities.h"
#include "model/modules/include/noc_ambaahb_bus.h"
#include "model/model.h"
#include "model/modules/include/noc_connector.h"
#include "NoRC/utilities/include/utilities.h"


template <int NR_OF_INPUT_CHANNEL      = 1,
		  int NR_OF_OUTPUT_CHANNEL     = 1,
		  int INPUT_CHANNEL_CAPACITY   = 1,
		  int OUTPUT_CHANNEL_CAPACITY  = 1,
		  class TLM_NOC_INPUT_PAYLOAD  = tlm_noc::noc_phit_payload,
		  class TLM_NOC_OUTPUT_PAYLOAD = tlm_noc::noc_phit_payload,
		  class TLM_GENERIC_PAYLOAD    = tlm::tlm_generic_payload,
		  class TLM_PHASE              = tlm::tlm_phase,
		  class TLM_SYNC_ENUM          = tlm::tlm_sync_enum>
class RTRCore : public NoCRTRCore<NR_OF_INPUT_CHANNEL,
											  NR_OF_OUTPUT_CHANNEL,
								              INPUT_CHANNEL_CAPACITY,
								              OUTPUT_CHANNEL_CAPACITY,
								              TLM_NOC_INPUT_PAYLOAD,
								              TLM_NOC_OUTPUT_PAYLOAD,
								              TLM_GENERIC_PAYLOAD,
								              TLM_PHASE,
								              TLM_SYNC_ENUM> {
public:
	SC_HAS_PROCESS(RTRCore);

	RTRCore(sc_core::sc_module_name name, int id) : NoCRTRCore<NOCPROTOCOLLAYER_PARAM>(name, id)
		                                           ,m_peq_receive_request_message_trans("m_peq_receive_request_message_trans")
	                                               ,mRTRCoreDelay(1, sc_core::SC_NS)
	{
		SC_METHOD(RTRTaskMapping);
		sensitive(m_peq_receive_request_message_trans.get_event());
	}
	~RTRCore() {}
	void RTRInitiator_method() {

	sc_core::sc_time delay;
	tlm::tlm_phase phase;
	tlm::tlm_generic_payload* p_trans;
	p_trans = m_mm.allocate();
	p_trans->acquire();

	tlm_noc::noc_message_payload* p_message_trans_ext;
	p_message_trans_ext = new tlm_noc::noc_message_payload;



	p_trans->set_extension(p_message_trans_ext);

	noc_gp_hdr(p_message_trans_ext, m_src_core) = getId();
	noc_gp_hdr(p_message_trans_ext, m_volunteer) = NULL;

	delay = sc_core::SC_ZERO_TIME;
	phase = tlm::BEGIN_REQ;

	if (isActive() == true) { // the core has an active task
		if (getTask()->action(p_trans) != 0) {
			switch (m_bus->nb_transport_fw(0, *p_trans, phase, delay)) {
			case tlm::TLM_COMPLETED:
			case tlm::TLM_UPDATED:
	    		if (phase == tlm::END_REQ) {

					//p_trans->release();
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


		} else {
			p_trans->release();
		}
	} else {
		p_trans->release();
	}
	}


	void RTRTaskMapping() {
#ifdef DEBUG
		trafficLogFile << name() << " RTRTaskMapping " << std::endl;
#endif
		phase_type phase;
		sc_core::sc_time delay;

		transaction_type* p_message_trans;
		while ((p_message_trans = m_peq_receive_request_message_trans.get_next_transaction()) != 0) {
			tlm_noc::noc_message_payload* p_message_trans_ext;
			p_message_trans->get_extension(p_message_trans_ext);
			trafficLogFile << name() << " task m_cmd " << noc_gp_hdr(p_message_trans_ext, m_cmd) << " Task id = " << noc_gp_hdr(p_message_trans_ext, m_req_task)->get_id() << std::endl;
			if  (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_ACTIVE_REQ) {
				trafficLogFile << name() << " TASK MAPPING REQUEST task m_cmd " << noc_gp_hdr(p_message_trans_ext, m_cmd) << " Task id = " << noc_gp_hdr(p_message_trans_ext, m_req_task)->get_id() << std::endl;
				setTask( noc_gp_hdr(p_message_trans_ext, m_req_task));
				if (getTask()->get_id() != 0) {
					noc_gp_hdr(p_message_trans_ext, m_dst_tile) = noc_gp_hdr(p_message_trans_ext, m_src_tile);
					noc_gp_hdr(p_message_trans_ext, m_cmd) = tlm_noc::TASK_ACTIVE_ACK;
					noc_gp_hdr(p_message_trans_ext, m_cast) = tlm_noc::UNI_CAST;
					noc_gp_hdr(p_message_trans_ext, m_dst_core) = noc_gp_hdr(p_message_trans_ext, m_src_core);
					noc_gp_hdr(p_message_trans_ext, m_src_core) = getId();
					p_message_trans->set_address((sc_dt::uint64)NETWORK_INTERFACE_ADD);
					switch (m_bus->nb_transport_fw(0, *p_message_trans, phase, delay)) {
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
					return;
				}
			} else {
				trafficLogFile << name() << " task m_cmd " << noc_gp_hdr(p_message_trans_ext, m_cmd) << std::endl;
			}

			trafficLogFile << name() << " task id " << getTask()->get_id() << std::endl;
			if (getTask()->get_id() == 0 ||
				noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_CMD_START ||
				noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::WRITE_DATA ) {
				if (getTask()->action(p_message_trans) != NULL) {
					if (p_message_trans != NULL) {
						noc_gp_hdr(p_message_trans_ext, m_src_core) = getId();
						noc_gp_hdr(p_message_trans_ext, m_volunteer) = NULL;
						delay = sc_core::SC_ZERO_TIME;
						phase = tlm::BEGIN_REQ;
						switch (m_bus->nb_transport_fw(0, *p_message_trans, phase, delay)) {
						case tlm::TLM_COMPLETED:
						case tlm::TLM_UPDATED:
		    				if (phase == tlm::END_REQ) {
								//p_message_trans->release();
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
						if (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::APPLICATION_MAPPED) {
							int number_of_data = 0;
							delay = sc_core::sc_time(10, sc_core::SC_NS);
							while ( number_of_data < 2) {
								tlm::tlm_generic_payload* p_message_trans;
								p_message_trans = m_mm.allocate();
								p_message_trans->acquire();

								tlm_noc::noc_message_payload* p_message_trans_ext;
								p_message_trans_ext = new tlm_noc::noc_message_payload;

								p_message_trans->set_extension(p_message_trans_ext);
								noc_gp_hdr(p_message_trans_ext, m_cmd) = tlm_noc::APPLICATION_MAPPED;
								if (getTask()->action(p_message_trans) != NULL) {
									if (p_message_trans != NULL) {
										noc_gp_hdr(p_message_trans_ext, m_src_core) = getId();
										noc_gp_hdr(p_message_trans_ext, m_volunteer) = NULL;
										delay = 2*delay;
										phase = tlm::BEGIN_REQ;
										switch (m_bus->nb_transport_fw(0, *p_message_trans, phase, delay)) {
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
									}
								}
								number_of_data++;
							}
						}
					}
				}
			}
		}
	}
	sync_enum_type nb_transport_fw(int input_ch, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {
		#ifdef DEBUG
		trafficLogFile << name() << " nb_transport_fw PORT = " << input_ch << std::endl;
#endif

	    r_trans.acquire();

		if (phase == tlm::BEGIN_REQ) {
			m_peq_receive_request_message_trans.notify(r_trans, mRTRCoreDelay);

			phase = tlm::END_REQ;
			t = sc_core::SC_ZERO_TIME;
			r_trans.set_response_status(tlm::TLM_OK_RESPONSE);
			return tlm::TLM_UPDATED;
		} else if (phase == tlm::END_RESP) {

		} else {
			assert(0); exit(1);
		}

		return tlm::TLM_ACCEPTED;
	}
	sync_enum_type nb_transport_bw(int input_ch, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {
		return tlm::TLM_ACCEPTED;
	}

	tlm_utils::peq_with_get<transaction_type>  m_peq_receive_request_message_trans;
	sc_core::sc_time                           mRTRCoreDelay;

	tlm_noc::NoRCMM                            m_mm;


	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >*                          m_bus;

};



#endif /*RTR_CORE_H_*/
