
#include <tasks/include/readimage.h>
#include <NoRC/utilities/include/utilities.h>
#include "NoRC/protocol_data_units/include/noc_payload.h"

ReadImageTask::ReadImageTask() {
	turn = 0;
//  InputFile.open("Image.ppm");
//  if (!InputFile.is_open())
//	  std::cout << "1-Error in Opened File " << std::endl;

}

int ReadImageTask::action(tlm::tlm_generic_payload* p_trans) {
tlm_noc::noc_message_payload* p_message_trans_ext;
  p_trans->get_extension(p_message_trans_ext);
  turn++;
  std::cout << "I'm Reading Image " << sc_core::sc_time_stamp() <<  "turn = " << turn << std::endl;
  //---------FUNCTION START----------------------------
  

  
  if (turn % 2 == 0) {
	InputFile.open("Image1.ppm");
	if (!InputFile.is_open())
		  std::cout << "1-Error in Opened File " << std::endl;
  } else {
	InputFile.open("Image2.ppm");
	if (!InputFile.is_open())
		  std::cout << "1-Error in Opened File " << std::endl;
  }

  OutputFile.open("ReadImageOut.txt");
  
  

  if (!InputFile.is_open())
    std::cout << "2-Error in Opened File " << std::endl;
  unsigned char  R1 = 0;
  int i = 0;
  InputFile.seekg (0, std::ios::beg);


  while (!InputFile.eof()) {
    InputFile >> R1;
	p_message_trans_ext->noc_trans.view_as.pdu.body[i++]= R1;
  }

  noc_gp_hdr(p_message_trans_ext, m_payload_size) = i-1;
	for (int j = 0; j < i-1; j++)
		OutputFile << p_message_trans_ext->noc_trans.view_as.pdu.body[j];
	std::cout << std::endl;	
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
	noc_gp_hdr(p_message_trans_ext, m_dst_tile)    = getSuccessors().front()->getHost_tile();
	noc_gp_hdr(p_message_trans_ext, m_dst_core)    = getSuccessors().front()->getHost_core();
	noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::WRITE_DATA;
	noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
	noc_gp_hdr(p_message_trans_ext, m_message_id)  = turn;

	InputFile.close();
  return 1;
}


ReadImageTask::~ReadImageTask() {
  InputFile.close();
}
