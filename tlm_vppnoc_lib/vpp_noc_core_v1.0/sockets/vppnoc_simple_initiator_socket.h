#ifndef VPPNOC_SIMPLE_INITIATOR_SOCKET_H
#define VPPNOC_SIMPLE_INITIATOR_SOCKET_H

#include "sockets/vppnoc_initiator_socket.h"
#include "core/vppnoc_core_ifs.h"
#include "core/vppnoc_types.h"

namespace vppnoc {

template <typename MODULE,
          unsigned int NOOFSOCKET = 4,
          typename TYPES = vppnoc_protocol_types>
class vppnoc_simple_initiator_socket :
  public vppnoc_initiator_socket<NOOFSOCKET, TYPES>
{
public:
  typedef typename TYPES::vppnoc_payload_type             transaction_type;
  typedef typename TYPES::vppnoc_phase_type               phase_type;
  typedef vppnoc_sync_enum                                sync_enum_type;
  typedef vppnoc_tlm_fw_transport_if<TYPES>               fw_interface_type;
  typedef vppnoc_tlm_bw_transport_if<TYPES>               bw_interface_type;
  typedef vppnoc_initiator_socket<NOOFSOCKET, TYPES>      base_type;

public:
  vppnoc_simple_initiator_socket() :
    base_type(sc_core::sc_gen_unique_name("vppnoc_simple_initiator_socket")),
    m_process(this->name())
  {
    this->m_export.bind(m_process);
  }

  explicit vppnoc_simple_initiator_socket(const char* n/* = "vppnoc_simple_initiator_socket"*/) :
    base_type(sc_core::sc_gen_unique_name(n)),
    m_process(this->name())
  {
    this->m_export.bind(m_process);
  }

  void register_nb_transport_bw(MODULE* mod,
                                sync_enum_type (MODULE::*cb)( transaction_type&
                                                            , sc_time&))
  {
    m_process.set_transport_ptr(mod, cb);
  }

  void register_invalidate_direct_mem_ptr(MODULE* mod,
                                          void (MODULE::*cb)(sc_dt::uint64, sc_dt::uint64))
  {
    m_process.set_invalidate_direct_mem_ptr(mod, cb);
  }

private:
  class process : public vppnoc_tlm_bw_transport_if<TYPES>
  {
  public:
    typedef sync_enum_type (MODULE::*TransportPtr)( transaction_type&
                                                  , sc_time&);
    
      
    process(const std::string& name) :
      m_name(name),
      m_mod(0),
      m_transport_ptr(0),
      m_invalidate_direct_mem_ptr(0)
    {
    }
  
    void set_transport_ptr(MODULE* mod, TransportPtr p)
    {
      if (m_transport_ptr) {
        std::cerr << m_name << ": non-blocking callback allready registered" << std::endl;

      } else {
        assert(!m_mod || m_mod == mod);
        m_mod = mod;
        m_transport_ptr = p;
      }
    }

    
    sync_enum_type nb_transport_bw(transaction_type& trans, sc_core::sc_time& delay)
    {
      if (m_transport_ptr) {
        // forward call
        assert(m_mod);
        return (m_mod->*m_transport_ptr)(p_trans, delay);

      } else {
        std::cerr << m_name << ": no transport callback registered" << std::endl;
        assert(0); exit(1);
      }
    }


  private:
    const std::string m_name;
    MODULE* m_mod;
    TransportPtr m_transport_ptr;

  };

private:
  process m_process;
};

// Tagged version

template <typename MODULE,
          unsigned int NOOFSOCKET = 4,
          typename TYPES = vppnoc_protocol_types>
class vppnoc_simple_initiator_socket_tagged :
  public vppnoc_initiator_socket<NOOFSOCKET, TYPES>
{
public:
  typedef typename TYPES::vppnoc_payload_type              transaction_type;
  typedef typename TYPES::vppnoc_phase_type                phase_type;
  typedef vppnoc_sync_enum                            sync_enum_type;
  typedef vppnoc_tlm_fw_transport_if<TYPES>               fw_interface_type;
  typedef vppnoc_tlm_bw_transport_if<TYPES>               bw_interface_type;
  typedef vppnoc_initiator_socket<NOOFSOCKET, TYPES>    base_type;

public:
  vppnoc_simple_initiator_socket_tagged() :
    base_type(sc_core::sc_gen_unique_name("vppnoc_simple_initiator_socket_tagged"))
   , m_process(this->name())
  {
    this->m_export.bind(m_process);
  }

  explicit vppnoc_simple_initiator_socket_tagged(const char* n/* = "vppnoc_simple_initiator_socket_tagged"*/) :
    base_type(sc_core::sc_gen_unique_name(n))
    ,m_process(this->name())
  {
    this->m_export.bind(m_process);
  }

  void register_nb_transport_bw(MODULE* mod,
                                sync_enum_type (MODULE::*cb)(int
                                                             , transaction_type&
                                                             , phase_type&
                                                             , sc_time&),
                                int id)
  {
    m_process.set_transport_ptr(mod, cb);
    m_process.set_transport_user_id(id);
  }

  

private:
  class process : public vppnoc_tlm_bw_transport_if<TYPES>
  {
  public:
    typedef sync_enum_type (MODULE::*TransportPtr)( int
                                                  , transaction_type&
                                                  , vppnoc_phase & phase
                                                  , sc_time&
                                                  );

    process(const std::string& name) :
      m_name(name),
      m_mod(0),
      m_transport_ptr(0),
      m_transport_user_id(0),
      m_invalidate_direct_mem_user_id(0)
    {
    }
  
    void set_transport_user_id(int id) { m_transport_user_id = id; }

    void set_transport_ptr(MODULE* mod, TransportPtr p)
    {
      if (m_transport_ptr) {
        std::cerr << m_name << ": non-blocking callback allready registered" << std::endl;

      } else {
        assert(!m_mod || m_mod == mod);
        m_mod = mod;
        m_transport_ptr = p;
      }
    }

    sync_enum_type nb_transport_bw(transaction_type& trans, vppnoc_phase & phase, sc_core::sc_time& delay)
    {
      if (m_transport_ptr) {
        // forward call
        assert(m_mod);
        return (m_mod->*m_transport_ptr)(m_transport_user_id, trans, phase, delay);

      } else {
        std::cerr << m_name << ": no transport callback registered" << std::endl;
        assert(0); exit(1);
      }
    }


  private:
    const std::string m_name;
    MODULE* m_mod;
    int m_transport_user_id;
    int m_invalidate_direct_mem_user_id;
	TransportPtr m_transport_ptr;
  };

private:
  process m_process;
};

}

#endif
