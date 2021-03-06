/*
 * controller.cpp
 *
 *  Created on: 29-May-2009
 *      Author: csxmh
 */

#include <tasks/include/taskmanager.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <systemc.h>
#include <model/tasks/include/noc_task.h>
#include <cstdlib>


#include <NoRC/utilities/include/utilities.h>
#include "NoRC/protocol_data_units/include/noc_payload.h"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/ContentHandler.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <stdio.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "tasks/xml/include/task_graph_handler.h"
#include "offchip/xml/include/xerces_string.h"



extern std::ofstream outputLogFile;
extern std::ofstream trafficLogFile;
extern std::ofstream trafficInRouterLogFile;
extern std::ofstream taskMappingPatternLogFile;
extern char application_path[300];
extern int TASK_SEEK_LIFE_TIME;
extern int dispersion;

int TaskManager::action(tlm::tlm_generic_payload* p_trans) {
#ifdef DEBUG
		trafficLogFile << " TaskManager action " << std::endl;
#endif
	
	static int ststicI = 0;
//	p_trans->acquire();
	tlm_noc::noc_message_payload* p_message_trans_ext;
	
	std::vector<ActiveTask*>::iterator it;
	unsigned char data[] = "data";
	p_trans->get_extension(p_message_trans_ext);
	static bool task_mapped_flag = false;
	char fileName[300] = "";
	int tile_id;
	//std::cout << " state = " << state << std::endl;
	//std::cout << " noc_gp_hdr(p_message_trans_ext, m_cmd) = " << noc_gp_hdr(p_message_trans_ext, m_cmd) << std::endl;
	switch (state) {
	case START :
		strcat(fileName, application_path);
		strcat(fileName, noc_gp_hdr(p_message_trans_ext, m_req_task)->getFileName());
		read_task_graph(fileName);
		state = TASK_MAPPING;
	case TASK_MAPPING:
		if ((noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_RLS_ACK)) {
			ReleaseACK(p_trans);
			p_trans->release();
			state = WAIT_TASK_ACK;
			return 1;
		} else if (    noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_ACTIVE_REQ 
			        || noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_ACTIVE_ACK 
					|| noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::ACK_SEEK 
					) {
			m_mappingTask = find_earliest_deadline_task();
			
			if (m_mappingTask != NULL) {
				sendTaskSeek(p_trans, data);
				state = WAIT_TASK_ACK;
				p_trans->release();
				return 1; 
			} else {
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
			
				dispersion += compute_dispersion();
				std::cout << " Manhatten = " << compute_manhattan_cost_function() << std::endl;
#ifdef TASK_MAPPING_PATTERN_LOG
				taskMappingPatternLogFile << "        <Cost Manhattan=\""  << compute_manhattan_cost_function() << "\"  Dispersion=\"" << compute_dispersion()
											  << "\" Reconfiguration=\"" << compute_reconfiguration_cost_function() << "\"/>" << std::endl;
					
				taskMappingPatternLogFile << "    </Application>" << std::endl;
#endif
				noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
				noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::APPLICATION_MAPPED;
				
				state = TASK_MAPPING;
				p_trans->release();
				return 1;
			}
		} else if (noc_gp_hdr(p_message_trans_ext, m_cmd)== tlm_noc::APPLICATION_MAPPED) {
			noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
			noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::TASK_CMD_START;
			ActiveTask* tg = m_task_graph.back();
					
			noc_gp_hdr(p_message_trans_ext, m_req_task)    = tg->getTask();
			noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
			noc_gp_hdr(p_message_trans_ext, m_message_id)  = tg->getTask()->getTaskId();
			noc_gp_hdr(p_message_trans_ext, m_dst_core)    = tg->getCoreId();
			noc_gp_hdr(p_message_trans_ext, m_dst_tile)    = tg->getTileId();
			noc_gp_hdr(p_message_trans_ext, m_period)      = getNominalPeriod().value();

			p_trans->release();
			state = TASK_MAPPING;
			return 1;
		} else {
			std::cout << " FATAL Error Taskmanager Code: 202020" << std::endl;
			std::cout << " noc_gp_hdr(p_message_trans_ext, m_cmd) = " << noc_gp_hdr(p_message_trans_ext, m_cmd) << std::endl;
		}
		break;
	case WAIT_TASK_ACK:
		if (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::ACK_SEEK) {
			if (SelectActiveVolunteer(p_trans)) {
				noc_gp_hdr(p_message_trans_ext, m_req_task) = m_mappingTask->getTask();
				for (it = m_task_graph.begin(); it != m_task_graph.end(); it++){
					if ((*it)->getTask() == noc_gp_hdr(p_message_trans_ext, m_req_task)) {
						(*it)->setMapped(true);
						(*it)->setCoreId(noc_gp_hdr(p_message_trans_ext, m_dst_core));
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
			} else {
				retry++;
				if (retry < 0) {
					state = TASK_MAPPING;
					return 1;
				}
				tlm_noc::noc_message_payload* p_message_trans_ext;
				p_trans->get_extension(p_message_trans_ext);
	
				noc_gp_hdr(p_message_trans_ext, m_volunteer) = NULL;

				std::vector<ActiveTask*>::iterator it;
				for (it = m_task_graph.begin(); it != m_task_graph.end(); it++){
					if ((*it)->getMapped() == true) {
						break;
					}
				}
				if (it != m_task_graph.end()) {
					nextTaskRelease(p_trans, (*it));
					state = WAIT_TASK_ACK;
				} else {
					taskManagerRelease(p_trans);
					state = TASK_MAPPING;
				}
				p_trans->release();
				return 1;
			}
		} else if ((noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::TASK_RLS_ACK)) {
			
			std::vector<ActiveTask*>::iterator it;
			for (it = m_task_graph.begin(); it != m_task_graph.end(); it++){
				if ((*it)->getTask() == noc_gp_hdr(p_message_trans_ext, m_req_task)) {
					(*it)->setMapped(false);
					break;
				}
			}
			tlm_noc::noc_message_payload* p_message_trans_ext;
			p_trans->get_extension(p_message_trans_ext);
			noc_gp_hdr(p_message_trans_ext, m_volunteer) = NULL;
			for (it = m_task_graph.begin(); it != m_task_graph.end(); it++){
				if ((*it)->getMapped() == true) {
					break;
				}
			}
			if (it != m_task_graph.end()) {
				nextTaskRelease(p_trans, (*it));
				state = WAIT_TASK_ACK;
			} else {
				taskManagerRelease(p_trans);
				state = TASK_MAPPING;
			}
			p_trans->release();
			return 1;
		}
		break;
	case WAIT_TASK_MAP: 
		retry = 0;
		tile_id = noc_gp_hdr(p_message_trans_ext, m_src_tile);
		m_mappingTask->setTileId(tile_id);
		m_mappingTask->setCoreId(noc_gp_hdr(p_message_trans_ext, m_src_core));
		m_mappingTask->getTask()->setHostTileId( tile_id);
		m_mappingTask->getTask()->setHostCoreId( m_mappingTask->getCoreId());
		//std::cout << "task  name = " << m_mappingTask->getTask()->getName() << " tile id = "<< m_mappingTask->getTileId() << " core id = " << m_mappingTask->getCoreId() << std::endl;
		//outputLogFile << "tile for task = " << tile_id ;
		tile_task_spectrum[(tile_id-1)/NO_OF_ROW_CONST][(tile_id-1)%NO_OF_ROW_CONST]++;
		//std::cout << "\ttask_name = " << m_mappingTask->getTask()->getName();
		//std::cout << "\ttask_id = " << m_mappingTask->getTask()->getTaskId() << std::endl;
		state = TASK_MAPPING;
		//trafficLogFile << " noc_gp_hdr(p_message_trans_ext, m_cmd); " << noc_gp_hdr(p_message_trans_ext, m_cmd) << std::endl;
		p_trans->release();
		return 1;
	default :
		p_trans->release();
		return 1;
	}
	std::cout << " END state = " << state << std::endl;
}


void TaskManager::read_task_graph(char* filename) {


	XMLPlatformUtils::Initialize();
	TaskGraphHandler handler;
	
	if(_access(filename, 04)) {
		std::cout << "Cannot open the file: " << filename << std::endl;
		exit(0);
	}
	XercesString wstrPath(filename);
	SAX2XMLReader* pParser = XMLReaderFactory::createXMLReader();
	LocalFileInputSource source(wstrPath);
	// our application specific handler.
		
	pParser->setFeature( XercesString("http://xml.org/sax/features/validation"), true );
	pParser->setFeature( XercesString("http://apache.org/xml/features/validation/dynamic"), true );
	pParser->setContentHandler(&handler);
	pParser->setErrorHandler(&handler);
	pParser->parse(source);
	
	
	m_task_graph = handler.getTaskGraph();
	// terminate the XML library
	XMLPlatformUtils::Terminate();

	for (unsigned int i = 0; i < m_task_graph.size(); i++) {
		m_task_graph[i]->getTask()->setApplicationId(getApplicationId());
		for(unsigned int j = 0; j < m_task_graph[i]->getTask()->getSuccessors()->size(); j++) {
			for (unsigned int k = 0; k < m_task_graph.size(); k++) {
				if (m_task_graph[i]->getTask()->getSuccessors()->at(j)->getNextTaskId() == m_task_graph[k]->getTask()->getTaskId()) {
					m_task_graph[i]->getTask()->getSuccessors()->at(j)->setNextTask(m_task_graph[k]->getTask());
				}
			}
		}
		for(unsigned int j = 0; j < m_task_graph[i]->getTask()->getPredecessors()->size(); j++) {
			for (unsigned int k = 0; k < m_task_graph.size(); k++) {
				if (m_task_graph[i]->getTask()->getPredecessors()->at(j)->getTaskId() == m_task_graph[k]->getTask()->getTaskId()) {
					delete m_task_graph[i]->getTask()->getPredecessors()->at(j);
					m_task_graph[i]->getTask()->getPredecessors()->at(j) = m_task_graph[k]->getTask();
				}
			}
		}
	}
	std::reverse(m_task_graph.begin(), m_task_graph.end());
	
}


ActiveTask* TaskManager::find_earliest_deadline_task() {

	ActiveTask*  selectedTask = NULL;
	sc_core::sc_time earliest_deadline = sc_core::sc_time(3600, sc_core::SC_SEC);
	std::vector<ActiveTask*>::iterator it;
	for (it = m_task_graph.begin(); it != m_task_graph.end(); it++) {
		if ((*it)->getMapped() == false) {
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
	int min_cost = INFINITIVE;
	int tile_cost;
	int   best_volunteer_tile_id = 0;
	int   best_volunteer_core_id = 0;
	while(vl != NULL) {
		switch(vl->tile_status) {
		case tlm_noc::TILE_BUSY:
			tile_cost = INFINITIVE;
			break;
		case tlm_noc::TILE_READY:
			tile_cost = (int) abs((vl->tile_id-1)/NO_OF_COL_CONST - (getHostTileId()-1)/NO_OF_COL_CONST) +
					    (int) abs((vl->tile_id-1)%NO_OF_COL_CONST - (getHostTileId()-1)%NO_OF_COL_CONST);
			break;
		case tlm_noc::TILE_FREE:
			//tile_cost = 2; //Should be changed
			tile_cost = (int) abs((vl->tile_id-1)/NO_OF_COL_CONST - (getHostTileId()-1)/NO_OF_COL_CONST) +
			            (int) abs((vl->tile_id-1)%NO_OF_COL_CONST - (getHostTileId()-1)%NO_OF_COL_CONST);
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
	vl = noc_gp_hdr(p_message_trans_ext, m_volunteer);
	while(vl != NULL) {
		tlm_noc::Volunteer* vlTmp;
		vlTmp = vl;
		vl = vl->next;
		delete vlTmp;
	}
	noc_gp_hdr(p_message_trans_ext, m_volunteer) = NULL;
	noc_gp_hdr(p_message_trans_ext, m_req_task)->setReconfigurationCost(min_cost);
	if (min_cost == 1) {
		//std::cout << "############################ min_cost =  " <<min_cost << std::endl;
	} else if (min_cost == INFINITIVE) {
		//outputLogFile << "task cannot be mapped" << std::endl;
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

	std::vector<ActiveTask*>::iterator it;

	for (it=m_task_graph.begin(); it!=m_task_graph.end(); it++) {
		delete (*it)->getTask();
		m_task_graph.erase(it);
	}
}

int TaskManager::compute_manhattan_cost_function() {
	int cost = 0;

	std::vector<ActiveTask*>::iterator it;
	for (it = m_task_graph.begin(); it != m_task_graph.end(); it++) {
		NoCTask* tsk = (*it)->getTask();
		std::vector<NoCTaskDirectedLink*>* succTasks;
		int currentTileId = tsk->getHostTileId();
		succTasks = tsk->getSuccessors();
		for (unsigned int i = 0; i < succTasks->size(); i++) {
			int succTskId = succTasks->at(i)->getNextTaskId();
			for (unsigned int j = 0; j < m_task_graph.size(); j++) {
				if (m_task_graph[j]->getTask()->getTaskId() == succTskId)
					cost += abs((currentTileId-1)/NO_OF_COL_CONST - (m_task_graph[j]->getTileId()-1)/NO_OF_COL_CONST) +
					        abs((currentTileId-1)%NO_OF_COL_CONST - (m_task_graph[j]->getTileId()-1)%NO_OF_COL_CONST);
			}
		}
	}
	return cost;
}

int TaskManager::compute_reconfiguration_cost_function() {
	int cost = 0;
	std::vector<ActiveTask*>::iterator it;
	for (it = m_task_graph.begin(); it != m_task_graph.end(); it++) {
		NoCTask* tsk = (*it)->getTask();
		cost += tsk->getReconfigurationCost();
		
	}
	return cost;
}

void TaskManager::ReleaseACK(transaction_type* p_trans) {

	tlm_noc::noc_message_payload* p_message_trans_ext;
	p_trans->get_extension(p_message_trans_ext);
	std::vector<ActiveTask*>::iterator it;
	for (it = m_task_graph.begin(); it != m_task_graph.end(); it++){
		if ((*it)->getMapped() == true) {
			break;
		}
	}
	if (it != m_task_graph.end()) {
		noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
		noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::TASK_RLS_REQ;
		noc_gp_hdr(p_message_trans_ext, m_req_task)    = (*it)->getTask();
		noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
		noc_gp_hdr(p_message_trans_ext, m_message_id)  = (*it)->getTask()->getTaskId();
		noc_gp_hdr(p_message_trans_ext, m_dst_core)    = (*it)->getCoreId();
		noc_gp_hdr(p_message_trans_ext, m_dst_tile)    = (*it)->getTileId();
		p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
		
	} else {
		std::cout << " FATAL ERROR, APPLICATION HAS NOT BEEN MAPPED BUT WANTS TO BE RELEASED !!! " << std::endl;
		assert(false); exit(1);
	}
}




void TaskManager::sendTaskSeek(transaction_type* p_trans, unsigned char* data) {
	tlm_noc::noc_message_payload* p_message_trans_ext;
	p_trans->get_extension(p_message_trans_ext);

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
	noc_gp_hdr(p_message_trans_ext, m_req_task)    = m_mappingTask->getTask();
	noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::BRD_CAST;
	noc_gp_hdr(p_message_trans_ext, m_message_id)  = m_mappingTask->getTask()->getTaskId();
	noc_gp_hdr(p_message_trans_ext, m_volunteer)   = NULL;
	noc_gp_hdr(p_message_trans_ext, m_life_time)   = TASK_SEEK_LIFE_TIME;
	noc_gp_hdr(p_message_trans_ext, m_dst_core)    = noc_gp_hdr(p_message_trans_ext, m_src_core);


}



void TaskManager::nextTaskRelease(transaction_type* p_trans, ActiveTask* atsk) {
	tlm_noc::noc_message_payload* p_message_trans_ext;
	p_trans->get_extension(p_message_trans_ext);
	
	noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
	noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::TASK_RLS_REQ;
	noc_gp_hdr(p_message_trans_ext, m_req_task)    = atsk->getTask();
	noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
	noc_gp_hdr(p_message_trans_ext, m_message_id)  = atsk->getTask()->getTaskId();
	noc_gp_hdr(p_message_trans_ext, m_dst_core)    = atsk->getCoreId();
	noc_gp_hdr(p_message_trans_ext, m_dst_tile)    = atsk->getTileId();
	p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
}

void TaskManager::taskManagerRelease(transaction_type* p_trans) {
	tlm_noc::noc_message_payload* p_message_trans_ext;
	p_trans->get_extension(p_message_trans_ext);

	noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
	noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::TASK_RLS_REQ;
	noc_gp_hdr(p_message_trans_ext, m_req_task)    = this;
	noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
	p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
}

int TaskManager::compute_dispersion() {
	int dis = 0;
	int maxXIndex = -1;
	int minXIndex = NO_OF_COL_CONST + 1;
	int maxYIndex = -1;
	int minYIndex = NO_OF_ROW_CONST + 1;

	std::vector<ActiveTask*>::iterator it;
	for (it = m_task_graph.begin(); it != m_task_graph.end(); it++) {
		NoCTask* tsk = (*it)->getTask();
		int index = tsk->getHostTileId() - 1;
		int x = (index)%NO_OF_COL_CONST;
		int y = (index)/NO_OF_ROW_CONST;
		if ( x > maxXIndex)
			maxXIndex = x;
		if (x < minXIndex)
			minXIndex = x;
		if (y > maxYIndex)
			maxYIndex = y;
		if (y < minYIndex)
			minYIndex = y;
	}
	
	int box_size = ((maxXIndex-minXIndex+1)*(maxYIndex-minYIndex+1)*2);
	dis = box_size - (m_task_graph.size()+1);
	return 100.0*dis/box_size;
}