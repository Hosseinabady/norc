/*
 * controller.cpp
 *
 *  Created on: 29-May-2009
 *      Author: csxmh
 */

#include <tasks/include/taskmanager.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <systemc.h>
#include <model/tasks/include/noc_task.h>
#include <tasks/include/taska.h>
#include <tasks/include/taskb.h>
#include <tasks/include/taskc.h>
#include <tasks/include/readimage.h>
#include <tasks/include/converter.h>
#include <tasks/include/blocking.h>

#include <tasks/include/downsampler.h>
#include <tasks/include/fdct.h>
#include <tasks/include/huffman.h>
#include <tasks/include/imagewrite.h>
#include <tasks/include/zigzag.h>
#include <tasks/include/quantizer.h>

#include <NoRC/utilities/include/utilities.h>
#include "NoRC/protocol_data_units/include/noc_payload.h"

extern std::ofstream outputLogFile;
extern std::ofstream trafficLogFile;
extern std::ofstream trafficInRouterLogFile;

int TaskManager::action(tlm::tlm_generic_payload* p_trans) {
#ifdef DEBUG
		trafficLogFile << " TaskManager action " << std::endl;
#endif
	
	static int ststicI = 0;
	p_trans->acquire();
	tlm_noc::noc_message_payload* p_message_trans_ext;
	static ActiveTask* mappingTask;
	std::list<ActiveTask*>::iterator it;
	unsigned char* data;
	p_trans->get_extension(p_message_trans_ext);


	if (state == START) {
		char fileName[50]="tasks\\applications\\app1.app";
		read_task_graph(fileName);
		state = IDLE;
	}

	static bool task_mapped_flag = false;
	if (state == WAIT_TASK_ACK) {
		if ((noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::ACK_SEEK) && SelectActiveVolunteer(p_trans)) {
			noc_gp_hdr(p_message_trans_ext, m_req_task) = mappingTask->task;
			for (it = task_graph.begin(); it != task_graph.end(); it++){
				if ((*it)->task == noc_gp_hdr(p_message_trans_ext, m_req_task)) {
					(*it)->mapped = true;
					(*it)->core_id = noc_gp_hdr(p_message_trans_ext, m_dst_core);

					task_mapped_flag = true;
				} 
			}
			if (!task_mapped_flag) {
				std::cout << "Fatal Error: task is not in task graph but has been mapped!!" << std::endl;
				exit(0);
			}
			state = WAIT_TASK_MAP;
			p_trans->release();
			return 1;
		}
		if ((noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_RLS_ACK) || !SelectActiveVolunteer(p_trans) ) {
			noc_gp_hdr(p_message_trans_ext, m_volunteer) = NULL;
			std::cout << " retry = " << retry++ <<std::endl;
			if (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_RLS_ACK) {
				for (it = task_graph.begin(); it != task_graph.end(); it++){
					if ((*it)->task == noc_gp_hdr(p_message_trans_ext, m_req_task)) {
						(*it)->mapped = false;
						break;
					}
				}
				if (it == task_graph.end()) {
					std::cout << " FATAL ERROR, AN UNMAPPED TASK HAS BEEN RELEASED !!! " << std::endl;
					assert(false); exit(1);
				}
					
			}
			if (retry > NUMBER_OF_RETRY_TO_MAP_TASK) {
				for (it = task_graph.begin(); it != task_graph.end(); it++){
					if ((*it)->mapped == true) {
						break;
					}
				}
				if (it != task_graph.end()) {
					noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
					noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::TASK_RLS_REQ;
					noc_gp_hdr(p_message_trans_ext, m_req_task)    = (*it)->task;
					noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
					noc_gp_hdr(p_message_trans_ext, m_message_id)  = mappingTask->task->get_id();
					noc_gp_hdr(p_message_trans_ext, m_dst_core)    = (*it)->core_id;
					noc_gp_hdr(p_message_trans_ext, m_dst_tile)    = (*it)->tile_id;
					p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
					state == WAIT_TASK_ACK;
					p_trans->release();
					return 1;
				} else {
					noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
					noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::TASK_RLS_REQ;
					noc_gp_hdr(p_message_trans_ext, m_req_task)    = this;
					noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
					
					p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
					state == WAIT_TASK_ACK;
				}
				state = IDLE;
				p_trans->release();
				return 1;
			} else 
				state = IDLE;
		}
	} else if (state == WAIT_TASK_MAP) {
		int tile_id = noc_gp_hdr(p_message_trans_ext, m_src_tile);
		mappingTask->tile_id = tile_id;
		mappingTask->core_id = noc_gp_hdr(p_message_trans_ext, m_src_core);
		mappingTask->task->setHost_tile( tile_id);
		mappingTask->task->setHost_core( mappingTask->core_id);
		std::cout << "task  name = " << mappingTask->task->get_fileName() << " tile id = "<< mappingTask->tile_id << " core id = " << mappingTask->core_id << std::endl;

		outputLogFile << "tile for task = " << tile_id ;
		tile_task_spectrum[(tile_id-1)/NO_OF_ROW_CONST][(tile_id-1)%NO_OF_ROW_CONST]++;
		outputLogFile << "\ttask_name = " << mappingTask->task->get_fileName();
		outputLogFile << "\ttask_id = " << mappingTask->task->get_id() << std::endl;
		state = IDLE;
		trafficLogFile << " noc_gp_hdr(p_message_trans_ext, m_cmd); " << noc_gp_hdr(p_message_trans_ext, m_cmd) << std::endl;
	} 
	
	if (state == IDLE) {
		mappingTask = find_earliest_deadline_task();
		if (mappingTask != NULL) {
			trafficLogFile << " New task Starts to be mapped = " << mappingTask->task->get_id() << std::endl;
			tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
			p_trans->set_command( cmd );
			p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
			p_trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
			p_trans->set_data_length( 4 );
			p_trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
			p_trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
			p_trans->set_dmi_allowed( false ); // Mandatory initial value
			p_trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial values
		

			noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
			noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::TASK_SEEK;
			noc_gp_hdr(p_message_trans_ext, m_req_task)    = mappingTask->task;
			noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::BRD_CAST;
			noc_gp_hdr(p_message_trans_ext, m_message_id)  = mappingTask->task->get_id();
			noc_gp_hdr(p_message_trans_ext, m_life_time)   = TASK_SEEK_LIFE_TIME;
			noc_gp_hdr(p_message_trans_ext, m_dst_core)    = noc_gp_hdr(p_message_trans_ext, m_src_core);


			state = WAIT_TASK_ACK;
			p_trans->release();
			return 1; 
		} else {
			outputLogFile << "Application sucessfully mapped!" << std::endl;
			state = START;
			tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
			p_trans->set_command( cmd );
			p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
			p_trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
			p_trans->set_data_length( 4 );
			p_trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
			p_trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
			p_trans->set_dmi_allowed( false ); // Mandatory initial value
			p_trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial values
		
			if (noc_gp_hdr(p_message_trans_ext, m_cmd)== tlm_noc::APPLICATION_MAPPED) {
				noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
				noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::TASK_CMD_START;
				
				ActiveTask* tg = task_graph.back();
				
				noc_gp_hdr(p_message_trans_ext, m_req_task)    = tg->task;
				noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
				noc_gp_hdr(p_message_trans_ext, m_message_id)  = tg->task->get_id();
				noc_gp_hdr(p_message_trans_ext, m_dst_core)    = tg->core_id;
				noc_gp_hdr(p_message_trans_ext, m_dst_tile)    = tg->tile_id;
			} else {
				noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
				noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::APPLICATION_MAPPED;
			}
	

			state = IDLE;
			p_trans->release();
			return 1;
		}
	}
	p_trans->release();
	return 1;
}


