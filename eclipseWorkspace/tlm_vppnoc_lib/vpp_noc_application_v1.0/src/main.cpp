
#include <systemc>
#include <iostream>

#include "protocol_data_unit/nocapp_message_payload.h"
#include "include/nocapp_norc.h"
#include "include/nocapp_utilities.h"
#include "host_module/nocapp_host_module.h"
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
    m_host_module = new nocapp_host_module("host_module", 1, DUMMY_TILE);
    for (int i = 0; i < 2*(NO_OF_ROW+NO_OF_COL); i++)
    {
      (*m_host_module->m_p_outPort[i])(*m_norc->m_p_inPort[i]);
      (*m_host_module->m_p_inPort[i])(*m_norc->m_p_outPort[i]);
    }

  
		SC_THREAD(run);
	}
	
	void run() {

	}
	nocapp_norc*                                                 m_norc;
  nocapp_host_module*                                          m_host_module;

  vppnoc_mm<vppnoc_payload_base, nocapp::nocapp_message_payload>            m_mm;
  
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
	sc_core::sc_start();//5311, SC_NS);
  simulation_timer.stopTimer();

  std::cout << "latency time   = " << simulation_timer.getElapsedTime() << std::endl;
	sc_core::sc_stop();
	
	return 1;
}