#ifndef RTR_TILE_H
#define RTR_TILE_H

#include "model/include/modelutilities.h"
#include "tlm.h"
#include "model/modules/include/noc_tile.h"
#include "NoRC/tile/cores/include/rtr_core.h"
#include "NoRC/tile/network_interface/include/tile_interface.h"
#include "model/modules/include/noc_mem.h"
#include "model/modules/include/noc_ambaahb_bus.h"


//template <int NR_OF_CHANNEL , int  INPUT_CHANNEL_CAPACITY, int OUTPUT_CHANNEL_CAPACITY >
//class RTRTile : public NoCTile { //Run-Time Reconfigurable Tile
template <int NR_OF_INPUT_CHANNEL = 1,
		  int NR_OF_OUTPUT_CHANNEL = 1,
		  int INPUT_CHANNEL_CAPACITY = 1,
		  int OUTPUT_CHANNEL_CAPACITY = 1,
		  class TLM_NOC_INPUT_PAYLOAD = tlm::tlm_base_protocol_types,
		  class TLM_NOC_OUTPUT_PAYLOAD = tlm_noc::noc_phit_payload,
		  class TLM_GENERIC_PAYLOAD = tlm::tlm_generic_payload,
		  class TLM_PHASE= tlm::tlm_phase,
		  class TLM_SYNC_ENUM = tlm::tlm_sync_enum  >
class RTRTile : public NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> {
public:

	RTRTile(sc_core::sc_module_name name, int id, TileType ty) :
		NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>(name, id)
	{
		p_AHBBus = new AMBAAHBBus<NOCPROTOCOLLAYER_PARAM>("AMBA_AHB_BUS", id);
		p_Mem = new LocalMemory("Local_Memory", id);
		m_core_connector = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>, NR_OF_CHANNEL_CONST>[MAX_RTR_MODULE+2];
		m_bus_connector = new NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>, NR_OF_CHANNEL_CONST>[MAX_RTR_MODULE+2];
		m_Interface = new TileInterface<TILE_TEMPLATE_PARAMETER>("Interface", id, ty);

		for ( int i = 0; i < MAX_RTR_MODULE; i++) {
			char txt[20];
			sprintf(txt, "RTRCore_%d", i);
			p_RTRCore[i] = new RTRCore<NOCPROTOCOLLAYER_PARAM>(txt, i);
			p_RTRCore[i]->m_bus = &m_core_connector[i];
			m_core_connector[i].setHead(p_AHBBus, i);
			p_AHBBus->m_core[i] = p_RTRCore[i];
//			p_AHBBus->m_core[i]->m_bus = &m_bus_connector[i];
//			m_bus_connector[i].setHead(p_RTRCore[i], i);
		}



		p_Mem->m_bus = &m_core_connector[MAX_RTR_MODULE];
		m_core_connector[MAX_RTR_MODULE].setHead(p_AHBBus, MAX_RTR_MODULE);
		
//		p_AHBBus->m_core[MAX_RTR_MODULE]->m_bus = &m_bus_connector[MAX_RTR_MODULE];
//		m_bus_connector[MAX_RTR_MODULE].setHead(p_Mem, MAX_RTR_MODULE);

		m_Interface->m_bus = &m_core_connector[MAX_RTR_MODULE+1];
		m_core_connector[MAX_RTR_MODULE+1].setHead(p_AHBBus, MAX_RTR_MODULE+1);
		p_AHBBus->m_core[MAX_RTR_MODULE+1] = m_Interface;
		
//		p_AHBBus->m_core[MAX_RTR_MODULE+1]->m_bus = &m_bus_connector[MAX_RTR_MODULE+1];
//		m_bus_connector[MAX_RTR_MODULE+1].setHead(p_Mem, MAX_RTR_MODULE+1);

		m_Interface->setDownlayer(this);
		setUplayer(m_Interface->mPhysicalLayer);
		m_3d_layer = m_Interface->mTransportLayer;

	}


    
	virtual sync_enum_type nb_transport_fw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t)
	{
		return up_layer->upstream_nb_transport_fw(initiator_id, r_phit_trans, phase, t);
	}

	virtual sync_enum_type nb_transport_bw(int initiator_id, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& t)
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

	sync_enum_type nb_external_transport_fw(char* FName) {
		return m_3d_layer->nb_external_transport_fw(FName);
	}


	~RTRTile() {}

//private:
	RTRCore<NOCPROTOCOLLAYER_PARAM>*				                                  p_RTRCore[MAX_RTR_MODULE];
	AMBAAHBBus<NOCPROTOCOLLAYER_PARAM>*                                               p_AHBBus;
	LocalMemory*                                                                      p_Mem;
	TileInterface<TILE_TEMPLATE_PARAMETER>*	                                          m_Interface;
	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> >*                          m_port;
	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>, NR_OF_CHANNEL_CONST>*                        m_core_connector;
	NoCConnector<NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM>, NR_OF_CHANNEL_CONST>*                        m_bus_connector;
	NoCProtocolLayer<PRTOCOL_LAYER_TEMPLATE_PARAMETER>*	                              m_3d_layer;

};


#endif /*RTR_TILE_H*/
