#ifndef VPPNOC_TYPES_H
#define VPPNOC_TYPES_H

/**
 *     VPPNOC protocol types.
 */

/* Namespaces */

namespace vppnoc {

	/*Forwards*/
class vppnoc_payload_base;


enum vppnoc_phase_enum { UNINITIALIZED_PHASE=0, BEGIN_REQ = 1, END_REQ, BEGIN_RESP, END_RESP };
enum vppnoc_sync_enum { VPPNOC_ACCEPTED, VPPNOC_UPDATED, VPPNOC_COMPLETED, VPPNOC_REJECTED };

struct vppnoc_protocol_types {
    typedef vppnoc_payload_base      vppnoc_payload_type;
    typedef vppnoc_phase_enum        vppnoc_phase_type;
};


typedef vppnoc_protocol_types::vppnoc_payload_type      vppnoc_transaction;
typedef vppnoc_protocol_types::vppnoc_phase_type        vppnoc_phase;

typedef vppnoc_sync_enum                        vppnoc_sync_enum_type;

}   /* namespace amba_pv */

#endif  /* defined(AMBA_PV_TYPES__H) */
