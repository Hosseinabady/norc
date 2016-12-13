#pragma once
#ifndef VPPNOC_UTILITIES_H
#define VPPNOC_UTILITIES_H



#define error_std std::cout


namespace vppnoc {



typedef  bool  vppnoc_signal;
typedef  bool  vppnoc_signal_value;
class vppnoc_payload_base;

#define  NR_OF_CHANNEL_CONST                                                       2


enum TileTypeEnum   {DUMMY_TILE, IO_TILE = 0, INTER_TILE};

enum vppnoc_response_status {
    VPPNOC_OK_RESPONSE = 1,
    VPPNOC_INCOMPLETE_RESPONSE = 0,
    VPPNOC_GENERIC_ERROR_RESPONSE = -1,
    VPPNOC_ADDRESS_ERROR_RESPONSE = -2,
    VPPNOC_COMMAND_ERROR_RESPONSE = -3,
    VPPNOC_BURST_ERROR_RESPONSE = -4,
    VPPNOC_BYTE_ENABLE_ERROR_RESPONSE = -5
};


typedef    TileTypeEnum                 TileType;



//*****************************************************
//             Duff's devise                          *
//*****************************************************
#define cr_context   int __s;
#define cr_init()    __s = 0;
#define cr_start()   switch (__s) { case 0:
#define cr_return(x) { __s = __LINE__; return x; case __LINE__: ; }
#define cr_end()     { break; default: for (;;) ; } } __s = 0; 
#define cr_reset()   __s = 0;

#define UNDERCONSTRUCTION std::cout << "under construction in function " << __FUNCTION__ << " at line " << __LINE__ << std::endl;
}

#endif //VPPNOC_UTILITIES_H