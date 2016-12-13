#ifndef VPPNOC_SLAVE_SOCKET__H
#define VPPNOC_SLAVE_SOCKET__H

#include "core/vppnoc_core_ifs.h"
#include "sockets/vppnoc_socket_base.h"
#include "sockets/vppnoc_simple_target_socket.h"


namespace vppnoc {

template<typename MODULE=vppnoc_fw_transport_if, unsigned int NOOFSOCKET = 4>
class vppnoc_slave_socket:
public vppnoc_socket_base,
public vppnoc_simple_target_socket_tagged< MODULE
                                         , NOOFSOCKET
                                         , vppnoc_protocol_types> 
{


    public:
		//Typedefine
        typedef
            vppnoc_simple_target_socket_tagged<MODULE,
                                                       NOOFSOCKET,
                                                       vppnoc_protocol_types>            base_type;

    // Constructor 
        vppnoc_slave_socket();
        explicit vppnoc_slave_socket(const char *, int = 0);

    /* sc_object overridables */
        virtual const char * kind() const;

    
    /* Bindings */
        using base_type::bind;
        using base_type::operator();
        void bind(MODULE &);
        void operator() (MODULE &);
        void operator() 
        ( MODULE & iface
        , sync_enum_type (MODULE::*cb)( int, transaction_type&
                                                      , vppnoc_phase & 
                                                      , sc_time&));

};

/* Functions */


template<typename MODULE, unsigned int NOOFSOCKET>
inline
vppnoc_slave_socket<MODULE, NOOFSOCKET>::vppnoc_slave_socket():
    base_type(sc_core::sc_gen_unique_name("vppnoc_slave_socket")) {
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline
vppnoc_slave_socket<MODULE, NOOFSOCKET>::vppnoc_slave_socket(const char * name,
                                                     int socket_id /* = 0 */):
    vppnoc_socket_base(socket_id),
    base_type(name) {
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline const char *
vppnoc_slave_socket<MODULE, NOOFSOCKET>::kind() const  {
    return ("vppnoc_slave_socket");
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline void
vppnoc_slave_socket<MODULE, NOOFSOCKET>::bind(MODULE & iface) {
    register_nb_transport_fw(& iface,
                             & MODULE::nb_transport_fw,
                             this->get_socket_id());
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline void
vppnoc_slave_socket<MODULE, NOOFSOCKET>::operator() (MODULE & iface) {
    bind(iface);
}

template<typename MODULE, unsigned int NOOFSOCKET>
inline void
vppnoc_slave_socket<MODULE, NOOFSOCKET>
::operator() 
( MODULE & iface
, sync_enum_type (MODULE::*cb)( int, transaction_type&
                                              , vppnoc_phase & phase
                                              , sc_time& )) {
   register_nb_transport_fw(& iface,
                            cb,
                            this->get_socket_id());

}


}   

#endif  // VPPNOC_SLAVE_SOCKET__H 
