#ifndef JPEG_FDCT_H
#define JPEG_FDCT_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"
#include "fstream"
#define FA1 (0.707106781f)
#define FA2 (0.541196100f)
#define FA3 FA1
#define FA4 (1.306562965f)
#define FA5 (0.382683433f)


class FDCTTask : public NoCTask {
  public:
  FDCTTask();
  ~FDCTTask();

  virtual	int action(tlm::tlm_generic_payload* p_trans);

  void FDCT1D( int row[ 8 ], int row_res[ 8 ], int bias );
  std::ofstream OutputFile;
  std::ofstream OutputFile2;
};


#endif

