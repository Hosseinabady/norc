#ifndef NoC_RTR_CORE_H_
#define NoC_RTR_CORE_H_

#include <tlm.h>
#include "model/modules/include/noc_abstract_module.h"
#include "NoRC/utilities/include/utilities.h"
#include "tlm_utils/peq_with_get.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "model/tasks/include/noc_task.h"
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
class NoCRTRCore : public NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
											  NR_OF_OUTPUT_CHANNEL,
								              INPUT_CHANNEL_CAPACITY,
								              OUTPUT_CHANNEL_CAPACITY,
								              TLM_NOC_INPUT_PAYLOAD,
								              TLM_NOC_OUTPUT_PAYLOAD,
								              TLM_GENERIC_PAYLOAD,
								              TLM_PHASE,
								              TLM_SYNC_ENUM> {
public:
//	typedef    tlm::tlm_generic_payload		 transaction_type;
//	typedef    tlm::tlm_phase                phase_type;
//	typedef    tlm::tlm_sync_enum            sync_enum_type;


public:


	SC_HAS_PROCESS(NoCRTRCore);

	NoCRTRCore(sc_core::sc_module_name name, int id) : NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>(name, id),
													   m_target_layer_delay(1, sc_core::SC_NS),
													   m_peq_receive_request_message_trans("PEQReceiveRequestMessageTrans")
	{
		free = true;
		SC_METHOD(RTRInitiator_method);
		task = NULL;
	};
	~NoCRTRCore() {};
	//virtual sync_enum_type nb_transport_fw(transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) = 0;
	//virtual sync_enum_type nb_transport_bw(transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) = 0;

	virtual void RTRInitiator_method() = 0;

	sc_core::sc_time           							m_target_layer_delay;
	tlm_utils::peq_with_get<tlm::tlm_generic_payload> 	m_peq_receive_request_message_trans;
	void setTask(NoCTask* t) {
		task = t;
	}
	void setNext_task(NoCTask* nt) {
		next_task = nt;
	}

	void configure(NoCTask* t) {
		setTask(t);
		free = false;
	}
	void release(NoCTask* t) {
			setTask(t);
			free = false;
	}
	bool isActive() {
		return !free;
	}
	NoCTask* getTask() {
		return task;
	}

	NoCTask* getNext_task() {
		return next_task;
	}
private:
	NoCTask*  task;
	bool      free;
	NoCTask*  next_task;


};



#endif /*NoC_RTR_CORE_H_*/

