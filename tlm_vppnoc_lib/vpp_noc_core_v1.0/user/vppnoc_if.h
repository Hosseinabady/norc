#ifndef VPPNOC_IF_H
#define VPPNOC_IF_H


#include "core/vppnoc_types.h"


namespace vppnoc {


template<unsigned int NOOFSOCKET = 4>
class 
vppnoc_if 
{
public:
	virtual ~vppnoc_if() 
  {
  }

	virtual 
    vppnoc_sync_enum_type 
    send
    ( int socket_id
    , vppnoc_transaction& trans
    , vppnoc_phase & phase
    , sc_core::sc_time & delay) 
    = 0;
  
  virtual 
    vppnoc_sync_enum_type 
    read
    ( int socket_id
    , vppnoc_transaction& trans
    , vppnoc_phase & phase
    , sc_core::sc_time & delay) 
    = 0;
  
  virtual 
    vppnoc_sync_enum_type 
    write
    ( int socket_id
    , vppnoc_transaction& trans
    , vppnoc_phase & phase
    , sc_core::sc_time & delay) 
    = 0;
    
    unsigned int 
      get_link_width_bits
      () const 
    {
		  return (NOOFSOCKET);
    }
   
};

} 

#endif  //VPPNOC_IF_H
