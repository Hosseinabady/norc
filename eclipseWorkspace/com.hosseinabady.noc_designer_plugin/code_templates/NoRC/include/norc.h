#ifndef NORC_H
#define NORC_H
#include <systemc>
#include <model/tasks/include/noc_task.h>
#include <list>
#include "tlm.h"
#include <model/include/modelutilities.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "NoRC/tile/include/rtr_tile.h"
#include "NoRC/tile/include/dummy_tile.h"
#include <sstream>
#include "string"
#include "tasks/include/taskmanager.h"
#include "model/tasks/include/noc_task.h"
#include "model/model.h"
#include "model/modules/include/noc_connector.h"
#include <stdlib.h>



class NoRC : public sc_core::sc_module {

public:
	NoRC(sc_core::sc_module_name name);
	~NoRC(){}
	sync_enum_type nb_external_transport_fw(int i_index, int j_index, char*  FName);

public:
	RTRTile<TILE_TEMPLATE_PARAMETER  >***		                 tile;
	void Report();
private:
	Router<ROUTER_TEMPLATE_PARAMETER>***                         router;
	RTRTile<TILE_TEMPLATE_PARAMETER  >*                          dummy_tile[2*(NO_OF_ROW_CONST+NO_OF_COL_CONST)];

	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >***   router_connector;
	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >*     dummy_tile_connector;
	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >**    rtr_tile_connector;
};
#endif //NORC_H