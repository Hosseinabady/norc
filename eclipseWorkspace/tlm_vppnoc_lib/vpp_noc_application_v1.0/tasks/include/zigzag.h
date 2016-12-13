#ifndef JPEG_ZIGZAG_H
#define JPEG_ZIGZAG_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"


extern bool bDebugFlag;
class ZigzagTask : public NoCTask {
  public:
    ZigzagTask();
    ~ZigzagTask();
    virtual	int action(tlm::tlm_generic_payload* p_trans);
	std::ofstream OutputFile;
    std::ofstream OutputFile2;
};


#endif

