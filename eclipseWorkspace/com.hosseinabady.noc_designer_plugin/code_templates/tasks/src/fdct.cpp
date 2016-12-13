
#include <tasks/include/fdct.h>
#include <NoRC/utilities/include/utilities.h>
#include "NoRC/protocol_data_units/include/noc_payload.h"


FDCTTask::FDCTTask() {

}

int FDCTTask::action(tlm::tlm_generic_payload* p_trans) {
    tlm_noc::noc_message_payload* p_message_trans_ext;
  p_trans->get_extension(p_message_trans_ext);

	std::cout << "I'm FDCT " << sc_core::sc_time_stamp() <<  "turn = " << noc_gp_hdr(p_message_trans_ext, m_message_id) << std::endl;

  //---------FUNCTION START----------------------------
  int  block[8];
  int  blockFDCT[8];
  int  tmp[8][8];
  int i, k, m;


  OutputFile.open("FDCTingOut.txt");
  OutputFile2.open("FDCTingIn.txt");

  int message_payload_size = noc_gp_hdr(p_message_trans_ext, m_payload_size);
  for (i = 0; i < message_payload_size; i++)
    OutputFile2 << p_message_trans_ext->noc_trans.view_as.pdu.body[i] << std::endl;
  OutputFile2.close();





	tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
	p_trans->set_command( cmd );
	p_trans->set_address( (sc_dt::uint64)NETWORK_INTERFACE_ADD );
	//p_trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
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
	//noc_gp_hdr(p_message_trans_ext, m_message_id)  = get_id();

  return 1;
}


FDCTTask::~FDCTTask() {

}


void FDCTTask::FDCT1D( int row[ 8 ], int row_res[ 8 ], int bias ) {

  float x0, x1, x2, x3, t0, t1, t2, t3, t4, t5, t6, t7;

  t0 = (float) (row[0] + row[7] - bias);
  t7 = (float) (row[0] - row[7]);
  t1 = (float) (row[1] + row[6] - bias);
  t6 = (float) (row[1] - row[6]);
  t2 = (float) (row[2] + row[5] - bias);
  t5 = (float) (row[2] - row[5]);
  t3 = (float) (row[3] + row[4] - bias);
  t4 = (float) (row[3] - row[4]);


  /* even part */
  x0 = t0 + t3;
  x2 = t1 + t2;
  row_res[0] = (int) (x0 + x2);
  row_res[4] = (int) (x0 - x2);
  x1 = t0 - t3;
  x3 = t1 - t2;
  t0 = (x1 + x3) * FA1;
  row_res[2] = (int) (x1 + t0);
  row_res[6] = (int) (x1 - t0);
  /* odd part */
  x0 = t4 + t5;
  x1 = t5 + t6;
  x2 = t6 + t7;

  t3 = x1 * FA1;
  t4 = t7 - t3;
  t0 = (x0 - x2) * FA5;
  t1 = x0 * FA2 + t0;
  row_res[3] = (int) (t4 - t1);
  row_res[5] = (int) (t4 + t1);

  t7 += t3;
  t2 = x2 * FA4 + t0;
  row_res[1] = (int) (t7 + t2);
  row_res[7] = (int) (t7 - t2);
}
