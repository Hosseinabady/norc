#ifndef __VPPNOC_SIGNALS_H__
#define __VPPNOC_SIGNALS_H__


class vppnoc_signal_base {

};

template <typename TYPE = bool>
class vppnoc_signal : public vpp_signal_base {

public:
  TYPE value;

};



#endif //__VPPNOC_SIGNALS_H__