#ifndef __SIMPLE_TARGET_SOCKET_H__
#define __SIMPLE_TARGET_SOCKET_H__

#include "sockets/vppnoc_target_socket.h"
#include "core/vppnoc_core_ifs.h"
#include "core/vppnoc_types.h"

namespace vppnoc {
using namespace sc_core;
using namespace sc_dt;
using namespace std;

template <typename MODULE,
          unsigned int NOOFSOCKET = 4,
          typename TYPES = vppnoc_protocol_types>
class vppnoc_simple_target_socket :
  public vppnoc_target_socket<NOOFSOCKET, TYPES>
{
  friend class fw_process;
  friend class bw_process;
public:
  typedef typename TYPES::vppnoc_payload_type              transaction_type;
  typedef typename TYPES::vppnoc_phase_type                phase_type;
  typedef vppnoc_sync_enum                                 sync_enum_type;
  typedef vppnoc_tlm_fw_transport_if<TYPES>                fw_interface_type;
  typedef vppnoc_tlm_bw_transport_if<TYPES>                bw_interface_type;
  typedef vppnoc_target_socket<NOOFSOCKET, TYPES>          base_type;

public:
  explicit vppnoc_simple_target_socket(const char* n = "vppnoc_simple_target_socket") :
    base_type(sc_core::sc_gen_unique_name(n)),
    m_fw_process(this),
    m_bw_process(this)
  {
    bind(m_fw_process);
  }

  // bw transport must come thru us.
  vppnoc_tlm_bw_transport_if<TYPES> * operator ->() {return &m_bw_process;}

  // REGISTER_XXX
  void register_nb_transport_fw(MODULE* mod,
                                sync_enum_type (MODULE::*cb)( transaction_type&
                                                            , vppnoc_phase & phase
                                                            , sc_core::sc_time&
                                                            ))
  {
    assert(!sc_core::sc_get_curr_simcontext()->elaboration_done());
    m_fw_process.set_nb_transport_ptr(mod, cb);
  }

private:
  //make call on bw path.
  sync_enum_type bw_nb_transport(transaction_type& trans, vppnoc_phase & phase, sc_core::sc_time &delay)
  {
    return base_type::operator ->()->nb_transport_bw(p_trans, phase, delay);
  }


  //Helper class to handle bw path calls
  // Needed to detect transaction end when called from b_transport.
  class bw_process : public vppnoc_tlm_bw_transport_if<TYPES>
  {
  public:
    bw_process(vppnoc_simple_target_socket *p_own) : m_owner(p_own)
    {
    }

    sync_enum_type nb_transport_bw(transaction_type& trans, vppnoc_phase & phase, sc_time &delay)
    {
	    return m_owner->bw_nb_transport(p_trans, phase, delay);
    }

  private:
    vppnoc_simple_target_socket *m_owner;
  };

  class fw_process : public vppnoc_tlm_fw_transport_if<TYPES>
  {
  public:
    typedef sync_enum_type (MODULE::*NBTransportPtr)(transaction_type&,
                                                     vppnoc_phase & phase,
                                                     sc_core::sc_time&);
 
    fw_process(vppnoc_simple_target_socket *p_own) :
      m_name(p_own->name()),
      m_owner(p_own),
      m_mod(0),
      m_nb_transport_ptr(0),
      m_response_in_progress(false)
    {
      sc_core::sc_spawn_options opts;
      opts.set_sensitivity(&m_peq.get_event());
      sc_core::sc_spawn(sc_bind(&fw_process::b2nb_thread, this), 
                        sc_core::sc_gen_unique_name("b2nb_thread"), &opts);
    }
  
    void set_nb_transport_ptr(MODULE* mod, NBTransportPtr p)
    {
      if (m_nb_transport_ptr) {
        std::cerr << m_name << ": non-blocking callback allready registered" << std::endl;
      } else {
        assert(!m_mod || m_mod == mod);
        m_mod = mod;
        m_nb_transport_ptr = p;
      }
    }

// Interface implementation
    sync_enum_type nb_transport_fw( transaction_type& trans
                                  , vppnoc_phase & phase
                                  , sc_time& delay)
    {
      if (m_nb_transport_ptr) {
        // forward call
        assert(m_mod);
        return (m_mod->*m_nb_transport_ptr)(p_trans, phase, delay);

      } else {
        std::cerr << m_name << ": no transport callback registered" << std::endl;
        assert(0); exit(1);
//        return tlm::TLM_COMPLETED;   ///< unreachable code
      }
    }

	


    void free(vppnoc_transaction& trans)
    {

    }


  private:
    const std::string m_name;
    vppnoc_simple_target_socket *m_owner;
    MODULE* m_mod;
    NBTransportPtr m_nb_transport_ptr;
    bool m_response_in_progress;
    sc_core::sc_event m_end_response;
  };

private:
  fw_process m_fw_process;
  bw_process m_bw_process;
};

//ID Tagged version
template <typename MODULE,
          unsigned int NOOFSOCKET = 32,
          typename TYPES = vppnoc_protocol_types>
class vppnoc_simple_target_socket_tagged :
  public vppnoc_target_socket<NOOFSOCKET, TYPES>
{
  friend class fw_process;
  friend class bw_process;
public:
  typedef typename TYPES::vppnoc_payload_type              transaction_type;
  typedef typename TYPES::vppnoc_phase_type                phase_type;
  typedef vppnoc_sync_enum                                 sync_enum_type;
  typedef vppnoc_tlm_fw_transport_if<TYPES>                fw_interface_type;
  typedef vppnoc_tlm_bw_transport_if<TYPES>                bw_interface_type;
  typedef vppnoc_target_socket<NOOFSOCKET, TYPES>          base_type;

public:
  vppnoc_simple_target_socket_tagged() :
    base_type(sc_core::sc_gen_unique_name("vppnoc_simple_target_socket_tagged")),
    m_fw_process(this),
    m_bw_process(this)
  {
    bind(m_fw_process);
  }
  
  explicit vppnoc_simple_target_socket_tagged(const char* n/* = "vppnoc_simple_target_socket_tagged"*/) :
    base_type(sc_core::sc_gen_unique_name(n)),
    m_fw_process(this),
    m_bw_process(this)
  {
    bind(m_fw_process);
  }

  // bw transport must come thru us.
  vppnoc_tlm_bw_transport_if<TYPES> * operator ->() {return &m_bw_process;}

  // REGISTER_XXX
  void register_nb_transport_fw(MODULE* mod,
                                sync_enum_type (MODULE::*cb)(int id
                                                            , transaction_type&
                                                            , phase_type & phase
                                                            , sc_time&),
                                int id)
  {
    assert(!sc_core::sc_get_curr_simcontext()->elaboration_done());
    m_fw_process.set_nb_transport_ptr(mod, cb);
    m_fw_process.set_nb_transport_user_id(id);
  }


private:
  //make call on bw path.
  sync_enum_type bw_nb_transport(transaction_type& trans, vppnoc_phase & phase, sc_time &delay)
  {
    return base_type::operator ->()->nb_transport_bw(trans, phase, delay);
  }


  //Helper class to handle bw path calls
  // Needed to detect transaction end when called from b_transport.
  class bw_process : public vppnoc_tlm_bw_transport_if<TYPES>
  {
  public:
    bw_process(vppnoc_simple_target_socket_tagged *p_own) : m_owner(p_own)
    {
    }

    sync_enum_type nb_transport_bw(transaction_type& trans, vppnoc_phase & phase, sc_time &delay)
    {
	    return m_owner->bw_nb_transport(trans, phase, delay);
    }

  private:
    vppnoc_simple_target_socket_tagged *m_owner;
  };

  class fw_process : public vppnoc_tlm_fw_transport_if<TYPES>
  {
  public:
    typedef sync_enum_type (MODULE::*NBTransportPtr)( int id 
                                                    , transaction_type&
                                                    , vppnoc_phase & phase
                                                    , sc_time&);
      
    fw_process(vppnoc_simple_target_socket_tagged *p_own) :
      m_name(p_own->name()),
      m_owner(p_own),
      m_mod(0),
      m_nb_transport_ptr(0),
      m_nb_transport_user_id(0),
      m_response_in_progress(false)
    {
//      sc_core::sc_spawn_options opts;
//      opts.set_sensitivity(&m_peq.get_event());
//      sc_core::sc_spawn(sc_bind(&fw_process::b2nb_thread, this), 
//                        sc_core::sc_gen_unique_name("b2nb_thread"), &opts);
    }
  
    void set_nb_transport_user_id(int id) { m_nb_transport_user_id = id; }


    void set_nb_transport_ptr(MODULE* mod, NBTransportPtr p)
    {
      if (m_nb_transport_ptr) {
        std::cerr << m_name << ": non-blocking callback allready registered" << std::endl;

      } else {
        assert(!m_mod || m_mod == mod);
        m_mod = mod;
        m_nb_transport_ptr = p;
      }
    }


    
// Interface implementation
	sync_enum_type nb_transport_fw( transaction_type& trans
                                , vppnoc_phase & phase
                                , sc_time& delay)
    {
		return (m_mod->*m_nb_transport_ptr)(m_nb_transport_user_id, trans, phase, delay);
    }
  private:
// dynamic process handler for nb2b conversion

    class process_handle_class {
    public:
      process_handle_class(transaction_type * trans,
                           sc_core::sc_event* e):
        m_trans(trans),m_e(e),m_suspend(false){}

      transaction_type*  m_trans;
      sc_core::sc_event* m_e;
      sc_core::sc_event  m_wakeup;
      bool m_suspend;
    };
    
    class process_handle_list {
    public:
      process_handle_list() {}
 
      process_handle_class* get_handle(transaction_type* trans,sc_core::sc_event* e) 
      {                
        typename std::vector<process_handle_class*>::iterator it;

        for(it = v.begin(); it != v.end(); it++) { 
          if ((*it)->m_suspend) {  // found suspended dynamic process, re-use it
            (*it)->m_trans = trans; // replace to new one
            (*it)->m_e = e;            
            return *it;  
          }
        }
        return NULL; // no suspended process
      }

      void put_handle(process_handle_class* ph)
      {
        v.push_back(ph);
      }

    private:
      std::vector<process_handle_class*> v;
    };

    process_handle_list m_process_handle;


    void free(vppnoc_transaction* trans)
    {

    }

  private:


  private:
    const std::string m_name;
    vppnoc_simple_target_socket_tagged *m_owner;
    MODULE* m_mod;
    NBTransportPtr m_nb_transport_ptr;
    int m_nb_transport_user_id;
    bool m_response_in_progress;
    sc_core::sc_event m_end_response;
  };

private:
  fw_process m_fw_process;
  bw_process m_bw_process;

};

}

#endif
