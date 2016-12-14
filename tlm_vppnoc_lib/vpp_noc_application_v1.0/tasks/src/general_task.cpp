#include <tasks/include/general_task.h>
#include <NoRC/utilities/include/utilities.h>
#include "NoRC/protocol_data_units/include/noc_payload.h"
#include <string>
#include <sstream>
#include <fstream>
GeneralTask::GeneralTask() {
	turn = 0;
}

int GeneralTask::action(tlm::tlm_generic_payload* p_trans) {
	tlm_noc::noc_message_payload* p_message_trans_ext;
	p_trans->get_extension(p_message_trans_ext);
	noc_gp_hdr(p_message_trans_ext, m_message_id)  = turn++;
	std::cout << "I'm " << getTaskType() << " of Application " << getApplicationId() << " " << sc_core::sc_time_stamp() << " turn = " << noc_gp_hdr(p_message_trans_ext, m_message_id) << std::endl;
	//---------FUNCTION START----------------------------
	int i = 0;
	
////////////////////////////

	if(getTaskId() == 1) {
		std::ifstream InputFile;
		InputFile.open("Image1.ppm");
		if (!InputFile.is_open())
			std::cout << "1-Error in Opened File " << std::endl;
			unsigned char  R1 = 0;
			int i = 0;
			InputFile.seekg (0, std::ios::beg);
			while (!InputFile.eof()) {
				InputFile >> R1;
				p_message_trans_ext->noc_trans.view_as.pdu.body[i++]= R1;
			}
			noc_gp_hdr(p_message_trans_ext, m_payload_size) = i-1;
	}
///////------------------------------


	std::ostringstream oss;
	oss << "output\\general_task_" << getTaskType() << "_of_app_" <<getApplicationId() << ".out" ;
	std::string fileName(oss.str());
	if (turn == 1)
		OutputFile.open(fileName.c_str());
	else
		OutputFile.open(fileName.c_str(), std::fstream::app);
  

  int message_payload_size = noc_gp_hdr(p_message_trans_ext, m_payload_size);
  OutputFile << "turn = " << turn-1 << "  ";
  for (i = 0; i < message_payload_size; i++)
	  OutputFile << p_message_trans_ext->noc_trans.view_as.pdu.body[i];
  OutputFile << std::endl;
  OutputFile.close();

    i = 0;

	tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
	p_trans->set_command( cmd );
	p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );

	p_trans->set_data_length( 4 );
	p_trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
	p_trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
	p_trans->set_dmi_allowed( false ); // Mandatory initial value
	p_trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial values
		

	noc_gp_hdr(p_message_trans_ext, m_src_task)    = this;
	noc_gp_hdr(p_message_trans_ext, m_src_tile)    = this->getHostTileId();
	noc_gp_hdr(p_message_trans_ext, m_src_core)    = this->getHostCoreId();
	noc_gp_hdr(p_message_trans_ext, m_dst_tile)    = getSuccessors()->front()->getNextTask()->getHostTileId();
	noc_gp_hdr(p_message_trans_ext, m_dst_core)    = getSuccessors()->front()->getNextTask()->getHostCoreId();
	noc_gp_hdr(p_message_trans_ext, m_cmd)         = tlm_noc::WRITE_DATA;
	noc_gp_hdr(p_message_trans_ext, m_cast)        = tlm_noc::UNI_CAST;
	noc_gp_hdr(p_message_trans_ext, m_message_id)  = getTaskType()+1000;
	
	

	if(getTaskId() == 9)
		return 0;
  return 1;
}


GeneralTask::~GeneralTask() {

}
