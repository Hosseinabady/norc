#ifndef JPEG_BLOCKING_H
#define JPEG_BLOCKING_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"
#include "fstream"



class GeneralTask : public NoCTask {
  public:
  GeneralTask();
  ~GeneralTask();

  virtual	int action(tlm::tlm_generic_payload* p_trans);
  
  int turn;
  std::ofstream OutputFile;
  
};


#endif

