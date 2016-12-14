/*
 * taska.h
 *
 *  Created on: 03-Jun-2009
 *      Author: csxmh
 */

#ifndef TASKA_H_
#define TASKA_H_

#include <model/tasks/include/noc_task.h>
#include <list>
#include "tlm.h"

class TaskA : public NoCTask {

public:
	TaskA() { };

	virtual	int action(tlm::tlm_generic_payload* p_trans);



};

#endif /* TASKA_H_ */
