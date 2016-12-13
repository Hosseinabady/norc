/*
 * controller.h
 *
 *  Created on: 29-May-2009
 *      Author: csxmh
 */

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <model/tasks/include/noc_task.h>
#include <list>
#include "tlm.h"
#include <model/include/modelutilities.h>
#include "NoRC/utilities/include/utilities.h"


struct ActiveTask {
	NoCTask*      task;
	bool          mapped;
	int           tile_id;
	int           core_id;
	int           successor_task_id;
	int           predecessor_task_id;

	ActiveTask() {
		task = NULL;
		mapped = false;
		tile_id = -1;
		core_id = -1;
	}
};

extern int tile_task_spectrum[NO_OF_ROW_CONST][NO_OF_COL_CONST];
class TaskManager : public NoCTask {

public:
	TaskManager() {
		state = START; 
		uncomplete = 0;
		retry = 0; 
	}
typedef enum {START, IDLE, WAIT_TASK_ACK, WAIT_TASK_MAP}  controller_state;
	~TaskManager();
	virtual	int action(tlm::tlm_generic_payload* p_trans);
private:
	void read_task_graph(char* filename);
	ActiveTask* find_earliest_deadline_task();
	bool SelectActiveVolunteer(tlm::tlm_generic_payload* p_trans);
//------------------------------------
// Variables
//------------------------------------
private:
	std::list<ActiveTask*> task_graph;
	controller_state state;
	int uncomplete;
	int retry;
	
};

#endif /* CONTROLLER_H_ */
