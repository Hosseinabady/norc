/*
 * processing_element.h
 *
 *  Created on: 27-Mar-2009
 *      Author: csxmh
 */

#ifndef NOC_TILE_H_
#define NOC_TILE_H_
#include "model/modules/include/noc_abstract_module.h"
#include "NoRC/utilities/include/utilities.h"




class NoCTile : public vppnoc_module {
public:
	NoCTile(sc_core::sc_module_name name, int id, TileTypeEnum type) :vppnoc_module(name, id) {}



private:

};

#endif /* NOC_TILE_H_ */
