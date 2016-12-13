

#include "tlm.h"

#include "NoRC/utilities/include/utilities.h"
#include "NoRC/router/include/router.h"
#include "NoRC/tile/include/rtr_tile.h"
#include "NoRC/tile/include/dummy_tile.h"
#include <sstream>
#include "string"
#include "tasks/include/taskmanager.h"
#include "model/tasks/include/noc_task.h"
#include "model/model.h"
#include "model/modules/include/noc_connector.h"
#include <stdlib.h>
#include "NoRC/include/norc.h"
#include "offchip/include/application_injector.h"
#include "model/modules/include/noc_connector.h"



std::ofstream  outputLogFile;
std::ofstream  trafficLogFile;
std::ofstream  trafficInRouterLogFile;
std::ofstream  fatal_error_log;



int sc_main(int argc, char* argv[])
{
	srand ( (unsigned int)time(NULL) );

	char* app_file_name ="tasks\\applications\\applications.app";
	trafficLogFile.open("trafficLog.txt");
	outputLogFile.open("outputLog.txt");
	trafficInRouterLogFile.open("trafficInRouterLogFile.txt");
	fatal_error_log.open("fata_error.log");

	NoRC  norc_chip("norc_chip");
	ApplicationInjector appInj_chip("appInj_chip");
	appInj_chip.down_layer = &norc_chip;

	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >***   transport_external_connector;
	transport_external_connector = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >**[NO_OF_ROW_CONST];
	for (int i = 0; i < NO_OF_ROW_CONST; i++) {
		transport_external_connector[i] = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >*[NO_OF_COL_CONST];
		for (int j = 0; j < NO_OF_COL_CONST; j++) {
			transport_external_connector[i][j] = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >[1];
		}
	}
	for (int i = 0; i < NO_OF_ROW_CONST; i++)
	  for (int j = 0; j < NO_OF_COL_CONST; j++) {
		norc_chip.tile[i][j]->m_Interface->mTransportLayer->m_external_port = &transport_external_connector[i][j][0];
		transport_external_connector[i][j][0].setHead(&appInj_chip, 0);
	  }

	appInj_chip.map_applications(app_file_name);

	sc_core::sc_start();
	sc_core::sc_stop();

	system("PAUSE");
	return 0;
}
