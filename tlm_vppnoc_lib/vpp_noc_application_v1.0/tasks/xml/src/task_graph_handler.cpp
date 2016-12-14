#include "tasks/xml/include/task_graph_handler.h"
#include <xercesc/sax2/Attributes.hpp>
#include <stdio.h>
#include "offchip/xml/include/xerces_string.h"
#include <float.h>
#include <cstdlib>
#include <systemc>
#include <tasks/include/readimage.h>
#include <tasks/include/general_task.h>

void TaskGraphHandler::startDocument() {
  
}


void TaskGraphHandler::endDocument()
{

}


void TaskGraphHandler::startElement(
	const XMLCh* const uri, 
	const XMLCh* const localname, 
	const XMLCh* const qname, 
	const Attributes& attrs) {
	const XercesString kstrApplication("Application");
	const XercesString kstrSize("Size");
	
	const XercesString kstrTask("Task");
	const XercesString kstrName("Name");
	const XercesString kstrType("Type");
	const XercesString kstrId("Id");
	const XercesString kstrComputationTime("ComputationTime");
	const XercesString kstrPredecessorTask("PredecessorTask");
	const XercesString kstrSuccessorTask("SuccessorTask");
	const XercesString kstrDataVolume("Data_Volume");

	if ( !XMLString::compareString(localname, kstrApplication) ) {
		const XercesString wstrSize( attrs.getValue(kstrSize));
		int fValue = wcstod(wstrSize, NULL);
		m_current_filed = "Application";
	} else if ( !XMLString::compareString(localname, kstrTask) ) {
		ActiveTask* atsk = new ActiveTask();
		const XercesString wstrName( attrs.getValue(kstrName));
		char* name = new char[50];
		wcstombs(name, wstrName, 49);

		const XercesString wstrType( attrs.getValue(kstrType));
		char* type = new char[50];
		wcstombs(type, wstrType, 49);
		atsk->setTask(new GeneralTask());
		atsk->getTask()->setName(name);
		atsk->getTask()->setTaskType(atoi(type));
		m_task_graph.push_back(atsk);
		m_current_filed = "Task";
	} else if ( !XMLString::compareString(localname, kstrId) ) {
		m_current_filed = "Id";
	} else if ( !XMLString::compareString(localname, kstrComputationTime) ) {
		m_current_filed = "ComputationTime";
	} else if ( !XMLString::compareString(localname, kstrPredecessorTask) ) {
		m_current_filed = "PredecessorTask";
	} else if ( !XMLString::compareString(localname, kstrSuccessorTask) ) {
		const XercesString wstrDataVolume( attrs.getValue(kstrDataVolume));
		fDataVolume = wcstod(wstrDataVolume, NULL);
		m_current_filed = "SuccessorTask";
	}
	flag = true;
}

void TaskGraphHandler::endElement(
  const XMLCh* const uri, 
  const XMLCh* const localname,
  const XMLCh* const qname) {
	  flag = false;
  
}

void TaskGraphHandler::characters(
  const XMLCh* const chars,
  const XMLSize_t length) {

	char *strChars = new char [length+1];
	XMLString::transcode(chars, strChars, length);
	strChars[length] = 0;
	if (!m_current_filed.compare("Id") && flag)
		m_task_graph.back()->getTask()->setTaskId(atoi(strChars));
	else if (!m_current_filed.compare("ComputationTime") && flag)
		m_task_graph.back()->getTask()->setComputationTime(sc_core::sc_time(atoi(strChars), sc_core::SC_NS));
	else if (!m_current_filed.compare("PredecessorTask") && flag) {
		if (atoi(strChars) != 0) {
			NoCTask* tsk = new NoCTask();
			tsk->setTaskId(atoi(strChars));
			m_task_graph.back()->getTask()->addPredecessors(tsk);
		}
	} else if (!m_current_filed.compare("SuccessorTask") && flag) {
		if (atoi(strChars) != 0) {
			NoCTaskDirectedLink* directedLink = new NoCTaskDirectedLink;
			directedLink->setNextTaskId(atoi(strChars));
			directedLink->setDataVolume(fDataVolume);
			m_task_graph.back()->getTask()->addSuccessors(directedLink);
		} 
	}

   delete[] strChars;
}