#pragma once
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

class TileInterface : public NoCProtocolLayer {

public:

	TileInterface(sc_core::sc_module_name name, int id, TileType ty) : NoCProtocolLayer(name, id) {
		mTransportLayer = new TransportLayer("TransportLayer", id, ty);


		mNetworkLayer = new NetworkLayer ("NetwotkLayer", id, ty);


		mPhysicalLayer = new PhysicalLayer("PhysicalLayer", id, ty);


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



	~TileInterface() {
		delete mTransportLayer;
		delete mNetworkLayer;
		delete mPhysicalLayer;
	}


public:

	TransportLayer* mTransportLayer;


	NetworkLayer* 	mNetworkLayer;

	PhysicalLayer*   		mPhysicalLayer;

	NoCConnector*                          m_bus;
};


#endif /*TILE_INTERFACE_H_*/
