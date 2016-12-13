#ifndef NOC_PAYLOAD_H_
#define NOC_PAYLOAD_H_

#include "model/protocol_data_unit/include/noc_tlm_extension_payload.h"
#include "NoRC/protocol_data_units/include/noc_message_payload.h"
#include "NoRC/protocol_data_units/include/noc_phit_payload.h"
#include "NoRC/protocol_data_units/include/noc_flit_payload.h"
#include "NoRC/protocol_data_units/include/noc_packet_payload.h"


#include "NoRC/utilities/include/utilities.h"


#define noc_gp_hdr(_pdu,_field)     (_pdu)->noc_trans.view_as.pdu.hdr._field
#define noc_gp_bdy(_pdu)            (_pdu)->noc_trans.view_as.pdu.body

#endif /*NOC_PAYLOAD_H_*/