void TaskManager::read_task_graph(char* filename) {
	std::ifstream fileHandler(filename, std::ifstream::in);
	const int MAX_LENGTH = 100;
    char line[MAX_LENGTH];
    char *token;
    ActiveTask* tmpTask;
	int number_of_tasks;
	ActiveTask** taskList;
	int task_id;

	if (fileHandler.good()) {
		outputLogFile << " Hello World! from TaskManager::read_task_graph" << std::endl;
		
		while (!fileHandler.eof()) {
			fileHandler.getline(line, MAX_LENGTH);
			token = strtok(line, " ");
			while(token != NULL) {
				if (!strcmp(token, "--")) {
					break;
				} else if (!strcmp(token, "NumberOfTasks")) {
					tmpTask = new ActiveTask;
					token = strtok(NULL, " ");
					number_of_tasks = atoi(token);
					taskList = new ActiveTask*[number_of_tasks+1];
					for (int i = 0; i < number_of_tasks+1; i++)
						taskList[i] = new ActiveTask;
				} else if (!strcmp(token, "Task")) {
					break;
				} else if (!strcmp(token, "ID")) {
					token = strtok(NULL, " ");
					task_id = atoi(token);
					tmpTask = taskList[task_id];
					break;
				}  else if (!strcmp(token, "Name")) {
					token = strtok(NULL, " ");
					if (!strcmp(token, "A")) {
						tmpTask->task = new TaskA();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
					}
					else if (!strcmp(token, "B")) {
						tmpTask->task = new TaskB();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
					}
					else if (!strcmp(token, "C")) {
						tmpTask->task = new TaskC();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
					}
					else if (!strcmp(token, "TaskManager")) {
						tmpTask->task = new TaskManager();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("taskmanager.cpp");
					}
					else if (!strcmp(token, "ReadImage")) {
						tmpTask->task = new ReadImageTask();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("readimage.cpp");
					}
					else if (!strcmp(token, "Converter")) {
						tmpTask->task = new ConverterTask();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("converter.cpp");
					}
					else if (!strcmp(token, "Blocking")) {
						tmpTask->task = new BlockingTask();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("blocking.cpp");
					}
					else if (!strcmp(token, "DownSampler")) {
						tmpTask->task = new DownsamplerTask();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("downsampler.cpp");
					}
					else if (!strcmp(token, "FDCT")) {
						tmpTask->task = new FDCTTask();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("fdct.cpp");
					}
					else if (!strcmp(token, "Quantizer")) {
						tmpTask->task = new QuantizerTask();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("quantizet.cpp");
					}
					else if (!strcmp(token, "ZigZag")) {
						tmpTask->task = new ZigzagTask();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("zigzag.cpp");
					}
					else if (!strcmp(token, "Huffman")) {
						tmpTask->task = new HuffmanTask();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("huffman.cpp");
					}
					else if (!strcmp(token, "WriteImage")) {
						tmpTask->task = new ImageWriteTask();
						taskList[task_id] = tmpTask;
						tmpTask->task->set_id(task_id);
						tmpTask->task->set_fileName("writeimage.cpp");
					}
					task_graph.push_back(tmpTask);
					break;
				
				} else if (!strcmp(token, "C")) {
					token = strtok(NULL, " ");
					sc_core::sc_time st (atoi(token), sc_core::SC_MS);
					tmpTask->task->set_computation_time(st+sc_core::sc_time_stamp());
					break;
				}  else if (!strcmp(token, "To")) {
					token = strtok(NULL, " ");
					sc_core::sc_time st (atoi(token), sc_core::SC_MS);
					tmpTask->task->set_nominal_period(st+sc_core::sc_time_stamp());
					break;
				}  else if (!strcmp(token, "Tmin")) {
					token = strtok(NULL, " ");
					sc_core::sc_time st (atoi(token), sc_core::SC_MS);
					tmpTask->task->set_minimum_period(st+sc_core::sc_time_stamp());
					break;
				}  else if (!strcmp(token, "Tmax")) {
					token = strtok(NULL, " ");
					sc_core::sc_time st (atoi(token), sc_core::SC_MS);
					tmpTask->task->set_maximum_period(st+sc_core::sc_time_stamp());
					break;
				}  else if (!strcmp(token, "E")) {
					token = strtok(NULL, " ");
					tmpTask->task->set_elastic_coefficient(atoi(token));
					break;
				}  else if (!strcmp(token, "PreTask")) {
					token = strtok(NULL, " ");
					int preTaskId = atoi(token);
					token = strtok(NULL, " ");
					tmpTask->predecessor_task_id = preTaskId;
					break;
				}  else if (!strcmp(token, "sucTask")) {
					token = strtok(NULL, " ");
					int sucTaskId = atoi(token);
					token = strtok(NULL, " ");
					tmpTask->successor_task_id = sucTaskId;
					break;
				}  else if (!strcmp(token, "END")) {
					break;
					//return;
				}  else
					break; 
			}
		}

		for (int i = 1; i < number_of_tasks+1; i++) {
			taskList[i]->task->add_predecessors(taskList[taskList[i]->predecessor_task_id]->task);
			taskList[i]->task->add_successors(taskList[taskList[i]->successor_task_id]->task);
		}
		task_graph.reverse();
	}
}

