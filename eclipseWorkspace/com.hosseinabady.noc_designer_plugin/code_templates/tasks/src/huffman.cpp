
#include <tasks/include/huffman.h>
#include <NoRC/utilities/include/utilities.h>
#include "NoRC/protocol_data_units/include/noc_payload.h"


HuffmanTask::HuffmanTask() {
  lpred = 0;
  crpred = 0;
  cbpred = 0;
  iPixel = 0;

}

int HuffmanTask::action(tlm::tlm_generic_payload* p_trans) {
  tlm_noc::noc_message_payload* p_message_trans_ext;
  p_trans->get_extension(p_message_trans_ext);

	std::cout << "I'm Huffman " << sc_core::sc_time_stamp() <<  "turn = " << noc_gp_hdr(p_message_trans_ext, m_message_id) << std::endl;
  //---------FUNCTION START----------------------------
  int i;

  OutputFile2.open("HuffmanIn.txt");
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


HuffmanTask::~HuffmanTask() {

}


char HuffmanTask::magnitude(short v) {
  char mag = 0;
  if (v < 0)
    v = -v;
  while (v > 0) {
    mag++;
    v >>= 1;
  }
  return mag;
}

inline void  HuffmanTask::huf_run (tlm_noc::noc_message_payload* p_message_trans_ext, short *pred, HUF   *dcht, HUF   *acht, int iBlock) {

  short    diff;
  int      len;
  int      pix;
  short    level;
  int      run, cnt;
  HUF      e;
  int      i;

  i = 0;
  pix = p_message_trans_ext->noc_trans.view_as.pdu.body[iBlock*64];
  diff  = pix - *pred;
  *pred = pix;

  if (diff < 0){
    len = magnitude(-diff);
    e   = dcht[len];
    HCode(p_message_trans_ext, e.val, e.nb);
    HCode(p_message_trans_ext, ((diff-1)&(~(-1<<len))), len);
  } else {
      len = magnitude(diff);
      e = dcht[len];
      HCode(p_message_trans_ext, e.val, e.nb);
      HCode(p_message_trans_ext, diff, len);
  }

  run = 0;

  for (cnt=1; cnt<64; ++cnt) {
    pix = p_message_trans_ext->noc_trans.view_as.pdu.body[iBlock*64 + cnt];
    level = pix;
    if (level != 0) {
      if (level < 0) {
        len = magnitude(-level);
        while (run & ~0xf) {
          e = acht[0xf0];
          HCode(p_message_trans_ext, e.val, e.nb);
          run -= 16;
        }
        e = acht[(run<<4) | len];
        HCode(p_message_trans_ext, e.val, e.nb);
        HCode(p_message_trans_ext, (level-1)&(~(-1<<len)), len);
      } else {
        len = magnitude(level);
        while (run & ~0xf) {
          e = acht[0xf0];
          HCode(p_message_trans_ext, e.val, e.nb);
          run -= 16;
        }
        e = acht[(run<<4) | len];
        HCode(p_message_trans_ext, e.val, e.nb);
        HCode(p_message_trans_ext, (level-1)&(~(-1<<len)), len);
      }
      run = 0;
    } else {
      run++;
    }
  }

  if (run > 0) {
    /* EOB */
    e = acht[0];
    HCode(p_message_trans_ext, e.val, e.nb);
  }
}


void HuffmanTask::HCode(tlm_noc::noc_message_payload* p_message_trans_ext, unsigned short v, unsigned short b) {
  /*pixel_type*/int pixel_bits;
  /*pixel_type*/int pixel_n;
  int _bits;
  int _n;

  pixel_n = b;
  pixel_bits = v;
  _n = pixel_n;
  _bits = pixel_bits;

   nbb += _n;
   if (nbb > 32)  {
     unsigned int extra = nbb - 32;
     bb |= (unsigned int)_bits >> extra;
     STORE_BITS(p_message_trans_ext, bb);
     bb = (unsigned int)_bits << (32 - extra);
     nbb = extra;
   } else {
     bb |= (unsigned int)_bits << (32 - nbb);
   }
}

void HuffmanTask::STORE_BITS(tlm_noc::noc_message_payload* p_message_trans_ext, int bb) {
  unsigned char t;

  t = bb>>24;
  wb(p_message_trans_ext, t);
  if (t == 0xff) wb(p_message_trans_ext, 0);;

  t = bb>>16;
  wb(p_message_trans_ext, t);
  if (t == 0xff) wb(p_message_trans_ext, 0);

  t = bb>>8;
  wb(p_message_trans_ext, t);
  if (t == 0xff) wb(p_message_trans_ext, 0);

  t = bb;
  wb(p_message_trans_ext, t);
  if (t == 0xff) wb(p_message_trans_ext, 0);
}

void HuffmanTask::wb(tlm_noc::noc_message_payload* p_message_trans_ext, int j) {
  char chj;
  chj = j;
  p_message_trans_ext->noc_trans.view_as.pdu.body[iPixel++] = chj;
  OutputFile << chj;

}

void HuffmanTask::ww(tlm_noc::noc_message_payload* p_message_trans_ext, int j) {
  wb(p_message_trans_ext, j>>8);
  wb(p_message_trans_ext, j);
}

void HuffmanTask::JpegHeader(tlm_noc::noc_message_payload* p_message_trans_ext, int sizex, int sizey)
{
	OutputFile.open("Image.jpg", std::ios::binary);
  //void fdump_init(char *filename, int sizex, int sizey) {
  int qv, i;
  num = 0;
  //OutputFile = fopen(filename,"w");
  bb      = 0; /* bitbuffer */
  nbb     = 0;

  ww(p_message_trans_ext, 0xffd8); // SOI
  ww(p_message_trans_ext, 0xffe0); // APP0
  ww(p_message_trans_ext, 0x0010);
  ww(p_message_trans_ext, 0x4a46); // standard APP0 marker
  ww(p_message_trans_ext, 0x4946);
  ww(p_message_trans_ext, 0x0001);
  ww(p_message_trans_ext, 0x0100);
  ww(p_message_trans_ext, 0x0001); // Xdensity
  ww(p_message_trans_ext, 0x0001); // Ydensity
  ww(p_message_trans_ext, 0x0000); // thumbnail

  ww(p_message_trans_ext, 0xffdb); // quantization table
  ww(p_message_trans_ext, 0x0043);
  wb(p_message_trans_ext, 0x00);   // table #0; luminance
  for (qv=0; qv<64; ++qv) {
    wb(p_message_trans_ext, stdlqt[zigzag_order[qv]]);
  }

  ww(p_message_trans_ext, 0xffdb); // quantization table
  ww(p_message_trans_ext, 0x0043);
  wb(p_message_trans_ext, 0x01);   // table #1; chrominance
  for (qv=0; qv<64; ++qv) {
    wb(p_message_trans_ext, stdcqt[zigzag_order[qv]]);
  }

  ww(p_message_trans_ext, 0xffc0); // SOF
  ww(p_message_trans_ext, 0x0011); // Frame Header Length
  wb(p_message_trans_ext, 0x08);   // sample precision
  ww(p_message_trans_ext, sizey);  // x size
  ww(p_message_trans_ext, sizex);
  wb(p_message_trans_ext, 0x03);   // 3 components
  wb(p_message_trans_ext, 0x01); wb(p_message_trans_ext, 0x21); wb(p_message_trans_ext, 0x00); // comp0, step x;y 2;1 , qtab #0
  wb(p_message_trans_ext, 0x02); wb(p_message_trans_ext, 0x11); wb(p_message_trans_ext, 0x01); // comp1, step x;y 1;1 , qtab #1
  wb(p_message_trans_ext, 0x03); wb(p_message_trans_ext, 0x11); wb(p_message_trans_ext, 0x01); // comp2, step x;y 1;1 , qtab #1

  // HUFFMAN TABLE DC # 0
  ww(p_message_trans_ext, 0xffc4);
  ww(p_message_trans_ext, 3 + ldhtftab[0]);
  wb(p_message_trans_ext, 0x00);
  for (i=0; i<ldhtftab[0]; i++)
    wb(p_message_trans_ext, ldhtftab[i+1]);

  // HUFFMAN TABLE AC # 0
  ww(p_message_trans_ext, 0xffc4);
  ww(p_message_trans_ext, 3 + lahtftab[0]);
  wb(p_message_trans_ext, 0x10);
  for (i=0; i<lahtftab[0]; i++)
    wb(p_message_trans_ext, lahtftab[i+1]);

  // HUFFMAN TABLE DC # 1
  ww(p_message_trans_ext, 0xffc4);
  ww(p_message_trans_ext, 3 + cdhtftab[0]);
  wb(p_message_trans_ext, 0x01);
  for (i=0; i<cdhtftab[0]; i++)
    wb(p_message_trans_ext, cdhtftab[i+1]);

  // HUFFMAN TABLE AC # 1
  ww(p_message_trans_ext, 0xffc4);
  ww(p_message_trans_ext, 3 + cahtftab[0]);
  wb(p_message_trans_ext, 0x11);
  for (i=0; i<cahtftab[0]; i++)
    wb(p_message_trans_ext, cahtftab[i+1]);

  ww(p_message_trans_ext, 0xffda); // SOS
  ww(p_message_trans_ext, 0x000c);
  wb(p_message_trans_ext, 0x03);   // components
  ww(p_message_trans_ext, 0x0100); // comp 0 - huffman table sel
  ww(p_message_trans_ext, 0x0211); // comp 1 - huffman table sel
  ww(p_message_trans_ext, 0x0311); // comp 2 - huffman table sel
  ww(p_message_trans_ext, 0x003f); // fixed for baseline jpeg
  wb(p_message_trans_ext, 0x00);
}
