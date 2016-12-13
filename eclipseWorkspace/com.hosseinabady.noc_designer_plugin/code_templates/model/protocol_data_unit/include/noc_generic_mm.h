#ifndef NOC_GENERIC_MM_H
#define NOC_GENERIC_MM_H

#include "tlm.h"
namespace tlm_noc {

template <typename TYPE_PAYLOAD>
class NoRCGenericMM 
{
  typedef TYPE_PAYLOAD gp_t;

public:
  NoRCGenericMM() : free_list(0), empties(0) {}

  gp_t* allocate() {
	gp_t* ptr;
	if (free_list)
	{
		ptr = free_list->trans;
		empties = free_list;
		free_list = free_list->next;
	}
	else
	{
	    ptr = new gp_t(this);
	}
    return ptr;
  }
   	
  void  free(gp_t* trans) {
	trans->reset(); // Delete auto extensions
	if (!empties)
	{
	    empties = new access;
	    empties->next = free_list;
	    empties->prev = 0;
	    if (free_list)
			free_list->prev = empties;
	}
	free_list = empties;
	free_list->trans = trans;
	empties = free_list->prev;
  }
  

private:
  struct access
  {
    gp_t* trans;
    access* next;
    access* prev;
  };

  access* free_list;
  access* empties;
};


}
#endif //NOC_GENERIC_MM_H