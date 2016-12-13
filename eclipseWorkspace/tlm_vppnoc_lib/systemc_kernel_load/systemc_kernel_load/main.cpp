#pragma comment(lib,"psapi.lib")
#include <systemc>
#include <time.h>
#include <sstream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <psapi.h>
#include "hr_time.h"

#define MAX_MODULE  2000

#define UNDER_TEST_THREAD 
#define THREAD 
#define STACK_SIZE   0x500

using namespace sc_core;
using namespace std;


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

class write_if : virtual public sc_interface
{
   public:
     virtual void write(int) = 0;
};




class producer_under_test : public sc_module
{
   public:
     sc_port<write_if> out;

     SC_HAS_PROCESS(producer_under_test);

     producer_under_test(sc_module_name name) : sc_module(name)
     {

       start = 0;
   	 	 SC_METHOD(main);
     }
     void main() {
       if (start == 0){
         start = 1;
         next_trigger(1.5, SC_NS);
       }
       else 
         out->write(1); 
     }
     int start;
};

class consumer_under_test : public sc_module, public write_if
{
   public:
     CStopWatch  simulation_timer;
     SC_HAS_PROCESS(consumer_under_test);

     consumer_under_test(sc_module_name name) : sc_module(name) {

#ifdef UNDER_TEST_THREAD
       set_stack_size(STACK_SIZE);
       SC_THREAD(main);
#else
		   SC_METHOD(main);
		   sensitive << activeData;
       dont_initialize();
#endif
     }

	 void write(int c) {
     data = c;
		 activeData.notify(sc_time(2, SC_MS));
		 simulation_timer.startTimer();
   }

   void main()
   {
#ifdef UNDER_TEST_THREAD
	   while(true) {
		   wait(activeData);
		   simulation_timer.stopTimer();
		 }
#else
		 simulation_timer.stopTimer();
#endif
   }

   sc_event activeData;
   int data;
};

class producer : public sc_module
{
   public:
     sc_port<write_if> out;

     SC_HAS_PROCESS(producer);

     producer(sc_module_name name) : sc_module(name)
     {

//#ifdef THREAD
//	   SC_THREAD(main);
//       set_stack_size(STACK_SIZE);
//#else
       SC_METHOD(main);
       start = 0;
//#endif
     }

     void main()
     {
       if (start == 0){
         start = 1;
         next_trigger(1.5, SC_NS);
       }
       else 
         out->write(1);
     }
     int start;
};

class consumer : public sc_module, public write_if
{
   public:
     SC_HAS_PROCESS(consumer);

     consumer(sc_module_name name) : sc_module(name)
     {

#ifdef THREAD
      
		 SC_THREAD(main1);
set_stack_size(STACK_SIZE);
#else
		 SC_METHOD(main1);
		 sensitive << activeData1;
     dont_initialize();
#endif
     }

	 void write(int c) {
		d = c;
		activeData1.notify(sc_time((1.0*(rand()+1))/(RAND_MAX)+5, SC_MS));
   }

   void main1()
   {
#ifdef THREAD
		 while(true) {
			wait(activeData1);

		}
#else	

#endif
		
     }
     sc_event activeData1;
     int d;
};

class Top : public sc_module
{
   public:

     producer_under_test *prod_ut_inst;
     consumer_under_test *cons_ut_inst;
#if MAX_MODULE != 0
     producer *prod_inst[MAX_MODULE];
     consumer *cons_inst[MAX_MODULE];
#endif

	Top(sc_module_name name) : sc_module(name)
    {
    prod_ut_inst = new producer_under_test("producer_under_test1");
		cons_ut_inst = new consumer_under_test("consumer_under_test1");

		prod_ut_inst->out(*cons_ut_inst);

#if MAX_MODULE != 0
		for (int j = 0; j < MAX_MODULE; j++) {
      int i = j;
			std::ostringstream prod_name;
			prod_name << "prod_name"  << i+1;
			prod_inst[i] = new producer(prod_name.str().c_str());
			
			std::ostringstream cons_name;
			cons_name << "cons_name"  << i+1;
			cons_inst[i] = new consumer(cons_name.str().c_str());
			
			prod_inst[i]->out(*cons_inst[i]);
		}
#endif



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

	std::cout << "total simulation time for module under test = " << top.cons_ut_inst->simulation_timer.getElapsedTime() << std::endl;


	PrintMemoryInfo( GetCurrentProcessId() );

	return 0;
}

