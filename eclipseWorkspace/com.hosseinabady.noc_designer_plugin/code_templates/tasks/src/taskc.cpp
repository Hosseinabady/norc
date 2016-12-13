/*
 * taskc.cpp
 *
 *  Created on: 03-Jun-2009
 *      Author: csxmh
 */

#include <tasks/include/taskc.h>

int TaskC::action(tlm::tlm_generic_payload* p_trans) {
	
	std::cout << "I'm TaskC " << sc_core::sc_time_stamp() << std::endl;
	return 0;
	
}
