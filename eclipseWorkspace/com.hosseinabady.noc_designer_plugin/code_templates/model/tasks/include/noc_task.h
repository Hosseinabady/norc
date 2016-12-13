/*
 * noc_task.h
 *
 *  Created on: 29-May-2009
 *      Author: csxmh
 */

#ifndef NOC_TASK_H_
#define NOC_TASK_H_

#include "tlm.h"
#include "list"




class NoCTask {

public:



	NoCTask(){
		id = -1;

	};
	~NoCTask(){};
	virtual int action(tlm::tlm_generic_payload* p_trans) { return 0;}

//-----SET & GET Functions
	int  get_id() {return id;}
	void set_id(int i) {id = i;}

	char*  get_fileName() {return file_name;}
	void set_fileName(char* fn) {file_name = fn;}

	int  get_taskType() {return taskType;}
	void set_taskType(int i) {taskType = i;}

	int  getHost_tile() {return host_tile;}
	void setHost_tile(int ht) {
		host_tile = ht;
	}
	int  getHost_core() {return host_core;}
	void setHost_core(int hc) {
		host_core = hc;
	}

	sc_core::sc_time  get_computation_time() {return computation_time;}
	void set_computation_time(sc_core::sc_time t) {computation_time = t;}

	sc_core::sc_time  get_nominal_period() {return nominal_period;}
	void set_nominal_period(sc_core::sc_time p) {nominal_period = p;}

	sc_core::sc_time  get_minimum_period() {return minimum_period;}
	void set_minimum_period(sc_core::sc_time p) { minimum_period = p;}

	sc_core::sc_time  get_maximum_period() {return maximum_period;}
	void set_maximum_period(sc_core::sc_time p) {maximum_period = p;}

	double get_elastic_coefficient() {return elastic_coefficient;}
	void   set_elastic_coefficient(double e) {elastic_coefficient = e;}
	
	void add_predecessors(NoCTask* t) {
		predecessors.push_back(t);
	}

	void add_successors(NoCTask* t) {
		successors.push_back(t);
	}

	std::list<NoCTask*> getSuccessors() {
		return successors;
	}

	std::list<NoCTask*> getPedecessors() {
		return predecessors;
	}

//------Private Variables
private:
	int                 id;
	int                 host_tile;
	int                 host_core;
	int                 taskType;
	char*               file_name;
	sc_core::sc_time    computation_time;
	sc_core::sc_time    nominal_period;
	sc_core::sc_time    minimum_period;
	sc_core::sc_time    maximum_period;
	double              elastic_coefficient;
	std::list<NoCTask*> predecessors;
	std::list<NoCTask*> successors;
};

#endif /* NOC_TASK_H_ */
