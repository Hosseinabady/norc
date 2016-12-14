#ifndef VPPNOC_MASTER_COCKET_H
#define VPPNOC_MASTER_COCKET_H

#include "user/vppnoc_if.h"
#include "sockets/vppnoc_socket_base.h"
#include "sockets/vppnoc_simple_initiator_socket.h"
#include "sockets/vppnoc_simple_target_socket.h"


namespace vppnoc {
using namespace sc_core;
using namespace sc_dt;
using namespace std;

template
<typename MODULE=vppnoc_bw_transport_if, unsigned int NOOFSOCKET = 4>
class 
vppnoc_master_socket
: public virtual vppnoc_if<NOOFSOCKET>
, public vppnoc_socket_base
, public vppnoc_simple_initiator_socket_tagged< MODULE
                                                       , NOOFSOCKET
                                                       , vppnoc_protocol_types> 
{
public:
  typedef vppnoc_simple_initiator_socket_tagged< MODULE
                                                , NOOFSOCKET
                                                , vppnoc_protocol_types> base_type;
            

    
  vppnoc_master_socket();
  explicit vppnoc_master_socket(const char *, int = 0);

  virtual const char * kind() const;

	// Convenience interface
	virtual 
  vppnoc_sync_enum_type 
  send
  ( int socket_id
  , vppnoc_transaction& trans
  , vppnoc_phase & phase
  , sc_time & delay
  );
	
  vppnoc_sync_enum_type 
  send
  ( vppnoc_transaction& trans
  , vppnoc_phase & phase
  , sc_time & delay
  );

  virtual 
  vppnoc_sync_enum_type 
  read
  ( int socket_id
  , vppnoc_transaction& trans
  , vppnoc_phase & phase
  , sc_time & delay);
	
  vppnoc_sync_enum_type 
  read
  ( vppnoc_transaction& trans
  , vppnoc_phase & phase
  , sc_time & delay );

  virtual vppnoc_sync_enum_type write(int socket_id,
                                       vppnoc_transaction& trans,
                                       vppnoc_phase & phase,
                                       sc_time & delay);
	vppnoc_sync_enum_type write( vppnoc_transaction& trans
                             , vppnoc_phase & phase
                             , sc_time & delay);


	// Blocking transport 
        vppnoc_sync_enum_type nb_transport_fw(int, vppnoc_transaction&, vppnoc_phase &,sc_time &);
        vppnoc_sync_enum_type nb_transport_fw(vppnoc_transaction&, vppnoc_phase &, sc_time &);

        vppnoc_sync_enum_type nb_transport_bw(int, vppnoc_transaction&, vppnoc_phase &,sc_time &);
        vppnoc_sync_enum_type nb_transport_bw(vppnoc_transaction&, vppnoc_phase &, sc_time &);

	// Bindings 
        using base_type::bind;
        using base_type::operator();
        void bind(MODULE &);
        void operator() (MODULE &);
        void operator() ( MODULE & iface, sync_enum_type (MODULE::*cb)( int, transaction_type&
                                              , vppnoc_phase & phase
                                              , sc_time&)) ;
//        void operator() (vppnoc_initiator_socket<NOOFSOCKET> & iface);
//        void operator() (vppnoc_target_socket<NOOFSOCKET> & iface);


};

