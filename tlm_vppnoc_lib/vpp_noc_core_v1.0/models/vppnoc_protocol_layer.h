/*
 * norc_protocol_layer.h
 *
 *  Created on: 27-Mar-2009
 *      Author: csxmh
 */
#pragma once
#ifndef NORC_PROTOCOL_LAYER_H_
#define NORC_PROTOCOL_LAYER_H_

#include "core/vppnoc_types.h"
#include "core/vppnoc_core_ifs.h"
#include "include/vppnoc_utilities.h"
#include "tasks/vppnoc_application.h"
#include "models/vppnoc_abstract_module.h"

namespace vppnoc {

class NoCProtocolLayer : public vppnoc_module {

public:
	NoCProtocolLayer(sc_core::sc_module_name name, int id, TileTypeEnum ty) :
		vppnoc_module(name, id, ty)
	{

	}


};
}
#endif /* NORC_PROTOCOL_LAYER_H_ */
