#ifndef APPLICATIONINJECTOR_H
#define APPLICATIONINJECTOR_H
#include <systemc>
#include <stdio.h>
#include <stdlib.h>
#include "NoRC/include/norc.h"
#include "NoRC/tile/network_interface/include/transport_layer.h"

extern std::ofstream outputLogFile;
extern std::ofstream trafficInRouterLogFile;

class ApplicationInjector : public NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> {

public:
	ApplicationInjector(sc_core::sc_module_name name) : NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>(name, 0) {
		mapped_application_number	 = 0;
		tried_application_number =0;
		SC_THREAD(inject_application);
		
	}
	~ApplicationInjector(){}

	SC_HAS_PROCESS(ApplicationInjector);
	int mapped_application_number;
	int tried_application_number;

	

	sc_core::sc_event e_inject;
private:
	struct App {
		int	   Id;
		char*  Name;
		char*  FName;
	};
	App app;
//	void inject_application(App app) {
	void inject_application() {
		
		while(1) {
			wait(e_inject);
			wait(sc_core::sc_time(2, sc_core::SC_NS));
			int t = 0;
		
			while (1) {
				down_layer->Report();
				std::cout << " t " << t++ << std::endl;
				int i_index = rand()%NO_OF_ROW_CONST;
				int j_index = rand()%NO_OF_ROW_CONST;
				sync_enum_type stat_map = down_layer->nb_external_transport_fw(i_index, j_index, app.FName);
				if (stat_map == tlm::TLM_COMPLETED) {
					outputLogFile << "Task Manager Mapped " << std::endl;
					//e_inject.notify(sc_core::sc_time(100000, sc_core::SC_NS));
					break;
				}
				else {
					outputLogFile << "Task Manager not Mapped " << std::endl;
				}
			}
		}
	}
public: 	
	void map_applications(char* app_file_name) {
		std::ifstream fileHandler(app_file_name, std::ifstream::in);
		const int MAX_LENGTH = 100;
		char line[MAX_LENGTH];
		char* token;

		
		if (fileHandler.good()) {
			outputLogFile << " Hello World! from Application Injector" << std::endl;
			
			while (!fileHandler.eof()) {
				fileHandler.getline(line, MAX_LENGTH);
				token = strtok(line, " ");
				while(token != NULL) {
					if (!strcmp(token, "--")) {
						break;
					} else if (!strcmp(token, "Application")) {
							
						break;
					} else if (!strcmp(token, "ID")) {
						token  = strtok(NULL, " ");
						app.Id = atoi(token);
						break;
					} else if (!strcmp(token, "Name")) {
						token  = strtok(NULL, " ");
						app.Name = new char[MAX_LENGTH];
						strcpy(app.Name, token);
						break;
					} else if (!strcmp(token, "FName")) {
						token  = strtok(NULL, " ");
						app.FName = new char[MAX_LENGTH];
						strcpy(app.FName, token);
						//inject_application(app);
						e_inject.notify(sc_core::sc_time(3, sc_core::SC_NS));
						break;
					}  else if (!strcmp(token, "END")) {
						break;
					} else
						break;
				}
			}
		}
	}
	
	virtual sync_enum_type nb_transport_bw(int input_ch, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& delay) {
		tried_application_number++;
		
		trafficInRouterLogFile << " tried_application_number = " << tried_application_number << " mapped_application_number = " << mapped_application_number <<std::endl;
		down_layer->Report();
		

		tlm_noc::noc_message_payload* p_message_trans_ext;
        r_phit_trans.get_extension(p_message_trans_ext);
		if (noc_gp_hdr(p_message_trans_ext, m_cmd) == tlm_noc::APPLICATION_MAPPED)
			mapped_application_number++;

		std::cout << " tried_application_number = " << tried_application_number << " mapped_application_number = " << mapped_application_number <<std::endl;
		trafficInRouterLogFile << " noc_gp_hdr(p_message_trans_ext, m_cmd) = " << noc_gp_hdr(p_message_trans_ext, m_cmd) << std::endl;
			if (tried_application_number < 30)
				map_applications("tasks\\applications\\applications.app");

//		} else if (noc_gp_hdr(p_message_trans_ext, m_cmd) = tlm_noc::APPLICATION_MAPPED_FAILED) {
 			//map_applications("tasks\\applications\\applications.app");
//		}


		return tlm::TLM_ACCEPTED;
	}



	void initialization() {
		int initial_task_mapped[8][8] = { {2, 2, 2, 1, 2, 1, 2, 2}, 
									      {1, 2, 2, 2, 2, 2, 2, 2},
										  {1, 2, 2, 0, 2, 2, 2, 2},
									      {2, 2, 2, 1, 2, 2, 2, 2},
										  {2, 2, 1, 2, 2, 2, 0, 2},
										  {2, 2, 2, 2, 2, 2, 2, 2},
										  {0, 2, 1, 2, 0, 2, 2, 1},
										  {0, 2, 2, 0, 2, 2, 2, 2}};

		for (int i = 0; i < NO_OF_ROW_CONST; i++)
			for (int j = 0; j < NO_OF_COL_CONST; j++)
				for ( int k = 0; k < initial_task_mapped[i][j]; k++)
					down_layer->tile[i][j]->m_Interface->mTransportLayer->reconfigurable_area[k].activity = ACTIVE;
 

	}

// Variables
public:
	NoRC*	    down_layer;
	

};

#endif /* APPLICATIONINJECTOR_H */
