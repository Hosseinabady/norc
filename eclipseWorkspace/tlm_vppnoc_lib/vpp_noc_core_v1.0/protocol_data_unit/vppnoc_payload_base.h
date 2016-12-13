#ifndef NOC_PAYLOAD_BASE
#define NOC_PAYLOAD_BASE


#include "include/vppnoc_utilities.h"
#include "core/vppnoc_types.h"

namespace vppnoc {
using namespace  sc_core; 
using namespace  sc_dt;
using namespace  std;
class vppnoc_mm_interface {
public:
	virtual void free(vppnoc_transaction*) {};
  virtual ~vppnoc_mm_interface() {}
};

enum transaction_command {IDLE=0, READ = 1, WRITE, POP, CONTROL_FREE_CHANNEL};
enum payload_kind {MESSAGE_PAYLOAD = 0, PACKET_PAYLOAD, FLIT_PAYLOAD};
enum vppnoc_trans_status {PAYLOAD_OK = 0, PAYLOAD_RESOURCE_ERROR, PAYLOAD_RESOURCE_FREE};
class vppnoc_payload_base {
public:
	vppnoc_payload_base() : m_ref_count(0), m_mm(0) {}
	void acquire(){
		assert(m_mm != 0); 
		m_ref_count++;
	}


	explicit vppnoc_payload_base(vppnoc_mm_interface* mm):
        m_mm(mm), 
        m_ref_count(0)
    {
    }

  void set_payload_kind(payload_kind payload_kind) { m_payload_kind = payload_kind;}
  payload_kind get_payload_kind() { return m_payload_kind;}
  void set_transaction_command(transaction_command trans_cmd) { m_trans_cmd = trans_cmd;}
  transaction_command get_transaction_command() { return m_trans_cmd;}

  int get_ref_count(){return m_ref_count;}
	void release(){
		assert(m_mm != 0); 
		if (--m_ref_count==0) 
			m_mm->free(this);
	}
	void set_mm(vppnoc_mm_interface* mm) { m_mm = mm; }
  bool has_mm() { return m_mm != 0; }
  
  vppnoc_trans_status get_status(                     ) {return m_status;}
  void                set_status(vppnoc_trans_status s) {   m_status = s;}

  void set_time_stamp(sc_time t) { m_time_stamp = t; }
  sc_time get_time_stamp(){return m_time_stamp; }
  virtual void free() { delete this->m_pdu; delete this; }	

	vppnoc_response_status  get_response_status() const {return m_response_status;}
	void                    set_response_status(const vppnoc_response_status response_status)
        {m_response_status = response_status;}
    std::string           get_response_string() const
    {
        switch(m_response_status)                               
        {
        case VPPNOC_OK_RESPONSE:            return "VPPNOC_OK_RESPONSE";
        case VPPNOC_INCOMPLETE_RESPONSE:    return "VPPNOC_INCOMPLETE_RESPONSE";
        case VPPNOC_GENERIC_ERROR_RESPONSE: return "VPPNOC_GENERIC_ERROR_RESPONSE";
        case VPPNOC_ADDRESS_ERROR_RESPONSE: return "VPPNOC_ADDRESS_ERROR_RESPONSE";
        case VPPNOC_COMMAND_ERROR_RESPONSE: return "VPPNOC_COMMAND_ERROR_RESPONSE";
        case VPPNOC_BURST_ERROR_RESPONSE:   return "VPPNOC_BURST_ERROR_RESPONSE";
        case VPPNOC_BYTE_ENABLE_ERROR_RESPONSE: return "VPPNOC_BYTE_ENABLE_ERROR_RESPONSE";
        }
        return "VPPNOC_UNKNOWN_RESPONSE";
    }

public:
   vppnoc_pdu_base*                         m_pdu;
private:
	unsigned int                              m_ref_count;
	vppnoc_mm_interface*                      m_mm;
	vppnoc_response_status                    m_response_status;
  transaction_command                       m_trans_cmd;
  payload_kind                              m_payload_kind;
  sc_time                                   m_time_stamp;
  vppnoc_trans_status                       m_status;
};


}



#endif //NOC_PAYLOAD_BASE