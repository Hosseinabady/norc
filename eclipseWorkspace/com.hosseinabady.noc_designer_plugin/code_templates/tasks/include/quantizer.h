#ifndef JPEG_QUANTIZER_H
#define JPEG_QUANTIZER_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"
#include "fstream"
#include "tasks/include/jpegutil.h"

#define B0 0.35355339059327376220
#define B1 0.25489778955207958447
#define B2 0.27059805007309849220
#define B3 0.30067244346752264027
#define B4 0.35355339059327376220
#define B5 0.44998811156820785231
#define B6 0.65328148243818826392
#define B7 1.28145772387075308943

extern bool bDebugFlag;
class QuantizerTask : public NoCTask {
  public:
  QuantizerTask();
  ~QuantizerTask();

  virtual	int action(tlm::tlm_generic_payload* p_trans);

  std::ofstream OutputFile;
  std::ofstream OutputFile2;
  double first_stage[8];

  void init() {
    first_stage[0] = B0;
    first_stage[1] = B1;
    first_stage[2] = B2;
    first_stage[3] = B3;
    first_stage[4] = B4;
    first_stage[5] = B5;
    first_stage[6] = B6;
    first_stage[7] = B7;

    FDCTCorrection(stdlqt, CorrectedLQT);
    FDCTCorrection(stdcqt, CorrectedCQT);
  }

  void FDCTCorrection(const int* in, double* out) {
    int i;
    double v,q;
    for (i = 0; i < 64; ++i) {
      v = first_stage[i >> 3];
      v *= first_stage[i & 7];
      q = (double) in[i];
      out[i] = v / q;
    }
  }

};


#endif

