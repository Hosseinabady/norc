/*
 * controller.h
 *
 *  Created on: 29-May-2009
 *      Author: csxmh
 */

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <list>
#include "vppnoc_core.h"
#include "include/nocapp_utilities.h"
#include "include/vppnoc_core.h"
#include "tasks/include/active_tasks.h"



//extern int tile_task_spectrum[NO_OF_ROW_CONST][NO_OF_COL_CONST];
class TaskManager : public NoCTask {

public:
	TaskManager() {
		state = START; 
		uncomplete = 0;
		retry = 0; 
	}
	sc_core::sc_time  getNominalPeriod() {return m_nominal_period;}
	void setNominalPeriod(sc_core::sc_time p) {m_nominal_period = p;}

	sc_core::sc_time  getMinimumPeriod() {return m_minimum_period;}
	void setMinimumPeriod(sc_core::sc_time p) { m_minimum_period = p;}

	sc_core::sc_time  getMaximumPeriod() {return m_maximum_period;}
	void setMaximumPeriod(sc_core::sc_time p) {m_maximum_period = p;}

	double getElasticCoefficient() {return m_elastic_coefficient;}
	void   setElasticCoefficient(double e) {m_elastic_coefficient = e;}

	void ReleaseACK(vppnoc::vppnoc_transaction* p_trans);
typedef enum {START, TASK_MAPPING, WAIT_TASK_ACK, WAIT_TASK_MAP}  controller_state;
	~TaskManager();
	virtual	int action(vppnoc::vppnoc_transaction* p_trans);
private:
	void read_task_graph(char* filename);
	ActiveTask* find_earliest_deadline_task();
	bool SelectActiveVolunteer(vppnoc::vppnoc_transaction* p_trans);
	int compute_manhattan_cost_function();
	int compute_reconfiguration_cost_function();
	void sendTaskSeek(vppnoc::vppnoc_transaction* p_trans, unsigned char* data);
	void nextTaskRelease(vppnoc::vppnoc_transaction* p_trans, ActiveTask* atsk);
	void taskManagerRelease(vppnoc::vppnoc_transaction* p_trans);
	int  compute_dispersion();
//------------------------------------
// Variables
//------------------------------------
private:
	std::vector<ActiveTask*> m_task_graph;

	sc_core::sc_time    m_nominal_period;
	sc_core::sc_time    m_minimum_period;
	sc_core::sc_time    m_maximum_period;
	double              m_elastic_coefficient;
	ActiveTask*         m_mappingTask;

	controller_state state;
	int uncomplete;
	int retry;
	
};

#endif /* CONTROLLER_H_ */
