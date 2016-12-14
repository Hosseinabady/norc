/*
 * noc_task.h
 *
 *  Created on: 29-May-2009
 *      Author: csxmh
 */

#ifndef NOC_TASK_H_
#define NOC_TASK_H_

#include <systemc>
#include "list"
#include "core/vppnoc_types.h"
#include "vppnoc_task_directed_link.h"

using namespace std;
using namespace vppnoc;
using namespace sc_core;
using namespace sc_dt;


class NoCTask {

	//------Private Variables
private:
	int                   m_task_id;
	int                   m_host_tile_id;
	int                   m_host_core_id;
	int                   m_taskType;
	int                   m_application_id;
	char*                 m_name;
	char*                 m_file_name;
	int                   m_reconfiguration_cost;
	NoCTask*              m_task_manager;
	sc_core::sc_time      m_computation_time;
	std::vector<NoCTask*> m_predecessors;
	std::vector<NoCTaskDirectedLink*> m_successors;

public:

	NoCTask(){
		m_task_id = -1;
		m_host_tile_id = -1;
		m_host_core_id = -1;
		m_successors.clear();
		m_predecessors.clear();

	};
	~NoCTask(){};
	virtual int action(vppnoc_transaction* p_trans) { return 0;}

//-----SET & GET Functions
	int  getTaskId() {return m_task_id;}
	void setTaskId(int i) {m_task_id = i;}

	char*  getName() {return m_name;}
	void setName(char* n) {m_name = n;}

	char*  getFileName() {return m_file_name;}
	void setFileName(char* fn) {m_file_name = fn;}

	int  getTaskType() {return m_taskType;}
	void setTaskType(int i) {m_taskType = i;}

	int  getHostTileId() {return m_host_tile_id;}
	void setHostTileId(int ht) { m_host_tile_id = ht; }
	int  getHostCoreId() {return m_host_core_id;}
	void setHostCoreId(int hc) { m_host_core_id = hc; }

	NoCTask* getTaskManager() { return m_task_manager; }
	void setTaskManager(NoCTask* tm) { m_task_manager = tm; }

	int getApplicationId() { return m_application_id; }
	void setApplicationId(int api) {m_application_id = api; }

	int  getReconfigurationCost() {return m_reconfiguration_cost;}
	void setReconfigurationCost(int i) {m_reconfiguration_cost = i;}

	sc_core::sc_time  getComputationTime() {return m_computation_time;}
	void setComputationTime(sc_core::sc_time t) {m_computation_time = t;}

	
	void addPredecessors(NoCTask* t) {
		m_predecessors.push_back(t);
	}

	void addSuccessors(NoCTaskDirectedLink* dl) {
		m_successors.push_back(dl);
	}

	std::vector<NoCTaskDirectedLink*>* getSuccessors() {
		return &m_successors;
	}

	std::vector<NoCTask*>* getPredecessors() {
		return &m_predecessors;
	}
};

#endif /* NOC_TASK_H_ */
