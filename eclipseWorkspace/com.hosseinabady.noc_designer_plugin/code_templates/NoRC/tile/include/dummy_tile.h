#ifndef DUMMY_TILE_H_
#define DUMMY_TILE_H_

#include "tlm.h"
#include "NoRC/tile/cores/include/dummy_core.h"
#include "NoRC/tile/cores/include/rtr_core.h"
#include "NoRC/tile/network_interface/include/tile_interface.h"
#include "NoRC/protocol_data_units/include/noc_payload.h"



//template <int NR_OF_CHANNEL , int  INPUT_CHANNEL_CAPACITY, int OUTPUT_CHANNEL_CAPACITY >
//class DummyTile : public NoCTile {
template <int NR_OF_INPUT_CHANNEL = 1,
		  int NR_OF_OUTPUT_CHANNEL = 1,
		  int INPUT_CHANNEL_CAPACITY = 1,
		  int OUTPUT_CHANNEL_CAPACITY = 1,
		  class TLM_NOC_INPUT_PAYLOAD = tlm::tlm_base_protocol_types,
		  class TLM_NOC_OUTPUT_PAYLOAD = tlm_noc::noc_phit_payload,
		  class TLM_GENERIC_PAYLOAD = tlm::tlm_generic_payload,
		  class TLM_PHASE= tlm::tlm_phase,
		  class TLM_SYNC_ENUM = tlm::tlm_sync_enum  >
class DummyTile : public NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> {
public:


	DummyTile(sc_core::sc_module_name name, int Id, TileType ty) :
		NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>(name, Id),
		m_DummyCore("DummyCore"),
		m_Interface("Interface", Id, ty),
		m_Id(Id)
	{
		m_DummyCore.initiator_socket(m_Interface.bus_target_socket);
		m_Interface.bus_initiator_socket(m_DummyCore.target_socket);

		m_Interface.setDownlayer(this);
		setDownlayer(&m_Interface);
	}

	~DummyTile() {}

	sync_enum_type router_nb_transport_fw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t)
	{
		return up_layer->upstream_nb_transport_fw(initiator_id, r_phit_trans, phase, t);
	}

	sync_enum_type router_nb_transport_bw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t)
	{
		return up_layer->upstream_nb_transport_bw(initiator_id, r_phit_trans, phase, t);
	}

	sync_enum_type downstream_nb_transport_fw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t)
	{
		return m_port->nb_transport_fw(initiator_id, r_phit_trans, phase, t);
	}

	sync_enum_type downstream_nb_transport_bw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t)
	{
		return m_port->nb_transport_bw(initiator_id, r_phit_trans, phase, t);
	}




//private:
	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >*                           m_port;
	DummyCore	                                            m_DummyCore;
	TileInterface<NR_OF_INPUT_CHANNEL, NR_OF_OUTPUT_CHANNEL, INPUT_CHANNEL_CAPACITY, OUTPUT_CHANNEL_CAPACITY,
				  tlm::tlm_base_protocol_types,
				  tlm::tlm_base_protocol_types,
				  tlm::tlm_generic_payload,
				  tlm::tlm_phase,
				  tlm::tlm_sync_enum>		   m_Interface;
	const unsigned int                         m_Id;
};


#endif /*DUMMY_TILE_H_*/
