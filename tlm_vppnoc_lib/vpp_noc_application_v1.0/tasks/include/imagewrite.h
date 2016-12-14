#ifndef JPEG_IMAGEWRITE_H
#define JPEG_IMAGERWRITE_H
#include <model/tasks/include/noc_task.h>


#include "tlm.h"
#include "fstream"

extern bool bDebugFlag;
class ImageWriteTask : public NoCTask {
  public:
    ImageWriteTask();
    ~ImageWriteTask();
     virtual	int action(tlm::tlm_generic_payload* p_trans);
  private:
	  std::ofstream OutputFile;
		int turn;

};


#endif

