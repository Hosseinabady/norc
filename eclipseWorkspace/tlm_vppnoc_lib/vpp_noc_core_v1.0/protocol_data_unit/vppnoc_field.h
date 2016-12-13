#pragma once
#ifndef VPPNOC_FIELD_H
#define VPPNOC_FIELD_H

namespace vppnoc {

template <typename TYPE, unsigned int LENGTH>
class vppnoc_field {
public:
	void set_field(TYPE fldval) 
	{  
		unsigned int intval = (unsigned int)fldval;
		int i = 0;
		for (; i < LENGTH; i++)
			field[i] = 0;
		i = 0;
		while(intval > 0) {
			assert (i < LENGTH);
			field[i++] = intval%2;
			intval = intval/2;     
		}
	}
	TYPE get_field() 
	{
  	unsigned int    int_value = 0;
		for (int i = 0; i < LENGTH; i++) {
			int_value = int_value*2+field[LENGTH-(i+1)];
		}
		return (TYPE)int_value;
	}
private:
	bool   field[LENGTH];

};

}
#endif //VPPNOC_FIELD_H