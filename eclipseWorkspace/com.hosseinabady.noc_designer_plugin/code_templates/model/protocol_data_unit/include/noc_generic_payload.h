#ifndef NOC_GENERIC_PAYLOAD_
#define NOC_GENERIC_PAYLOAD_

namespace tlm_noc {

template <typename HEADER, typename BUDDY, int size>
class noc_generic_payload
{
 public:

	 noc_generic_payload():  stream_tail(0),
	 stream_head(0){

	 }
	 friend noc_generic_payload<HEADER, BUDDY, size>& operator<< (noc_generic_payload& left, const noc_generic_payload& right);
	 friend noc_generic_payload<HEADER, BUDDY, size>& operator>> (noc_generic_payload& left, const noc_generic_payload& right);

 public:
    enum {pci_size = sizeof(HEADER)};
    enum {sdu_size = size * sizeof(BUDDY)};
    enum {pdu_size = sizeof(HEADER) + size * sizeof(BUDDY)};
	void reset() {
		stream_tail = 0;
		stream_head = 0;
	}

    union
    {
        struct
        {
            HEADER hdr;
            BUDDY  body[size];
        } pdu;
        char stream[pdu_size];
    } view_as;
    unsigned int stream_tail;
    unsigned int stream_head;
};

#include "noc_generic_payload_inc.h"
}

#endif /*NOC_GENERIC_PAYLOAD_*/
