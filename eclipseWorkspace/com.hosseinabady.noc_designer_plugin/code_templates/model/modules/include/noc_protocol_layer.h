/*
 * norc_protocol_layer.h
 *
 *  Created on: 27-Mar-2009
 *      Author: csxmh
 */

#ifndef NORC_PROTOCOL_LAYER_H_
#define NORC_PROTOCOL_LAYER_H_


#include "model/model.h"
#include "NoRC/protocol_data_units/include/noc_payload.h"
#include "model/modules/include/noc_abstract_module.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"


#define  NOCNIMANAGER_TEMPLATE_PARAMETER NoCProtocolLayer<NR_OF_INPUT_CHANNEL,\
											  NR_OF_OUTPUT_CHANNEL,\
								              INPUT_CHANNEL_CAPACITY, \
								              OUTPUT_CHANNEL_CAPACITY,\
								              tlm::tlm_base_protocol_types,\
                                              tlm::tlm_base_protocol_types,\
								              TLM_GENERIC_PAYLOAD,\
								              TLM_PHASE,\
								              TLM_SYNC_ENUM>,\
											  transaction_type,\
											  tlm::tlm_base_protocol_types,\
											  NR_OF_INPUT_CHANNEL,\
											  NR_OF_OUTPUT_CHANNEL,\
								              INPUT_CHANNEL_CAPACITY,\
								              OUTPUT_CHANNEL_CAPACITY,\
								              tlm::tlm_base_protocol_types,\
                                              tlm::tlm_base_protocol_types,\
								              TLM_GENERIC_PAYLOAD,\
								              TLM_PHASE,\
								              TLM_SYNC_ENUM\

#define PRTOCOL_LAYER_TEMPLATE_PARAMETER      NR_OF_INPUT_CHANNEL,\
											  NR_OF_OUTPUT_CHANNEL,\
											  INPUT_CHANNEL_CAPACITY,\
											  OUTPUT_CHANNEL_CAPACITY,\
											  TLM_NOC_INPUT_PAYLOAD,\
											  TLM_NOC_OUTPUT_PAYLOAD,\
											  TLM_GENERIC_PAYLOAD,\
											  TLM_PHASE,\
											  TLM_SYNC_ENUM\

#define NUMBER_OF_CALLBACK_FUNCTION  6

template <int NR_OF_INPUT_CHANNEL = 1,
		  int NR_OF_OUTPUT_CHANNEL = 1,
          int INPUT_CHANNEL_CAPACITY = 1,
		  int OUTPUT_CHANNEL_CAPACITY = 1,
          class TLM_NOC_INPUT_PAYLOAD = tlm::tlm_base_protocol_types,
          class TLM_NOC_OUTPUT_PAYLOAD = tlm::tlm_base_protocol_types,
          class TLM_GENERIC_PAYLOAD = tlm::tlm_generic_payload,
          class TLM_PHASE= tlm::tlm_phase,
          class TLM_SYNC_ENUM = tlm::tlm_sync_enum
          >
class NoCProtocolLayer : public NoCAbstractModule {

public:


	NoCProtocolLayer(sc_core::sc_module_name name, int id) :
		NoCAbstractModule(name, id)
	{

	}

	//*********************************************************************************
	// 					Virtual Interfaces for Ports
	//*********************************************************************************
	virtual sync_enum_type downstream_nb_transport_fw(int initiator_id, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {return tlm::TLM_ACCEPTED;}

	virtual sync_enum_type downstream_nb_transport_bw(int initiator_id, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {return tlm::TLM_ACCEPTED;}

	virtual sync_enum_type upstream_nb_transport_fw(int initiator_id, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {return tlm::TLM_ACCEPTED;}

	virtual sync_enum_type upstream_nb_transport_bw(int initiator_id, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {return tlm::TLM_ACCEPTED;}

	virtual sync_enum_type nb_transport_bw(int input_ch, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& delay) {return tlm::TLM_ACCEPTED;}
	virtual sync_enum_type nb_transport_fw(int input_ch, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& delay) {return tlm::TLM_ACCEPTED;}

	virtual sync_enum_type nb_external_transport_fw(char* FName) {return tlm::TLM_ACCEPTED;}
	
	virtual void function_31 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_32 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_33 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_34 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_35 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_36 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 

	virtual void function_21 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_22 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_23 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_24 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_25 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_26 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 

	virtual void function_11 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_12 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_13 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_14 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_15 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 
	virtual void function_16 (transaction_type* p_flit_trans, unsigned int input_ch_no) {}; 

	

	void setUplayer(NoCProtocolLayer<PRTOCOL_LAYER_TEMPLATE_PARAMETER>* ul) {
		up_layer = ul;
	}

	NoCProtocolLayer<PRTOCOL_LAYER_TEMPLATE_PARAMETER>* getUplayer() {
		return up_layer;
	}

	void setDownlayer(NoCProtocolLayer<PRTOCOL_LAYER_TEMPLATE_PARAMETER>* dl) {
		down_layer = dl;
	}

	NoCProtocolLayer<PRTOCOL_LAYER_TEMPLATE_PARAMETER>* getDownlayer() {
		return down_layer;
	}
//private:
	NoCProtocolLayer<PRTOCOL_LAYER_TEMPLATE_PARAMETER>*	    up_layer;
	NoCProtocolLayer<PRTOCOL_LAYER_TEMPLATE_PARAMETER>*	    down_layer;

	//tlm_noc::NoCNIManager<NOCNIMANAGER_TEMPLATE_PARAMETER>*	m_ni_manager;
};
#endif /* NORC_PROTOCOL_LAYER_H_ */
