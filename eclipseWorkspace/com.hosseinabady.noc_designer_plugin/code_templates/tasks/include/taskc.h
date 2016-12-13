/*
 * taskc.h
 *
 *  Created on: 03-Jun-2009
 *      Author: csxmh
 */

#ifndef TASKC_H_
#define TASKC_H_

#include <model/tasks/include/noc_task.h>
#include <list>
#include "tlm.h"

class TaskC : public NoCTask {

public:
	TaskC() { };

	virtual	int action(tlm::tlm_generic_payload* p_trans);


private:

};


#endif /* TASKC_H_ */
