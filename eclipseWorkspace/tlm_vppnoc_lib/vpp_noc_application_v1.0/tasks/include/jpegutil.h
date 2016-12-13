#ifndef JPEG_UTIL_H
#define JPEG_UTIL_H
#include <model/tasks/include/noc_task.h>

#include <list>
#include "tlm.h"
#include "fstream"

#define DATA_TYPE               int

struct huffentry {
  unsigned short val;
  unsigned short nb;
};
typedef struct huffentry HUF;


extern  double first_stage[8];
extern  int    stdlqt[64];
extern  int    stdcqt[64];
extern  double CorrectedLQT[64];
extern  double CorrectedCQT[64];
extern  int    zigzag_order[64];
extern  int    TotalBlockNumber;
extern  int ldhtftab[];
extern  int cdhtftab[];
extern  int lahtftab[];
extern  int cahtftab[];
extern  struct huffentry ldht[];
extern  struct huffentry cdht[];
extern  struct huffentry laht[];
extern  struct huffentry caht[];

#endif

