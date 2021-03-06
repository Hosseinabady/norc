
#include <tasks/include/imagewrite.h>
#include <NoRC/utilities/include/utilities.h>
#include "NoRC/protocol_data_units/include/noc_payload.h"

ImageWriteTask::ImageWriteTask() {
	turn = 0;
  
}

ImageWriteTask::~ImageWriteTask() {

}

int ImageWriteTask::action(tlm::tlm_generic_payload* p_trans) {

	tlm_noc::noc_message_payload* p_message_trans_ext;
	p_trans->get_extension(p_message_trans_ext);
	std::cout << "I'm ImageWrite " << sc_core::sc_time_stamp() <<  "turn = " << noc_gp_hdr(p_message_trans_ext, m_message_id) << std::endl;
	turn++;
  //---------FUNCTION START----------------------------
  int i;
  if ((turn % 2) == 0)
	OutputFile.open("Image1.jpg");
  else 
	OutputFile.open("Image2.jpg");
   int message_payload_size = noc_gp_hdr(p_message_trans_ext, m_payload_size);
  for (i = 0; i < message_payload_size; i++)
	  OutputFile << p_message_trans_ext->noc_trans.view_as.pdu.body[i] << std::endl;
  OutputFile.close();

 
	tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
	p_trans->set_command( cmd );
	p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
	p_trans->set_data_length( 4 );
	p_trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
	p_trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
	p_trans->set_dmi_allowed( false ); // Mandatory initial value
	p_trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial values
		

	noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
	noc_gp_hdr(p_message_trans_ext, m_src_tile)    = this->getHost_tile();
	noc_gp_hdr(p_message_trans_ext, m_src_core)    = this->getHost_core();
	noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::WRITE_DATA;
	noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
	//noc_gp_hdr(p_message_trans_ext, m_message_id)  = get_id();

  return 0;
}





