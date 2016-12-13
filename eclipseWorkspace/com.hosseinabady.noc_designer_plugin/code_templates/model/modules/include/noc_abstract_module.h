/*
 * norc_abstract_module.h
 *
 *  Created on: 27-Mar-2009
 *      Author: csxmh
 */

#ifndef NOC_ABSTRACT_MODULE_H_
#define NOC_ABSTRACT_MODULE_H_


#include "model/include/modelutilities.h"




class NoCAbstractModule : public sc_core::sc_module {



public:
	NoCAbstractModule(sc_core::sc_module_name name, int i) : sc_core::sc_module(name), id(i) {

	}
	void setLocation(NoCPosition p) {
		this->location = p;
	}

	NoCPosition getLocation() {
		return this->location;
	}

	void setId(int id) {
		this->id = id;
	}

	int getId() {
		return this->id;
	}

private:
	NoCPosition location;
	int id;
	int type;
};


#endif /* NOC_ABSTRACT_MODULE_H_ */
