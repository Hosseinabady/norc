#ifndef NOC_RESOURCE_MAP_H_
#define NOC_RESOURCE_MAP_H_



namespace tlm_noc {

struct PortInfo {
	int port_id;
	int channel_id;
};

struct ConnectionInfo {
	PortInfo from;
	PortInfo to;
};


template <class PAYLOAD_TYPE, int NR_OF_ROUTER_PORT, int NR_OF_CHANNEL_PER_PORT>
class Resource : public std::map<PAYLOAD_TYPE*, ConnectionInfo>  {

public:
	typedef typename Resource<PAYLOAD_TYPE, NR_OF_ROUTER_PORT, NR_OF_CHANNEL_PER_PORT>::iterator ResourceMapIterator;

	Resource () {
		for (int i = 0; i < NR_OF_ROUTER_PORT; i++)
			m_CurrentNumberOfTransaction[i] = 0;
	}

	bool add(PAYLOAD_TYPE& trans, PortInfo to, PortInfo from)	{
		if (m_CurrentNumberOfTransaction[from.port_id]++ < NR_OF_CHANNEL_PER_PORT) {
			const ConnectionInfo info = { from, to};
			assert((*this).find(&trans) == (*this).end());
			(*this)[&trans] = info;
			return true;
		} else {
//			m_CurrentNumberOfTransaction[from]--;
			return false;
		}
	}

	PAYLOAD_TYPE* find_from(PortInfo from)
	{
		typedef typename std::map<PAYLOAD_TYPE*, ConnectionInfo>::iterator IterType;
		IterType iter;
		for( iter = (*this).begin(); iter != (*this).end(); iter++ ) {
			if (iter->second.from.port_id == from.port_id && iter->second.from.channel_id == from.channel_id) {
				return iter->first;
			}
		}
		return 0;
	}
	void erase(ResourceMapIterator it) {

		m_CurrentNumberOfTransaction[it->second.from.port_id]--;

		std::map<PAYLOAD_TYPE*, ConnectionInfo>::erase(it);

		return;
	}
private:
	int m_CurrentNumberOfTransaction[NR_OF_ROUTER_PORT];
};

}

#endif /*NOC_RESOURCE_MAP_H_*/
