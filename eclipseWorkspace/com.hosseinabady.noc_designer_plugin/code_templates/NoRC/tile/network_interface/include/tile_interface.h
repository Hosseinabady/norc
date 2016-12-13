#ifndef TILE_INTERFACE_H_
#define TILE_INTERFACE_H_

#include "tlm.h"
#include "transport_layer.h"
#include "physical_layer.h"
#include "network_layer.h"

#include "NoRC/protocol_data_units/include/noc_payload.h"
//#include "noc_message_payload.h"
#include "NoRC/protocol_data_units/include/noc_resource_map.h"
#include <iostream>

#include <cassert>
#include <queue>

using namespace std;

#define TILEINTERFACE_TEMPLATE_PARAMETER  NR_OF_INPUT_CHANNEL,\
										  NR_OF_OUTPUT_CHANNEL,\
										  INPUT_CHANNEL_CAPACITY,\
										  OUTPUT_CHANNEL_CAPACITY,\
										  TLM_NOC_INPUT_PAYLOAD,\
										  TLM_NOC_OUTPUT_PAYLOAD,\
										  TLM_GENERIC_PAYLOAD,\
										  TLM_PHASE,\
										  TLM_SYNC_ENUM


template <int NR_OF_INPUT_CHANNEL = 1,
		  int NR_OF_OUTPUT_CHANNEL = 1,
		  int INPUT_CHANNEL_CAPACITY = 1,
		  int OUTPUT_CHANNEL_CAPACITY = 1,
		  class TLM_NOC_INPUT_PAYLOAD = tlm::tlm_base_protocol_types,
		  class TLM_NOC_OUTPUT_PAYLOAD = tlm_noc::noc_phit_payload,
		  class TLM_GENERIC_PAYLOAD = tlm::tlm_generic_payload,
		  class TLM_PHASE= tlm::tlm_phase,
		  class TLM_SYNC_ENUM = tlm::tlm_sync_enum  >
class TileInterface : public NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> {

public:




	TileInterface(sc_core::sc_module_name name, int id, TileType ty) : NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>(name, id)
		, m_ni_manager("NetworkInterfaceManager", id)
	{
		mTransportLayer = new TransportLayer<NOCPROTOCOLLAYER_PARAM>("TransportLayer", id, &m_ni_manager, ty);


		mNetworkLayer = new NetworkLayer<NOCPROTOCOLLAYER_PARAM> ("NetwotkLayer", id, &m_ni_manager, ty);


		mPhysicalLayer = new PhysicalLayer<NOCPROTOCOLLAYER_PARAM>("PhysicalLayer", id, &m_ni_manager, ty);


		mTransportLayer->setUplayer(this);
		mTransportLayer->setDownlayer(mNetworkLayer);

		mNetworkLayer->setUplayer(mTransportLayer);
		mNetworkLayer->setDownlayer(mPhysicalLayer);

		mPhysicalLayer->setUplayer(mNetworkLayer);
		mPhysicalLayer->setDownlayer(this);

	}

//registers TLM functions



	sync_enum_type nb_transport_fw(int input_ch, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {
		tlm_noc::noc_message_payload* p_message_trans_ext;
		r_trans.get_extension(p_message_trans_ext);
		noc_gp_hdr(p_message_trans_ext, m_src_tile) = getId();
		return mTransportLayer->downstream_nb_transport_fw(input_ch, r_trans,  phase,  t);
	}
	sync_enum_type nb_transport_bw(int input_ch, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {
		return mTransportLayer->downstream_nb_transport_bw(input_ch, r_trans, phase, t);
	}




	//Function Calls

	virtual sync_enum_type downstream_nb_transport_fw(int initiator_id, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {
		return down_layer->downstream_nb_transport_fw(initiator_id, r_trans, phase, t);
	}

	virtual sync_enum_type downstream_nb_transport_bw(int target_id, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {
		return down_layer->downstream_nb_transport_bw(target_id, r_trans, phase, t);
	}


	virtual sync_enum_type upstream_nb_transport_fw(int initiator_id, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {
		return m_bus->nb_transport_fw(initiator_id, r_trans, phase, t);
	}

	virtual sync_enum_type upstream_nb_transport_bw(int initiator_id, transaction_type& r_trans, phase_type& phase, sc_core::sc_time& t) {
		return m_bus->nb_transport_bw(initiator_id, r_trans, phase, t);
	}






public:

	TransportLayer<NR_OF_INPUT_CHANNEL,
	               NR_OF_OUTPUT_CHANNEL,
	               INPUT_CHANNEL_CAPACITY,
				   OUTPUT_CHANNEL_CAPACITY,
	               tlm::tlm_base_protocol_types,
	               tlm::tlm_base_protocol_types,
	               tlm::tlm_generic_payload,
	               tlm::tlm_phase,
	               tlm::tlm_sync_enum>* mTransportLayer;


	NetworkLayer<NR_OF_INPUT_CHANNEL,
	             NR_OF_OUTPUT_CHANNEL,
                 INPUT_CHANNEL_CAPACITY,
				 OUTPUT_CHANNEL_CAPACITY,
                 tlm::tlm_base_protocol_types,
                 tlm::tlm_base_protocol_types,
                 tlm::tlm_generic_payload,
                 tlm::tlm_phase,
                 tlm::tlm_sync_enum>* 	mNetworkLayer;

	PhysicalLayer<NR_OF_INPUT_CHANNEL,
	              NR_OF_OUTPUT_CHANNEL,
                  INPUT_CHANNEL_CAPACITY,
				  OUTPUT_CHANNEL_CAPACITY,
                  tlm::tlm_base_protocol_types,
                  tlm::tlm_base_protocol_types,
                  tlm::tlm_generic_payload,
                  tlm::tlm_phase,
                  tlm::tlm_sync_enum>*   		mPhysicalLayer;

	tlm_noc::NoCNIManager<NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
	               NR_OF_OUTPUT_CHANNEL,
                  INPUT_CHANNEL_CAPACITY,
				  OUTPUT_CHANNEL_CAPACITY,
                  tlm::tlm_base_protocol_types,
                  tlm::tlm_base_protocol_types,
                  tlm::tlm_generic_payload,
                  tlm::tlm_phase,
                  tlm::tlm_sync_enum>,
				  transaction_type,
				  tlm::tlm_base_protocol_types,
				  NR_OF_INPUT_CHANNEL,
	              NR_OF_OUTPUT_CHANNEL,
                  INPUT_CHANNEL_CAPACITY,
				  OUTPUT_CHANNEL_CAPACITY,
                  tlm::tlm_base_protocol_types,
                  tlm::tlm_base_protocol_types,
                  tlm::tlm_generic_payload,
                  tlm::tlm_phase,
                  tlm::tlm_sync_enum >	m_ni_manager;


	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >*                          m_bus;
};


#endif /*TILE_INTERFACE_H_*/
