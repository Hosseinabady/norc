/*
 * noc_generic_payload_inc.h
 *
 *  Created on: Aug 26, 2008
 *      Author: forever
 */

#ifndef NOC_GENERIC_PAYLOAD_INC_H_
#define NOC_GENERIC_PAYLOAD_INC_H_
template <typename HEADER, typename BUDDY, int size, typename HEADER2, typename BUDDY2, int size2>
noc_generic_payload<HEADER, BUDDY, size>& operator<<  (noc_generic_payload<HEADER, BUDDY, size>& left,const noc_generic_payload<HEADER2, BUDDY2, size2>& right)
{
	unsigned int free_space = left.pdu_size - left.stream_head;
	unsigned int fill_capacity = right.sdu_size;
	unsigned int nbytes = free_space > fill_capacity ? fill_capacity : free_space;

	memcpy(&(left.view_as.stream[left.stream_head]), (char*)(right.view_as.pdu.body), nbytes);
	left.stream_head += nbytes;

	if (left.stream_head == left.pdu_size) {
		left.stream_head = 0;
		noc_generic_payload<HEADER, BUDDY, size>* tmp = 0;
		return *tmp;
	} else {
		return left;
	}
}

template <typename HEADER, typename BUDDY, int size, typename HEADER2, typename BUDDY2, int size2>
noc_generic_payload<HEADER, BUDDY, size>& operator>>  (noc_generic_payload<HEADER, BUDDY, size>& left,const noc_generic_payload<HEADER2, BUDDY2, size2>& right)
{
	unsigned int free_space = right.sdu_size;
	unsigned int fill_capacity = left.pdu_size - left.stream_tail;
	unsigned int nbytes = ((free_space > fill_capacity) ? fill_capacity : free_space);

	memcpy((char*)(right.view_as.pdu.body),&(left.view_as.stream[left.stream_tail]),nbytes);

	left.stream_tail += nbytes;

	if (left.stream_tail == left.pdu_size) {
		left.stream_tail = 0;
		noc_generic_payload<HEADER, BUDDY, size>* tmp = 0;
		return *tmp;
	} else {
		return left;
	}
}


#endif /* NOC_GENERIC_PAYLOAD_INC_H_ */