ActiveTask* TaskManager::find_earliest_deadline_task() {

	ActiveTask*  selectedTask = NULL;
	sc_core::sc_time earliest_deadline = sc_core::sc_time(3600, sc_core::SC_SEC);
	std::list<ActiveTask*>::iterator it;
	for (it = task_graph.begin(); it != task_graph.end(); it++) {
		if ((*it)->mapped == false) {
			//if (((*it)->task->get_computation_time()-sc_core::sc_time_stamp()) < earliest_deadline) {
			//	earliest_deadline = ((*it)->task->get_computation_time()-sc_core::sc_time_stamp());
				selectedTask = *it;
			}
	}
	
	return selectedTask;
}


bool TaskManager::SelectActiveVolunteer(tlm::tlm_generic_payload* p_trans) {
	tlm_noc::noc_message_payload* p_message_trans_ext;
	p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
	p_trans->get_extension(p_message_trans_ext);
	tlm_noc::Volunteer* vl;
	vl = noc_gp_hdr(p_message_trans_ext, m_volunteer);
	float min_cost = INFINITIVE;
	float tile_cost;
	int   best_volunteer_tile_id = 0;
	int   best_volunteer_core_id = 0;
 	for (int i = 0; i < TASK_SEEK_LIFE_TIME; i++) {
		switch(vl->tile_status) {
		case tlm_noc::TILE_BUSY:
			tile_cost = INFINITIVE;
			break;
		case tlm_noc::TILE_READY:
			tile_cost = 1; //Should be changed
			break;
		case tlm_noc::TILE_FREE:
			tile_cost = 2; //Should be changed
			break;
		case tlm_noc::TILE_RESERVED:
			tile_cost = INFINITIVE; //Should be changed
			break;

		default:
			std::cout<< "the program should not reach this point" << std::endl;
		}
		if (min_cost >= tile_cost ) {
			min_cost = tile_cost;
			best_volunteer_tile_id = vl->tile_id;
			best_volunteer_core_id = vl->core_id;
		}
		vl = vl->next;	
	}
	if (min_cost == 1)
		std::cout << "############################ min_cost =  " <<min_cost << std::endl;
	if (min_cost == INFINITIVE) {
		outputLogFile << "task cannot be mapped" << std::endl;
		return 0;
	}

	noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
	noc_gp_hdr(p_message_trans_ext, m_dst_tile)    = best_volunteer_tile_id;
	noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::TASK_ACTIVE_REQ;
	noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
	noc_gp_hdr(p_message_trans_ext, m_dst_core)    = best_volunteer_core_id;
	return 1;
}

TaskManager::~TaskManager() {

	std::list<ActiveTask*>::iterator it;

	for (it=task_graph.begin(); it!=task_graph.end(); it++) {
		delete (*it)->task;
		task_graph.erase(it);
	}
}
