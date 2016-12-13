#ifndef DUMMY_CORE_
#define DUMMY_CORE_


#include <tlm.h>
#include "model/modules/include/noc_rtr_core.h"
#include "NoRC/protocol_data_units/include/noc_payload.h"
#include "tlm_utils/peq_with_get.h"



class DummyCore : public sc_core::sc_module {
public:

	tlm_utils::simple_initiator_socket<DummyCore> initiator_socket;
	tlm_utils::simple_target_socket<DummyCore>    target_socket;



	SC_HAS_PROCESS(DummyCore);

	DummyCore(sc_core::sc_module_name name) {
		SC_THREAD(dummy_thread);
	}

	~DummyCore() {}
	void dummy_thread() {
		return;
	}

};


#endif /*DUMMY_CORE_*/
