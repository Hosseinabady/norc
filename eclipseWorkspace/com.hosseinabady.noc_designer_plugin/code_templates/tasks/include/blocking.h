#ifndef JPEG_BLOCKING_H
#define JPEG_BLOCKING_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"
#include "fstream"


extern bool bDebugFlag;
class BlockingTask : public NoCTask {
  public:
  BlockingTask();
  ~BlockingTask();

  virtual	int action(tlm::tlm_generic_payload* p_trans);
  
  std::ofstream OutputFile;
  std::ofstream OutputFile2;
};


#endif

