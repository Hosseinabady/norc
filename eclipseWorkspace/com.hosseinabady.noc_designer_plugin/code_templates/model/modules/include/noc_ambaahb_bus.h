/*
 * ambaahb_bus.h
 *
 *  Created on: 29-May-2009
 *      Author: csxmh
 */

#ifndef NOC_AMBAAHB_BUS_H_
#define NOC_AMBAAHB_BUS_H_

#include <model/modules/include/noc_abstract_module.h>
#include "systemc"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "model/model.h"

template <int NR_OF_INPUT_CHANNEL      = 1,
		  int NR_OF_OUTPUT_CHANNEL     = 1,
		  int INPUT_CHANNEL_CAPACITY   = 1,
		  int OUTPUT_CHANNEL_CAPACITY  = 1,
		  class TLM_NOC_INPUT_PAYLOAD  = tlm_noc::noc_phit_payload,
		  class TLM_NOC_OUTPUT_PAYLOAD = tlm_noc::noc_phit_payload,
		  class TLM_GENERIC_PAYLOAD    = tlm::tlm_generic_payload,
		  class TLM_PHASE              = tlm::tlm_phase,
		  class TLM_SYNC_ENUM          = tlm::tlm_sync_enum>
class AMBAAHBBus : public NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
											  NR_OF_OUTPUT_CHANNEL,
								              INPUT_CHANNEL_CAPACITY,
								              OUTPUT_CHANNEL_CAPACITY,
								              TLM_NOC_INPUT_PAYLOAD,
								              TLM_NOC_OUTPUT_PAYLOAD,
								              TLM_GENERIC_PAYLOAD,
								              TLM_PHASE,
								              TLM_SYNC_ENUM> {

public:

	AMBAAHBBus(sc_core::sc_module_name name, int i) : NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>(name, i) {

	}

	tlm::tlm_sync_enum nb_transport_fw(int initiator_id, tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t) {
#ifdef DEBUG
		trafficLogFile << name() << " nb_transport_fw PORT = " << initiator_id << std::endl;
#endif
		sc_core::sc_time ldelay;
		tlm::tlm_phase   lphase;
		int module_port;

		trans.acquire();
		tlm_noc::noc_message_payload* p_message_trans_ext;
		trans.get_extension(p_message_trans_ext);

		sc_dt::uint64 address = trans.get_address();


		if (address == (sc_dt::uint64)NETWORK_INTERFACE_ADD) {
			module_port = MAX_RTR_MODULE+1;
		} else if (address == (sc_dt::uint64)RTR_CORE_0_INTERFACE_ADD) {
			module_port = 0;
		} else  if (address == (sc_dt::uint64)RTR_CORE_1_INTERFACE_ADD) {
			module_port = 1;
		} else if (address == (sc_dt::uint64)MEMORY_MODULE) {
			module_port = 2;
		}

		ldelay = sc_core::sc_time(AMBABUS_TO_TRANSPORT_DELAY, sc_core::SC_NS)+t;
		lphase = tlm::BEGIN_REQ;
		switch (m_core[module_port]->nb_transport_fw(0, trans, lphase, ldelay)) {
		case tlm::TLM_COMPLETED:
		case tlm::TLM_UPDATED:
	    	if (lphase == tlm::END_REQ) {
				phase = tlm::END_REQ; 
		   	   // trans->free_all_extensions();
		   	    //delete &trans;
				trans.release();
		   		return tlm::TLM_UPDATED;
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

		//trans.release();
		return tlm::TLM_ACCEPTED;
	}

	tlm::tlm_sync_enum nb_transport_bw(int initiator_id, tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t) {
		return tlm::TLM_ACCEPTED;
	}

	NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>*	                                      m_core[MAX_RTR_MODULE+2];
	

};

#endif /* NOC_AMBAAHB_BUS_H_ */
