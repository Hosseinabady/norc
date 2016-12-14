#ifndef READ_IMAGE_H
#define READ_IMAGE_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"
#include "fstream"


class ReadImageTask : public NoCTask {

  public:
    ReadImageTask();
    ~ReadImageTask();
    virtual	int action(tlm::tlm_generic_payload* p_trans);

	std::ifstream   InputFile;
    std::ofstream   OutputFile;
	int turn;
};


#endif

