#ifndef JPEG_CONVERTER_H
#define JPEG_CONVERTER_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"
#include "fstream"

extern bool bDebugFlag;
class ConverterTask : public NoCTask {
  public:
  ConverterTask();
  ~ConverterTask();

  virtual	int action(tlm::tlm_generic_payload* p_trans);
  
  std::ofstream OutputFile;
  int turn;
};


#endif

