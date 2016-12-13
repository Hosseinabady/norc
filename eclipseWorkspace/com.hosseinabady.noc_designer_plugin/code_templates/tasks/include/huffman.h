#ifndef JPEG_HUFFMAN_H
#define JPEG_HUFFMAN_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"
#include "fstream"
#include "tasks/include/jpegutil.h"
#include <NoRC/utilities/include/utilities.h>
#include "NoRC/protocol_data_units/include/noc_payload.h"

extern bool bDebugFlag;

class HuffmanTask : public NoCTask {
  public:
    HuffmanTask();
    ~HuffmanTask();
    virtual	int action(tlm::tlm_generic_payload* p_trans);
    char magnitude(short v);
    inline void  huf_run (tlm_noc::noc_message_payload* p_message_trans_ext, short *pred, HUF   *dcht, HUF   *acht, int iBlock);
    void HCode(tlm_noc::noc_message_payload* p_message_trans_ext, unsigned short v, unsigned short nb);
    void STORE_BITS(tlm_noc::noc_message_payload* p_message_trans_ext, int bb);
    void wb(tlm_noc::noc_message_payload* p_message_trans_ext, int j);
    void ww(tlm_noc::noc_message_payload* p_message_trans_ext, int j);
    void JpegHeader(tlm_noc::noc_message_payload* p_message_trans_ext, int sizex, int sizey);

    short lpred;
    short crpred;
    short cbpred;
    int   iPixel;

    unsigned int   bb;
    unsigned int   nbb;
    int num;
	std::ofstream OutputFile;
	std::ofstream OutputFile2;
};


#endif

