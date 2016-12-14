#ifndef VPPNOC_INITIATOR_SOCKET_H
#define VPPNOC_INITIATOR_SOCKET_H

#include <systemc>
#include "core/vppnoc_core_ifs.h"
#include "core/vppnoc_types.h"

namespace vppnoc {

template <unsigned int NOOFSOCKET = 32,
          typename FW_IF = vppnoc_tlm_fw_transport_if<TYPES>,
          typename BW_IF = vppnoc_tlm_bw_transport_if<TYPES> >
class vppnoc_base_initiator_socket_b
{
public:
  virtual ~vppnoc_base_initiator_socket_b() {}
	
  virtual sc_core::sc_port_b<FW_IF> & get_base_port() = 0;
  virtual                    BW_IF  & get_base_interface() = 0;
  virtual sc_core::sc_export<BW_IF> & get_base_export() = 0;
};

template <unsigned int NOOFSOCKET,
          typename FW_IF,
          typename BW_IF> class vppnoc_base_target_socket_b;


template <unsigned int NOOFSOCKET,
          typename FW_IF,
          typename BW_IF,
          int N,
          sc_core::sc_port_policy POL > class vppnoc_base_target_socket;

template <unsigned int NOOFSOCKET = 4,
          typename FW_IF = vppnoc_tlm_fw_transport_if<TYPES>,
          typename BW_IF = vppnoc_tlm_bw_transport_if<TYPES>,
          int N = 1,
          sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND
          >
class vppnoc_base_initiator_socket : public vppnoc_base_initiator_socket_b<NOOFSOCKET, FW_IF, BW_IF>, 
                                     public sc_core::sc_port<FW_IF, N, POL>
{
public:
  typedef FW_IF                                         fw_interface_type;
  typedef BW_IF                                         bw_interface_type;
  typedef sc_core::sc_port<fw_interface_type, N, POL>   port_type;
  typedef sc_core::sc_export<bw_interface_type>         export_type;
  typedef vppnoc_base_target_socket<NOOFSOCKET,
                                 fw_interface_type,
                                 bw_interface_type,
                                 N,
                                 POL
                                >                       target_socket_type;

  typedef vppnoc_base_target_socket_b<NOOFSOCKET,
                                   fw_interface_type,
                                   bw_interface_type>   base_target_socket_type;                                  
  typedef vppnoc_base_initiator_socket_b<NOOFSOCKET,
                                      fw_interface_type,
                                      bw_interface_type> base_type;

  template <unsigned int, typename, typename, int ,sc_core::sc_port_policy>
  friend class vppnoc_base_target_socket;

public:
  vppnoc_base_initiator_socket()
  : port_type(sc_core::sc_gen_unique_name("vppnoc_base_initiator_socket"))
  , m_export(sc_core::sc_gen_unique_name("vppnoc_base_initiator_socket_export"))
  {
  }

  explicit vppnoc_base_initiator_socket(const char* name)
  : port_type(name)
  , m_export(sc_core::sc_gen_unique_name((std::string(name) + "_export").c_str()))
  {
  }

  unsigned int get_bus_width() const
  {
    return NOOFSOCKET;
  }

  //
  // Bind initiator socket to target socket
  // - Binds the port of the initiator socket to the export of the target
  //   socket
  // - Binds the port of the target socket to the export of the initiator
  //   socket
  //
  void bind(base_target_socket_type& s)
  {
    // initiator.port -> target.export
    (get_base_port())(s.get_base_interface());
    // target.port -> initiator.export
    (s.get_base_port())(get_base_interface());
  }

  void operator() (base_target_socket_type& s)
  {
    bind(s);
  }

  //
  // Bind initiator socket to initiator socket (hierarchical bind)
  // - Binds both the export and the port
  //
  void bind(base_type& s)
  {
    // port
    (get_base_port())(s.get_base_port());
    // export
    (s.get_base_export())(get_base_export());
  }

  void operator() (base_type& s)
  {
    bind(s);
  }

  //
  // Bind interface to socket
  // - Binds the interface to the export of this socket
  //
  void bind(bw_interface_type& ifs)
  {
    (get_base_export())(ifs);
  }

  void operator() (bw_interface_type& s)
  {
    bind(s);
  }

  // Implementation of pure virtual functions of base class
  virtual sc_core::sc_port_b<FW_IF> &   get_base_port()      { return *this;    }
  virtual                    BW_IF  &   get_base_interface() { return m_export; }
  virtual sc_core::sc_export<BW_IF> &   get_base_export()    { return m_export; }

protected:
  export_type m_export;
};


template <unsigned int NOOFSOCKET = 4,
          typename TYPES = vppnoc_protocol_types,
          int N = 1,
          sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND >

class vppnoc_initiator_socket :
  public vppnoc_base_initiator_socket <NOOFSOCKET,
                               vppnoc_tlm_fw_transport_if<TYPES>,
                               vppnoc_tlm_bw_transport_if<TYPES>,
                               N
                               ,POL
                              >
{
public:
  vppnoc_initiator_socket() :
    vppnoc_base_initiator_socket<NOOFSOCKET,
                         vppnoc_tlm_fw_transport_if<TYPES>,
                         vppnoc_tlm_bw_transport_if<TYPES>,
                         N,
                         POL
                         >()
  {
  }

  explicit vppnoc_initiator_socket(const char* name) :
    vppnoc_base_initiator_socket<NOOFSOCKET,
                         vppnoc_tlm_fw_transport_if<TYPES>,
                         vppnoc_tlm_bw_transport_if<TYPES>,
                         N,
                         POL
                         >(name)
  {
  }
};
}

#endif //VPPNOC_INITIATOR_SOCKET_H