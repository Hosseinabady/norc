/*
 * noc_mem.h
 *
 *  Created on: 29-May-2009
 *      Author: csxmh
 */

#ifndef NOC_MEM_H_
#define NOC_MEM_H_
#include "model/include/modelutilities.h"
#include <model/modules/include/noc_abstract_module.h>
#include "systemc"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

class LocalMemory : public NoCAbstractModule {

public:

	enum { SIZE = 1*MAX_RTR_MODULE };
	const sc_core::sc_time LATENCY;

	LocalMemory(sc_core::sc_module_name name, int i) : NoCAbstractModule(name, i),  LATENCY(1, sc_core::SC_NS) {

		mem = new int[SIZE];
	   //Clear memory
		for (int i = 0; i < SIZE; i++)
			mem[i] = 0x0;

	}

	virtual void nb_transport_bw( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay) {
		tlm::tlm_command cmd = trans.get_command();
		sc_dt::uint64    adr = trans.get_address() / 4;
		unsigned char*   ptr = trans.get_data_ptr();
		unsigned int     len = trans.get_data_length();
		unsigned char*   byt = trans.get_byte_enable_ptr();
		unsigned int     wid = trans.get_streaming_width();

		wait(delay);
		delay = sc_core::SC_ZERO_TIME;
		if (cmd == tlm::TLM_READ_COMMAND)
			memcpy(ptr, &mem[adr], len);
		else if (cmd == tlm::TLM_WRITE_COMMAND)
			memcpy(&mem[adr], ptr, len);

		trans.set_response_status(tlm::TLM_OK_RESPONSE);
	}

	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
		                        tlm::tlm_dmi& dmi_data) {
		return true;
	}

	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans) {
		return 0;
	}

	int* mem;
	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >*			   m_bus;

};

#endif /* NOC_MEM_H_ */
