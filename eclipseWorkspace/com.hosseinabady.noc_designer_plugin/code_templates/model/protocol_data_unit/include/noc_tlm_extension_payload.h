#ifndef NOC_TLM_EXTENTION_PAYLOAD_H_
#define NOC_TLM_EXTENTION_PAYLOAD_H_

#include "tlm.h"
#include <cassert>
#include <model/model.h>

#include "model/protocol_data_unit/include/noc_generic_payload.h"
//#include "utilities.h"

namespace tlm_noc {

template <typename HEADER, typename BUDDY, int size>
class noc_tlm_extension_payload: public tlm::tlm_extension<noc_tlm_extension_payload<HEADER, BUDDY, size> >
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type    tlm_payload_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type       tlm_phase_type;

	noc_tlm_extension_payload() {

	}

	tlm::tlm_extension_base* clone() const {
		noc_tlm_extension_payload<HEADER, BUDDY, size>* t = new noc_tlm_extension_payload<HEADER, BUDDY, size>;
	    t->noc_trans            = this->noc_trans;

	    return t;
	}
	void free () {
		delete this;
	}

	void copy_from(tlm::tlm_extension_base const& e) {
//		assert(typeid(this) == typeid(e));
		noc_trans = static_cast<noc_tlm_extension_payload const &>(e).noc_trans;
	}



	tlm_noc::noc_generic_payload<HEADER, BUDDY, size>  noc_trans;
//	tlm_noc::NoRCGenericMM<noc_tlm_extension_payload<HEADER, BUDDY, size> >*              m_mm;
    unsigned int                   m_ref_count;
};
}
#endif /*NOC_TLM_EXTENTION_PAYLOAD_H_*/
