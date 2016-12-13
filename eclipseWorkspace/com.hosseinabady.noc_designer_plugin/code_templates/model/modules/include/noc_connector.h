#ifndef NOC_CONNECTOR
#define NOC_CONNECTOR



#include "NoRC/utilities/include/utilities.h"
#include "model/model.h"


template <typename OWN = NoCProtocolLayer< NOCPROTOCOLLAYER_PARAM>, int NR_OF_CHANNEL = NR_OF_CHANNEL_CONST>
class NoCConnector {
private:
	NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM >   *head;
	int                                          port_head;

public:
	NoCConnector() : head(NULL), port_head(0){}
	sync_enum_type nb_transport_bw(int input_ch, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& delay) {
		return head->nb_transport_bw(port_head*NR_OF_CHANNEL+input_ch, r_phit_trans,phase,delay);
	}
	sync_enum_type nb_transport_fw(int input_ch, transaction_type& r_phit_trans, phase_type& phase, sc_core::sc_time& delay) {
		return (head)->nb_transport_fw(port_head*NR_OF_CHANNEL+input_ch, r_phit_trans, phase, delay);
	}

	void setHead(NoCProtocolLayer<NOCPROTOCOLLAYER_PARAM> *h, int p) {
		head = h;
		port_head = p;
	}

/*	void setHead(Router<NOCPROTOCOLLAYER_PARAM> *h, int p) {
		head = h;
		port_head = p;
	}
	void setHead(RTRTile<NOCPROTOCOLLAYER_PARAM> *h, int p) {
		head = h;
		port_head = p;
	}
	void setHead(DummyTile<NOCPROTOCOLLAYER_PARAM> *h, int p) {
		head = h;
		port_head = p;
	}*/

};
#endif //NOC_CONNECTOR



