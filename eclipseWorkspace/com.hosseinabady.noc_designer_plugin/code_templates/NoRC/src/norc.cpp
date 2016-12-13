

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




NoRC::NoRC(sc_core::sc_module_name name) : sc_core::sc_module(name) {
	
	router_connector = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >**[NO_OF_ROW_CONST];
	for (int i = 0; i < NO_OF_ROW_CONST; i++) {
		router_connector[i] = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >*[NO_OF_COL_CONST];
		for (int j = 0; j < NO_OF_COL_CONST; j++) {
			router_connector[i][j] = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >[NR_OF_ROUTER_PORT_CONST];
		}
	}

	dummy_tile_connector = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >[2*(NO_OF_ROW_CONST+NO_OF_COL_CONST)];

	rtr_tile_connector = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >*[NO_OF_ROW_CONST];
	tile = new RTRTile<TILE_TEMPLATE_PARAMETER  >**[NO_OF_ROW_CONST];
	router = new Router<ROUTER_TEMPLATE_PARAMETER>**[NO_OF_ROW_CONST];

	for (int i = 0; i < NO_OF_ROW_CONST; i++) {
		rtr_tile_connector[i] = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >[NO_OF_COL_CONST];
		tile[i]   = new RTRTile<TILE_TEMPLATE_PARAMETER  >*[NO_OF_COL_CONST];
	    router[i] = new Router<ROUTER_TEMPLATE_PARAMETER>*[NO_OF_COL_CONST];

	}


  for (int i = 0; i < 2*(NO_OF_ROW_CONST+NO_OF_COL_CONST); i++) {
	  std::ostringstream dummy_tile_name;
	  dummy_tile_name << "Dummy_Tile"  << i+1;
	   dummy_tile[i] = new RTRTile<TILE_TEMPLATE_PARAMETER  >(dummy_tile_name.str().c_str() ,i+1, DUMMY_TILE);
	  dummy_tile[i]->m_port = &dummy_tile_connector[i];
  }

  for (int i = 0; i < NO_OF_ROW_CONST; i++) {
	  for (int j = 0; j < NO_OF_COL_CONST; j++) {
		  std::ostringstream tile_name;
		  std::ostringstream router_name;

		  tile_name << "Tile"  << i*NO_OF_COL_CONST+j+1;
		  router_name << "Router"  << i*NO_OF_COL_CONST+j+1;

		  if (i == 0 && j == 0)
			  tile[i][j]   = new   RTRTile<TILE_TEMPLATE_PARAMETER>(tile_name.str().c_str(), i*NO_OF_COL_CONST+j+1, INTER_TILE);
		  else
			  tile[i][j]   = new   RTRTile<TILE_TEMPLATE_PARAMETER>(tile_name.str().c_str(),  i*NO_OF_COL_CONST+j+1, INTER_TILE);

		  tile[i][j]->m_port = &rtr_tile_connector[i][j];
		  router[i][j] = new   Router<ROUTER_TEMPLATE_PARAMETER>(router_name.str().c_str(), i*NO_OF_COL_CONST+j+1);
	  }
  }

  int i, j;
  int k = 0;
//-----------------------------------------------------------------------------
  for (i = 0; i < NO_OF_ROW_CONST; i++)
	  for (j = 0; j < NO_OF_COL_CONST; j++) {
		  for (k = 0; k < NR_OF_ROUTER_PORT_CONST; k++)
			router[i][j]->m_port[k] = &router_connector[i][j][k];
	  }
//-----------------------------------------------------------------------------
  k = 0;
  for (i = 0; i < NO_OF_ROW_CONST; i++) {
  	  for (j = 0; j < NO_OF_COL_CONST; j++) {

  		  if (j+1 < NO_OF_COL_CONST) {
			  router_connector[i][j][RIGHT_PORT].setHead(router[i][j+1], LEFT_PORT);
			  router_connector[i][j+1][LEFT_PORT].setHead(router[i][j], RIGHT_PORT);
  		  } else {
			router_connector[i][j][RIGHT_PORT].setHead(dummy_tile[k], 0);
			dummy_tile_connector[k].setHead(router[i][j], RIGHT_PORT);
  			k++;
		  }

  		  if (i+1 < NO_OF_ROW_CONST) {
    		router_connector[i][j][DOWN_PORT].setHead(router[i+1][j], UP_PORT);
			router_connector[i+1][j][UP_PORT].setHead(router[i][j], DOWN_PORT);
  		  } else {
			router_connector[i][j][DOWN_PORT].setHead(dummy_tile[k], 0);
			dummy_tile_connector[k].setHead(router[i][j], DOWN_PORT);
  			k++;
  		  }

  		  if (i == 0) {
			router_connector[i][j][UP_PORT].setHead(dummy_tile[k], 0);
			dummy_tile_connector[k].setHead(router[i][j], UP_PORT);
  			k++;
  		  }

  		  if (j == 0) {
			router_connector[i][j][LEFT_PORT].setHead(dummy_tile[k], 0);
			dummy_tile_connector[k].setHead(router[i][j], LEFT_PORT);
  			k++;
  		  }

 		  router_connector[i][j][LOCAL_PORT].setHead(tile[i][j], 0);
		  rtr_tile_connector[i][j].setHead(router[i][j], LOCAL_PORT);
	  }
  }

  std::cout << "K =" << k << std::endl;
  std::cout << "I =" << i << std::endl;

}

sync_enum_type NoRC::nb_external_transport_fw(int i_index, int j_index, char*  FName) {
	return tile[i_index][j_index]->nb_external_transport_fw(FName);
}

void NoRC::Report() {

	for (int i = 0; i < NO_OF_ROW_CONST; i++) {
		for (int j = 0; j < NO_OF_COL_CONST; j++) { 
			router[i][j]->Report();
		}
	}
	trafficInRouterLogFile << std::endl;
	trafficInRouterLogFile << std::endl;
	trafficInRouterLogFile << std::endl;
	for (int i = 0; i < NO_OF_ROW_CONST; i++) {
		for (int j = 0; j < NO_OF_COL_CONST; j++) { 
			int tmpsum = 0;
			for (int k = 0; k < NO_OF_RECONFIGURABLE_CORE; k++)
				tmpsum += tile[i][j]->m_Interface->mTransportLayer->reconfigurable_area[k].activity;
			trafficInRouterLogFile << tmpsum;
		}
		trafficInRouterLogFile << std::endl;
	}




}