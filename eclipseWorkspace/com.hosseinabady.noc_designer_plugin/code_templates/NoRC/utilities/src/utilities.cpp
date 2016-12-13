
#include "NoRC/utilities/include/utilities.h"
#include <systemc>
#include <iostream>
#include <fstream>

extern std::ofstream outputLogFile;
extern std::ofstream trafficLogFile;
extern std::ofstream fatal_error_log;

void PrMsg(char* FileName, int LineNumber, sc_core::sc_module_name name)
{
	outputLogFile << std::endl;

	outputLogFile << "!-----------------------------------------------------------------------!" << std::endl;
	outputLogFile << " Module name = "<< name << "  Message from file "<< FileName << " at line " << LineNumber  << " Simulation time = " << sc_core::sc_time_stamp().to_string() << std::endl;
	outputLogFile << "!-----------------------------------------------------------------------!" << std::endl;
}

void trafficLog(sc_core::sc_module_name name, int id, char direction[]) {
	trafficLogFile << "name=" << name << " channel_id=" << id << " direction=" << direction << " time=" << sc_core::sc_time_stamp().to_string() << std::endl;

}

int tile_task_spectrum[NO_OF_ROW_CONST][NO_OF_COL_CONST] = {0};