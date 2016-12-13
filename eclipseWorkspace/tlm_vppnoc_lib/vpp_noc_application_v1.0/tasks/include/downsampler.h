#ifndef JPEG_DOWNSAMPLER_H
#define JPEG_DOWNSAMPLER_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"
#include "fstream"


class DownsamplerTask : public NoCTask {
  public:
  DownsamplerTask();
  ~DownsamplerTask();
   virtual	int action(tlm::tlm_generic_payload* p_trans);
  std::ofstream OutputFile;
  std::ofstream OutputFile2;
};


#endif

