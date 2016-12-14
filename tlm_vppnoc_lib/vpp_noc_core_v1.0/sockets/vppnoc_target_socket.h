#ifndef VPPNOC_TARGET_SOCKET_H
#define VPPNOC_TARGET_SOCKET_H

#include <systemc>
#include "core/vppnoc_core_ifs.h"
#include "core/vppnoc_types.h"




namespace vppnoc {

template <unsigned int NOOFSOCKET = 4,
          typename FW_IF = vppnoc_tlm_fw_transport_if<TYPES>,
          typename BW_IF = vppnoc_tlm_bw_transport_if<TYPES> >
class vppnoc_base_target_socket_b
{
public:
  virtual ~vppnoc_base_target_socket_b() {}
	
  virtual sc_core::sc_port_b<BW_IF> & get_base_port() = 0;
  virtual sc_core::sc_export<FW_IF> & get_base_export() = 0;
  virtual                    FW_IF  & get_base_interface() = 0;
};

template <unsigned int NOOFSOCKET,
          typename FW_IF,
          typename BW_IF> class vppnoc_base_initiator_socket_b;

template <unsigned int NOOFSOCKET,
          typename FW_IF,
          typename BW_IF,
          int N, 
          sc_core::sc_port_policy POL> class vppnoc_base_initiator_socket;

template <unsigned int NOOFSOCKET = 4,
          typename FW_IF = vppnoc_tlm_fw_transport_if<TYPES>,
          typename BW_IF = vppnoc_tlm_bw_transport_if<TYPES>,
          int N = 1,
          sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class vppnoc_base_target_socket : public vppnoc_base_target_socket_b<NOOFSOCKET, FW_IF, BW_IF>, 
                                  public sc_core::sc_export<FW_IF>
{
public:
  typedef FW_IF                                 fw_interface_type;
  typedef BW_IF                                 bw_interface_type;
  typedef sc_core::sc_port<bw_interface_type, N, POL>   port_type;

  typedef sc_core::sc_export<fw_interface_type> export_type;
  typedef vppnoc_base_initiator_socket_b<NOOFSOCKET,
                                      fw_interface_type,
                                      bw_interface_type>  base_initiator_socket_type;

  typedef vppnoc_base_initiator_socket<NOOFSOCKET,
                                    fw_interface_type,
                                    bw_interface_type,
                                    N, POL> initiator_socket_type;


  typedef vppnoc_base_target_socket_b<NOOFSOCKET,
                                   fw_interface_type,
                                   bw_interface_type> base_type;

  template <unsigned int, typename, typename, int, sc_core::sc_port_policy>
  friend class vppnoc_base_initiator_socket;

public:
  vppnoc_base_target_socket()
  : export_type(sc_core::sc_gen_unique_name("vppnoc_base_target_socket"))
  , m_port(sc_core::sc_gen_unique_name("vppnoc_target_socket_port"))
  {
  }

  explicit vppnoc_base_target_socket(const char* name)
  : export_type(name)
  , m_port(sc_core::sc_gen_unique_name((std::string(name) + "_port").c_str()))
  {
  }

  unsigned int get_bus_width() const
  {
    return NOOFSOCKET;
  }

  //
  // Bind target socket to initiator socket
  // - Binds the port of the initiator socket to the export of the target
  //   socket
  // - Binds the port of the target socket to the export of the initiator
  //   socket
  //
  void bind(base_initiator_socket_type& s)
  {
    // initiator.port -> target.export
    (s.get_base_port())(get_base_interface());
    // target.port -> initiator.export
    get_base_port()(s.get_base_interface());
  }

  void operator() (base_initiator_socket_type& s)
  {
    bind(s);
  }

  //
  // Bind target socket to target socket (hierarchical bind)
  // - Binds both the export and the port
  //
  void bind(base_type& s)
  {
    // export
    (get_base_export())(s.get_base_export());
    // port
    (s.get_base_port())(get_base_port());
  }

  void operator() (base_type& s)
  {
    bind(s);
  }

  //
  // Bind interface to socket
  // - Binds the interface to the export
  //
  void bind(fw_interface_type& ifs)
  {
    (get_base_export())(ifs);
  }

  void operator() (fw_interface_type& s)
  {
    bind(s);
  }

  //
  // Forward to 'size()' of port class
  //
  int size() const
  {
  	return m_port.size();
  }
    
  //
  // Forward to 'operator->()' of port class
  //
  bw_interface_type* operator->()
  {
    return m_port.operator->();
  }
  
  //
  // Forward to 'operator[]()' of port class
  //
  bw_interface_type* operator[](int i)
  {
    return m_port.operator[](i);
  }

  // Implementation of pure virtual functions of base class
  virtual sc_core::sc_port_b<BW_IF> & get_base_port()      { return m_port;   }
  virtual                    FW_IF  & get_base_interface() { return *this;   }
  virtual sc_core::sc_export<FW_IF> & get_base_export()    { return *this;   }

protected:
  port_type m_port;
};


//
// Convenience blocking and non-blocking socket classes
//

template 
< unsigned int NOOFSOCKET = 4
, typename TYPES = vppnoc_protocol_types
, int N = 1
, sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND
>
class vppnoc_target_socket 
: public vppnoc_base_target_socket <NOOFSOCKET
                                   , vppnoc_tlm_fw_transport_if<TYPES>
                                   , vppnoc_tlm_bw_transport_if<TYPES>
                                   , N
							                     , POL>
{
public:
  vppnoc_target_socket() :
    vppnoc_base_target_socket<NOOFSOCKET,
                      vppnoc_tlm_fw_transport_if<TYPES>,
                      vppnoc_tlm_bw_transport_if<TYPES>,
                      N, 
                      POL>()
  {
  }

  explicit vppnoc_target_socket(const char* name) :
    vppnoc_base_target_socket<NOOFSOCKET,
                      vppnoc_tlm_fw_transport_if<TYPES>,
                      vppnoc_tlm_bw_transport_if<TYPES>,
                      N,
                      POL>(name)
  {
  }
};

} // namespace tlm

#endif
