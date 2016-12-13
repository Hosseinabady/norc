/*
 * processing_element.h
 *
 *  Created on: 27-Mar-2009
 *      Author: csxmh
 */

#ifndef NOC_PROCESSING_ELEMENT_H_
#define NOC_PROCESSING_ELEMENT_H_

class NoCTile : public NoCAbstractModule {
	NoCTile(sc_core::sc_module_name name) :sc_module(name) {}
};

#endif /* NOC_PROCESSING_ELEMENT_H_ */
