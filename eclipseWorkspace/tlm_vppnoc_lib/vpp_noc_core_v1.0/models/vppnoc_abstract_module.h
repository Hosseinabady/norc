/*
 * norc_abstract_module.h
 *
 *  Created on: 27-Mar-2009
 *      Author: csxmh
 */
#pragma once
#ifndef NOC_ABSTRACT_MODULE_H_
#define NOC_ABSTRACT_MODULE_H_

#include "systemc"
#include "core/vppnoc_core_ifs.h"
#include "models/vppnoc_module_utilities.h"

using namespace sc_core;
namespace vppnoc {


class  
vppnoc_module 
: public virtual vppnoc_fw_transport_if
, public virtual vppnoc_bw_transport_if
, public sc_core::sc_module {

public:
	vppnoc_module
  ( sc_module_name name
  , int i
  , TileType typ
  ) 
  : 
    sc_module(name)
  , id(i)
  , type(typ) 
  {
	}
	void setLocation(NoCPosition p) { this->location = p;}
	NoCPosition getLocation() {	return this->location; }

	void set_id(int id) { this->id = id;}
	int get_id() { return this->id; }

	void set_type(TileType ty) { this->type = ty;}
	TileType get_type() { return this->type; }

private:
	NoCPosition location;
	int id;
	TileType type;
};

}
#endif /* NOC_ABSTRACT_MODULE_H_ */
