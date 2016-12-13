#ifndef VPPNOC_PROCESS_H
#define VPPNOC_PROCESS_H
#include "models/vppnoc_microscheduler.h"
namespace vppnoc {


template <typename OWNER_TYPE = NoCProtocolLayer, typename TYPES=vppnoc_transaction>
class vppnoc_process {

	typedef  typename TYPES    payload_type;
  typedef int (OWNER_TYPE::*function_cb)(TYPES& trans, sc_core::sc_time & r_delay);
public:


	vppnoc_process( VPPNOCMicroScheduler<OWNER_TYPE, TYPES>**	ma=0, OWNER_TYPE* ownr=0, bool aht = true) : manager(ma), owner(ownr), ahead_of_time(aht) {}
	virtual int operator()(vppnoc_transaction& trans) {
	  return 1;
  }

  void set_ahead_of_time(bool aht) { ahead_of_time = aht;}
  bool get_ahead_of_time() { return ahead_of_time;}
  int wait(payload_type& trans, sc_core::sc_time& t, vppnoc_process<OWNER_TYPE, TYPES>* proc) {
		(*manager)->wait(trans, t, proc);
		return 1;
	}

  int wait(payload_type& trans, sc_core::sc_time& t, vppnoc_signal* s, vppnoc_signal_value sv, vppnoc_process<OWNER_TYPE, TYPES>* proc) {
		(*manager)->wait(trans, t, s, sv, proc);
		return 1;
	}
  
  int notify(payload_type& trans, sc_core::sc_time& when, function_cb func) {
		(*manager)->notify(trans, when, func);
		return 1;
	}
public:
	OWNER_TYPE*                                            owner;
	VPPNOCMicroScheduler<OWNER_TYPE, TYPES>**	             manager;
  bool                                                   ahead_of_time;
};


}

#endif //VPPNOC_PROCESS_H