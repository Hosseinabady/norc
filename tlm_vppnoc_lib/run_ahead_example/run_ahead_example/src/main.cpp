
#include <systemc>
#include <iostream>

#include "protocol_data_unit/nocapp_message_payload.h"
#include "include/nocapp_norc.h"
#include "include/nocapp_utilities.h"
#include "include/hr_time.h"
#include <fstream>

extern std::ofstream dumpnetworklayerfile;
extern std::ofstream dumpphysicallayerfile;
extern std::ofstream dumprouterlayerfile;
extern std::ofstream dumpreceivedmessagefile;

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace vppnoc;
using namespace nocapp;
double total_time = 0;
/* Global time variable */
CStopWatch  simulation_timer;

class top : sc_module {
public:
	SC_HAS_PROCESS(top);

	top(sc_core::sc_module_name name) : sc_module(name) {
		m_norc = new nocapp_norc("norc", 1, INTER_TILE);
	}
	

	nocapp_norc*                                                              m_norc;
  
};

int sc_main(int argc, char* argv[])
{
  total_time = 0;
  dumpnetworklayerfile.open("dumpnetworklayer.txt");
  dumpphysicallayerfile.open("dumpphysicallayer.txt");
  dumprouterlayerfile.open("dumprouterlayer.txt");
  dumpreceivedmessagefile.open("dumpreceivedmessagefile.txt");
	top t("t");

  simulation_timer.startTimer();
	sc_core::sc_start();//155, SC_NS);
  simulation_timer.stopTimer();
  std::cout << "latency time   = " << simulation_timer.getElapsedTime() << std::endl;
	sc_core::sc_stop();
	
	return 1;
}