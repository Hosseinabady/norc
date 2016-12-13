#ifndef NOCTASKDIRECTEDLINK
#define NOCTASKDIRECTEDLINK

#include "models/vppnoc_module_utilities.h"

class NoCTask;
class NoCTaskDirectedLink {

private:
	NoCTask*                m_next_task;
	int                     m_next_task_id;
	NOC_DATA_VOLUME_TYPE    m_data_volume;

public:
	void     setNextTask(NoCTask* nt) { m_next_task = nt;   }
	NoCTask* getNextTask()            { return m_next_task; }


	void                  setDataVolume(NOC_DATA_VOLUME_TYPE dv) { m_data_volume = dv;   }
	NOC_DATA_VOLUME_TYPE  getDataVolume()                        { return m_data_volume; }

	void setNextTaskId(int tid) { m_next_task_id = tid;  }
	int  getNextTaskId()        { return m_next_task_id; }
};


#endif