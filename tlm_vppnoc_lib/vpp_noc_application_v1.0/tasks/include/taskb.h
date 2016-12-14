/*
 * taskb.h
 *
 *  Created on: 03-Jun-2009
 *      Author: csxmh
 */

#ifndef TASKB_H_
#define TASKB_H_

#include <model/tasks/include/noc_task.h>
#include <list>
#include "tlm.h"

class TaskB : public NoCTask {

public:
	TaskB() { };

	virtual	int action(tlm::tlm_generic_payload* p_trans);


private:

};
#endif /* TASKB_H_ */
