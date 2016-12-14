#include <xercesc/sax2/DefaultHandler.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include "tasks/include/active_tasks.h"
#include "offchip/xml/include/xerces_string.h"
#include "model/tasks/include/application.h"

class TaskGraphHandler : public DefaultHandler
{
  XercesString mName;
private:
	std::vector<ActiveTask*> m_task_graph;
	std::string m_current_filed;
	bool flag;
	int fDataVolume;
public:
	TaskGraphHandler() {
		flag = false;
	}
	void setTaskGraph(std::vector<ActiveTask*> tsk) {
		m_task_graph = tsk;
	}
	std::vector<ActiveTask*> getTaskGraph() {
		return m_task_graph;
	}
  virtual void startDocument();

  virtual void endDocument();
    
  virtual void startElement(
    const XMLCh* const uri, 
    const XMLCh* const localname, 
    const XMLCh* const qname, 
    const Attributes& attrs);
    
  virtual void endElement(
    const XMLCh* const uri, 
    const XMLCh* const localname,
    const XMLCh* const qname);

  virtual void characters(
    const XMLCh* const chars,
    const XMLSize_t length);
};