#ifndef VPPNOC_CORE_IFS_H
#define VPPNOC_CORE_IFS_H



#include <systemc>
#include "core/vppnoc_types.h"

#include "include/vppnoc_utilities.h"
using namespace sc_core;

namespace vppnoc {

/* Forwards */

template<typename MODULE, unsigned int NOOFSOCKET>
class vppnoc_master_socket;
template<typename MODULE, unsigned int NOOFSOCKET>
class vppnoc_slave_socket;



class vppnoc_fw_transport_if: public virtual sc_core::sc_interface {

    /* Transport interface */

    /* Friends */
    template<typename MODULE, unsigned int NOOFSOCKET> friend class vppnoc_slave_socket;

    virtual vppnoc_sync_enum_type
        nb_transport_fw(int socket_id,
                        vppnoc_transaction& trans
                      , vppnoc_phase & phase
                      , sc_core::sc_time & delay) = 0;

};


class vppnoc_bw_transport_if: public virtual sc_core::sc_interface {

    
	/* Friends */
    template<typename MODULE,unsigned int NOOFSOCKET> friend class vppnoc_master_socket;

    virtual 
    vppnoc_sync_enum_type
    nb_transport_bw( int socket_id
                   , vppnoc_transaction& trans
                   , vppnoc_phase & phase
                   , sc_time & delay) = 0; 
};



////////////////////////////////////////////////////////////////////////////
// Basic interfaces
////////////////////////////////////////////////////////////////////////////
template <typename TRANS = vppnoc_protocol_types,
          typename PHASE = vppnoc_phase_enum>
class vppnoc_tlm_fw_nonblocking_transport_if : public virtual sc_core::sc_interface {
public:
  virtual vppnoc_sync_enum nb_transport_fw( TRANS& trans
                                          , vppnoc_phase & phase
                                          , sc_time& delay) = 0;
};

template <typename TRANS = vppnoc_protocol_types,
          typename PHASE = vppnoc_phase_enum>
class vppnoc_tlm_bw_nonblocking_transport_if : public virtual sc_core::sc_interface {
public:
  virtual vppnoc_sync_enum nb_transport_bw( TRANS& trans
                                          , vppnoc_phase & phase
                                          , sc_core::sc_time& delay) = 0;
};









// The forward interface:
template <typename TYPES = vppnoc_protocol_types>
class vppnoc_tlm_fw_transport_if
  : public virtual vppnoc_tlm_fw_nonblocking_transport_if<typename TYPES::vppnoc_payload_type,
                                                          typename TYPES::vppnoc_phase_type>
  
{};

// The backward interface:
template <typename TYPES = vppnoc_protocol_types>
class vppnoc_tlm_bw_transport_if
  : public virtual vppnoc_tlm_bw_nonblocking_transport_if<typename TYPES::vppnoc_payload_type,
                                                          typename TYPES::vppnoc_phase_type>
  
{};

}   /* namespace vppnoc */




#endif  /* defined(VPPNOC_CORE_IFS_H) */


