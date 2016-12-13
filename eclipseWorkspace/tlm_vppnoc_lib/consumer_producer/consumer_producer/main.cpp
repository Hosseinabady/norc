#pragma comment(lib,"psapi.lib")
#include <systemc>
#include <time.h>
#include <sstream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <psapi.h>
#include "hr_time.h"

#include "producer.h"
#include "consumer.h"


using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace vppnoc;
using namespace nocapp;



CStopWatch  elaboration_timer;
CStopWatch  total_simulation_timer;

void PrintMemoryInfo( DWORD processID )
{
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;

    // Print the process identifier.

    printf( "\nProcess ID: %u\n", processID );

    // Print information about the memory usage of the process.

    hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                             PROCESS_VM_READ,
                             FALSE, 
                             processID );
    if (NULL == hProcess)
        return;

    if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
    {
        printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
        printf( "\tYour app's PEAK MEMORY CONSUMPTION: 0x%08X\n", 
                  pmc.PeakWorkingSetSize );
        printf( "\tYour app's CURRENT MEMORY CONSUMPTION: 0x%08X\n", pmc.WorkingSetSize );
        printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n", 
                  pmc.QuotaPeakPagedPoolUsage );
        printf( "\tQuotaPagedPoolUsage: 0x%08X\n", 
                  pmc.QuotaPagedPoolUsage );
        printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n", 
                  pmc.QuotaPeakNonPagedPoolUsage );
        printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n", 
                  pmc.QuotaNonPagedPoolUsage );
        printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage ); 
        printf( "\tPeakPagefileUsage: 0x%08X\n", 
                  pmc.PeakPagefileUsage );
    }

    CloseHandle( hProcess );
}




class Top : public sc_module
{
   public:

     producer *prod;
     consumer *cons;

	Top(sc_module_name name) : sc_module(name)
    {
    prod = new producer("producer");
		cons = new consumer("consumer");

    prod->m_outPort2Consumer(cons->m_inPort_from_producer);
//    cons->m_inPort_from_producer(prod->m_outPort2Consumer);

   }
};



int sc_main(int argc, char* argv[])
{
	srand ((unsigned int)time(NULL) );
	
	elaboration_timer.startTimer();
	
	Top top("top");
	PrintMemoryInfo( GetCurrentProcessId() );
	elaboration_timer.stopTimer();
	total_simulation_timer.startTimer();
	sc_start();
	total_simulation_timer.stopTimer();

	
	std::cout << "total elaboration time  = " << elaboration_timer.getElapsedTime() << std::endl;
	std::cout << "total simulation time   = " << total_simulation_timer.getElapsedTime() << std::endl;

//	std::cout << "total simulation time for module under test = " << top.cons_ut_inst->simulation_timer.getElapsedTime() << std::endl;


	PrintMemoryInfo( GetCurrentProcessId() );

	return 0;
}

