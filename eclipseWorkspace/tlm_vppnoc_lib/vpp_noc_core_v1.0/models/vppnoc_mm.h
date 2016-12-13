#ifndef VPPNOC_MM_H
#define VPPNOC_MM_H


namespace vppnoc {
  template<typename TYPE_TRANS=vppnoc_protocol_types, typename TYPE_PDU=vppnoc_pdu_base>
class vppnoc_mm : public vppnoc_mm_interface
{
public:
  typedef TYPE_TRANS gp_trans;
  typedef TYPE_PDU   gp_pdu;

  
  vppnoc_mm() : free_list(0), empties(0) {}

  gp_trans* allocate() {
	  gp_trans* ptr;
	  if (free_list)
	  {
		  ptr = free_list->trans;
		  empties = free_list;
		  free_list = free_list->next;
	  }
	  else
	  {
      ptr = new gp_trans(this);
      ptr->m_pdu = new gp_pdu;
	  }
      return ptr;
    }
   	
    void  free(gp_trans* trans) {
    	//trans->reset(); // Delete auto extensions
    	//trans->free_pdu();

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
    gp_trans* trans;
    access* next;
    access* prev;
  };

  access* free_list;
  access* empties;
};


}
#endif //VPPNOC_MM_H