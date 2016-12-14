#ifndef NOC_GENERIC_PAYLOAD_
#define NOC_GENERIC_PAYLOAD_

#include <systemc>

#include "protocol_data_unit/vppnoc_pdu_base.h"
namespace vppnoc {

template <typename HEADER, typename BODY = bool, int size = 10>
class vppnoc_pdu : public vppnoc_pdu_base
{
public:
  vppnoc_pdu():  vppnoc_pdu_base(), stream_tail(0),
	stream_head(0) {
	 }

	 friend vppnoc_pdu<HEADER, BODY, size>& operator<< (vppnoc_pdu& left, const vppnoc_pdu& right);
	 friend vppnoc_pdu<HEADER, BODY, size>& operator>> (vppnoc_pdu& left, const vppnoc_pdu& right);

 public:
    enum {pci_size = sizeof(HEADER)};
    enum {sdu_size = size * sizeof(BODY)};
    enum {pdu_size = sizeof(HEADER) + size * sizeof(BODY)};
	void reset() {
		stream_tail = 0;
		stream_head = 0;
	}

    union
    {
        struct
        {
            HEADER hdr;
            BODY   body[size];
        } pdu;
        bool stream[pdu_size];
    } view_as;
    unsigned int stream_tail;
    unsigned int stream_head;
};

template <typename HEADER, typename BODY, int size, typename HEADER2, typename BODY2, int size2>
vppnoc_pdu<HEADER, BODY, size>& operator<<  (vppnoc_pdu<HEADER, BODY, size>& left,const vppnoc_pdu<HEADER2, BODY2, size2>& right)
{
	unsigned int free_space = left.pdu_size - left.stream_head;
	unsigned int fill_capacity = right.sdu_size;
	unsigned int nbytes = free_space > fill_capacity ? fill_capacity : free_space;

	memcpy(&(left.view_as.stream[left.stream_head]), (char*)(right.view_as.pdu.body), nbytes);
	left.stream_head += nbytes;

	if (left.stream_head == left.pdu_size) {
		left.stream_head = 0;
		vppnoc_pdu<HEADER, BODY, size>* tmp = 0;
		return *tmp;
	} else {
		return left;
	}
}

template <typename HEADER, typename BODY, int size, typename HEADER2, typename BODY2, int size2>
vppnoc_pdu<HEADER, BODY, size>& operator>>  (vppnoc_pdu<HEADER, BODY, size>& left,const vppnoc_pdu<HEADER2, BODY2, size2>& right)
{
	unsigned int free_space = right.sdu_size;
	unsigned int fill_capacity = left.pdu_size - left.stream_tail;
	unsigned int nbytes = ((free_space > fill_capacity) ? fill_capacity : free_space);

	memcpy((char*)(right.view_as.pdu.body),&(left.view_as.stream[left.stream_tail]),nbytes);

	left.stream_tail += nbytes;

	if (left.stream_tail == left.pdu_size) {
		left.stream_tail = -1;
		vppnoc_pdu<HEADER, BODY, size>* tmp = 0;
		return *tmp;
	} else {
		return left;
	}
}



}

#endif /*NOC_GENERIC_PAYLOAD_*/