	//Function implementation 

template<typename MODULE, unsigned int NOOFSOCKET>
inline
vppnoc_master_socket<MODULE, NOOFSOCKET>::vppnoc_master_socket()
:
base_type(sc_gen_unique_name("vppnoc_master_socket"))
    {
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline
vppnoc_master_socket<MODULE, NOOFSOCKET>::vppnoc_master_socket(const char * name,
                                                       int socket_id /* = 0 */):
    vppnoc_socket_base(socket_id)
    //,base_type(name) 
    {
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline const char *
vppnoc_master_socket<MODULE, NOOFSOCKET>::kind() const  {
    return ("vppnoc_master_socket");
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline vppnoc_sync_enum_type 
	vppnoc_master_socket<MODULE, NOOFSOCKET>::send(int socket_id
                                                , vppnoc_transaction& trans
                                                , vppnoc_phase & phase
                                                , sc_time & delay) {
	return nb_transport_fw(socket_id, trans, phase, delay);
}

template<typename MODULE, unsigned int NOOFSOCKET>
inline vppnoc_sync_enum_type 
	vppnoc_master_socket<MODULE, NOOFSOCKET>::send(vppnoc_transaction& trans
                                                , vppnoc_phase & phase
                                                , sc_time & delay) {
	return send(0, trans, phase, delay);
}

template<typename MODULE, unsigned int NOOFSOCKET>
inline vppnoc_sync_enum_type 
	vppnoc_master_socket<MODULE, NOOFSOCKET>::read(int socket_id
                                                , vppnoc_transaction& trans
                                                , vppnoc_phase & phase
                                                , sc_time & delay) {
	return nb_transport_fw(socket_id, trans, phase, delay);
}
template<typename MODULE, unsigned int NOOFSOCKET>
inline vppnoc_sync_enum_type 
	vppnoc_master_socket<MODULE, NOOFSOCKET>::read(vppnoc_transaction& trans
                                                , vppnoc_phase & phase
                                                , sc_time & delay) {
	return send(0, trans, phase, delay);
}

template<typename MODULE, unsigned int NOOFSOCKET>
inline vppnoc_sync_enum_type 
	vppnoc_master_socket<MODULE, NOOFSOCKET>::write(int socket_id
                                                 , vppnoc_transaction& trans
                                                 , vppnoc_phase & phase
                                                 , sc_time & delay) {
	return nb_transport_fw(socket_id, trans, phase, delay);
}
template<typename MODULE, unsigned int NOOFSOCKET>
inline vppnoc_sync_enum_type 
	vppnoc_master_socket<MODULE, NOOFSOCKET>::write( vppnoc_transaction& trans
                                                 , vppnoc_phase & phase
                                                 , sc_time & delay) {
	return send(0, trans, phase, delay);
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline 
vppnoc_sync_enum_type
vppnoc_master_socket<MODULE, NOOFSOCKET>::nb_transport_fw
( int socket_id
 , vppnoc_transaction& trans
 , vppnoc_phase & phase
 , sc_time & delay
 ) 
{
    return (* this)->nb_transport_fw(trans, phase, delay);
}

template<typename MODULE, unsigned int NOOFSOCKET>
inline vppnoc_sync_enum_type
vppnoc_master_socket<MODULE, NOOFSOCKET>::nb_transport_bw( vppnoc_transaction& trans
                                              , vppnoc_phase & phase
                                              , sc_time & delay) {
    return nb_transport_bw(0, trans, phase, delay);
}

template<typename MODULE, unsigned int NOOFSOCKET>
inline vppnoc_sync_enum_type
vppnoc_master_socket<MODULE, NOOFSOCKET>::nb_transport_bw(int socket_id,
                                             vppnoc_transaction& trans
                                             , vppnoc_phase & phase
                                             , sc_time & delay) {
    return (* this)->nb_transport_bw(trans, phase, delay);
}

template<typename MODULE, unsigned int NOOFSOCKET>
inline vppnoc_sync_enum_type
vppnoc_master_socket<MODULE, NOOFSOCKET>::nb_transport_fw( vppnoc_transaction& trans
                                              , vppnoc_phase & phase
                                              , sc_time & delay) {
    return nb_transport_fw(0, trans, phase, delay);
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline void
vppnoc_master_socket<MODULE, NOOFSOCKET>::bind(MODULE & iface) {

    register_nb_transport_bw(& iface,
                             & MODULE::nb_transport_bw,
                             this->get_socket_id());
}


template<typename MODULE, unsigned int NOOFSOCKET>
inline void
vppnoc_master_socket<MODULE, NOOFSOCKET>::operator() (MODULE & iface) {
    bind(iface);
}

template<typename MODULE, unsigned int NOOFSOCKET>
inline void
vppnoc_master_socket<MODULE, NOOFSOCKET>
::operator() 
( MODULE & iface
, sync_enum_type (MODULE::*cb)( int, transaction_type&
                                              , vppnoc_phase & phase
                                              , sc_time&)) {
   register_nb_transport_bw(& iface,
                            cb,
                            this->get_socket_id());

}

//template<typename MODULE, unsigned int NOOFSOCKET>
//inline void
//vppnoc_master_socket<MODULE, NOOFSOCKET>::operator() (vppnoc_initiator_socket<NOOFSOCKET> & iface) {
//   bind(iface);
//}

//template<typename MODULE, unsigned int NOOFSOCKET>
//inline void
//vppnoc_master_socket<MODULE, NOOFSOCKET>::operator() (vppnoc_target_socket<NOOFSOCKET> & iface) {
//   bind(iface);
//}

}


#endif //VPPNOC_MASTER_COCKET_H