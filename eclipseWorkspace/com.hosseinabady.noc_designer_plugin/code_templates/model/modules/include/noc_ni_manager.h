#ifndef NOC_NI_MANAGER
#define NOC_NI_MANAGER



#include <vector>
#include <systemc>
#include <tlm.h>

extern std::ofstream trafficLogFile;
namespace tlm_noc {

template <typename OWNER, typename transaction_type, typename TYPES=tlm::tlm_base_protocol_types,
		  int NR_OF_INPUT_CHANNEL = 1,
		  int NR_OF_OUTPUT_CHANNEL = 1,
          int INPUT_CHANNEL_CAPACITY = 1,
		  int OUTPUT_CHANNEL_CAPACITY = 1,
          class TLM_NOC_INPUT_PAYLOAD = tlm::tlm_base_protocol_types,
          class TLM_NOC_OUTPUT_PAYLOAD = tlm::tlm_base_protocol_types,
          class TLM_GENERIC_PAYLOAD = tlm::tlm_generic_payload,
          class TLM_PHASE= tlm::tlm_phase,
          class TLM_SYNC_ENUM = tlm::tlm_sync_enum>
class NoCNIManager :public sc_core::sc_module {
public:


	typedef typename TYPES::tlm_payload_type    tlm_payload_type;
	typedef typename TYPES::tlm_phase_type      tlm_phase_type;
	struct payload_unit {
		NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
						 NR_OF_OUTPUT_CHANNEL,
						 INPUT_CHANNEL_CAPACITY,
						 OUTPUT_CHANNEL_CAPACITY,
						 TLM_NOC_INPUT_PAYLOAD,
						 TLM_NOC_OUTPUT_PAYLOAD,
						 TLM_GENERIC_PAYLOAD,
						 TLM_PHASE,
						 TLM_SYNC_ENUM>*  layer;
		tlm_payload_type*  trans;
		tlm_phase_type     phase;
		int                channel;
		unsigned int       part;
		payload_unit(NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
									  NR_OF_OUTPUT_CHANNEL,
								      INPUT_CHANNEL_CAPACITY,
								      OUTPUT_CHANNEL_CAPACITY,
								      TLM_NOC_INPUT_PAYLOAD,
								      TLM_NOC_OUTPUT_PAYLOAD,
								      TLM_GENERIC_PAYLOAD,
								      TLM_PHASE,
								      TLM_SYNC_ENUM>* l, tlm_payload_type*  t, tlm_phase_type     p, int c, unsigned int pa):
		layer(l), trans(t), phase(p), channel(c), part(pa){}
		payload_unit(){}

	};

	typedef  payload_unit                   PAYLOAD;

	typedef void (OWNER::*function_cb)(transaction_type* p_trans, unsigned int ch_no);



public:
	class delta_list {
	public:

		delta_list() {
			reset();
			entries.resize(100);
		}

		inline void insert(const PAYLOAD& p) {
			if (size == entries.size()) {
				entries.resize(entries.size()*2);
			}
			entries[size++] = p;
		}
		inline PAYLOAD& get() {
			return entries[out++];
		}

		inline bool next() {
			return out < size;
		}

		inline void reset() {
			size = 0;
			out = 0;
		}

	public:
		unsigned int size;

	private:
		std::vector<PAYLOAD> entries;
		unsigned int         out;
	};



	class waiting_list {
	public:
		struct waited_element {
			struct waited_element   *next;
			PAYLOAD                  p;
			bool                     active;
			waited_element(PAYLOAD& p, bool a) : p(p), active(a) {}
			waited_element(){}
		};
		waited_element   *nill;
		waited_element   *empties;
		waited_element   *list;
		unsigned int      size;
		int               active_status;


		waiting_list() : nill(new waited_element()), empties(NULL), list(nill),size(0),active_status(0) {}

		~waiting_list() {
			while(size) {
				delete_top();
			}

			while(empties) {
				struct waited_element *e = empties->next;
				delete empties;
				empties = e;
			}
			delete nill;
		}
		void insert(const PAYLOAD & p) {
			if (!empties) {
				empties = new struct waited_element();
				empties->next = NULL;
			}
			struct waited_element *e = empties;
			empties = empties->next;
			e->p = p;


			struct waited_element *ancestor = nill;
			struct waited_element *iterator = list;
			while(iterator != nill) {
				ancestor = iterator;
				iterator = iterator->next;
			}
			if (ancestor == nill) {
				e->next = list;
				list = e;
			} else {
				e->next = iterator;
				ancestor->next=e;
			}
			size++;
		}

		void delete_top() {
			if (list != nill) {
				struct waited_element *e=list;
				list = list->next;
				e->next = empties;
				empties = e;
				size--;
			}
		}

		void delete_top_action() {
			struct waited_element *iterator = list;
			while(iterator != nill) {
				if (iterator->active == true) {
					if (list != nill) {
						struct waited_element *e=list;
						list = list->next;
						e->next = empties;
						empties = e;
						size--;
						return;
					}
				}
				iterator = iterator->next;
			}
		}
		unsigned int get_size() {
			return size;
		}

		PAYLOAD &top_active() {
			struct waited_element *iterator = list;
			while(iterator != nill) {
				if (iterator->active == true)
					return iterator->p;
				iterator = iterator->next;
			}
		}

		sc_core::sc_time next_time() {
			return list->next->t;
		}


		int get_active_status() {
			return active_status;
		}
		void add_active_status() {
			active_status++;
		}
		void rm_active_status() {
			active_status--;
		}

		void do_activate(int ch) {
			struct waited_element *iterator = list;
			while(iterator != nill) {
				if (iterator->p.channel == ch) {
					iterator->active = true;
					add_active_status();
				}
				iterator = iterator->next;
			}
		}
	};
	NoCNIManager(sc_core::sc_module_name name, int id) : sc_core::sc_module(name), m_id(id), number_of_cbfunction(0)
	{
		sc_core::sc_spawn_options opts;
		opts.spawn_method();
		opts.set_sensitivity(&m_e);
		opts.dont_initialize();
		sc_core::sc_spawn(sc_bind(&NoCNIManager::main_action, this), sc_core::sc_gen_unique_name("NoCNIManager"), &opts);
	}
	~NoCNIManager(){}

	int register_activity(function_cb f_cb) {

		m_call_back_functions[number_of_cbfunction] = f_cb;
		return number_of_cbfunction++;
	}

	void notify(NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
								 NR_OF_OUTPUT_CHANNEL,
								 INPUT_CHANNEL_CAPACITY,
								 OUTPUT_CHANNEL_CAPACITY,
								 TLM_NOC_INPUT_PAYLOAD,
								 TLM_NOC_OUTPUT_PAYLOAD,
								 TLM_GENERIC_PAYLOAD,
								 TLM_PHASE,
								 TLM_SYNC_ENUM>*  la, tlm_payload_type& t, tlm_phase_type& p, const sc_core::sc_time& when, const int& channel, const unsigned int& part) {
		if (when == sc_core::SC_ZERO_TIME) {
			if (sc_core::sc_delta_count() & (sc_dt::uint64)0x1) {
#ifdef DEBUG
				trafficLogFile << name() << " odd notify " <<  std::endl;
#endif
				m_even_delta.insert(PAYLOAD(la, &t, p, channel, part));
			} else {
#ifdef DEBUG
				trafficLogFile << name() << " even notify " <<  std::endl;
#endif
				m_uneven_delta.insert(PAYLOAD(la, &t, p, channel, part));
			}
			m_e.notify(sc_core::SC_ZERO_TIME);
		} else {
#ifdef DEBUG
			trafficLogFile << name() << " non zero notify size = " << m_ppq.get_size() << std::endl;
#endif
			m_ppq.insert(PAYLOAD(la, &t, p, channel, part), when+sc_core::sc_time_stamp());
			m_e.notify();
		}
	}

	void notify (NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
								  NR_OF_OUTPUT_CHANNEL,
								  INPUT_CHANNEL_CAPACITY,
								  OUTPUT_CHANNEL_CAPACITY,
								  TLM_NOC_INPUT_PAYLOAD,
								  TLM_NOC_OUTPUT_PAYLOAD,
								  TLM_GENERIC_PAYLOAD,
								  TLM_PHASE,
								  TLM_SYNC_ENUM>*  la, tlm_payload_type& t, tlm_phase_type& p, const int& channel, const unsigned int& part) {
		m_immediate_yield.insert(PAYLOAD(la, &t, p, channel, part));
		m_e.notify();
	}

	void notify(NoCProtocolLayer<NR_OF_INPUT_CHANNEL,
								 NR_OF_OUTPUT_CHANNEL,
								 INPUT_CHANNEL_CAPACITY,
								 OUTPUT_CHANNEL_CAPACITY,
								 TLM_NOC_INPUT_PAYLOAD,
								 TLM_NOC_OUTPUT_PAYLOAD,
								 TLM_GENERIC_PAYLOAD,
								 TLM_PHASE,
								 TLM_SYNC_ENUM>*  la, tlm_payload_type& t, const int& channel, const unsigned int& part) {
		if (part % (NUMBER_OF_CALLBACK_FUNCTION + 2) == 0) { // PHYSICAL_MAIN_ACTION_WAIT = 0
			m_wait_list.insert(PAYLOAD(la, &t, 0, channel, part));
			trafficLogFile << name() << " notify PHYSICAL_MAIN_ACTION_WAIT = " << std::endl;
		} else if (part % (NUMBER_OF_CALLBACK_FUNCTION + 2) == 1 ) { // PHYSICAL_WAIT_LIST_ACTIVE = 1
			trafficLogFile << name() << " notify PHYSICAL_WAIT_LIST_ACTIVE = " << std::endl;
			m_wait_list.do_activate(channel);
		}
		m_e.notify();
	}

	void main_action() {
#ifdef DEBUG
		trafficLogFile << name() << " main_action " << " m_ppq.get_size() = " << m_ppq.get_size()  
			<< " m_wait_list.get_active_status() = " << m_wait_list.get_active_status()
			<< " m_wait_list.get_size() = " << m_wait_list.get_size() <<  std::endl;
#endif
		
		while(m_wait_list.get_active_status()) {
			PAYLOAD& tmp = m_wait_list.top_active();
			trafficLogFile << name() << " 1 tmp.part = " << tmp.part << std::endl;
			(tmp.layer->*m_call_back_functions[tmp.part])(tmp.trans, tmp.channel);
			m_wait_list.delete_top_action();
			m_wait_list.rm_active_status();
		}

		while(m_immediate_yield.next()) {
			PAYLOAD& tmp = m_immediate_yield.get();
			trafficLogFile << name() << " 2 tmp.part = " << tmp.part << std::endl;
			(tmp.layer->*m_call_back_functions[tmp.part])(tmp.trans, tmp.channel);
		}
		m_immediate_yield.reset();

		if (sc_core::sc_delta_count() & (sc_dt::uint64) 0x1) {
			while (m_uneven_delta.next()) {
				PAYLOAD& tmp = m_uneven_delta.get();
				trafficLogFile << name() << " 3 tmp.part = " << tmp.part << std::endl;
				(tmp.layer->*m_call_back_functions[tmp.part])(tmp.trans, tmp.channel);
			}
			m_uneven_delta.reset();
			if (m_even_delta.size) m_e.notify(sc_core::SC_ZERO_TIME);
		} else {
			while (m_even_delta.next()) {
				PAYLOAD& tmp = m_even_delta.get();
				trafficLogFile << name() << " 4 tmp.part = " << tmp.part << std::endl;
				(tmp.layer->*m_call_back_functions[tmp.part])(tmp.trans, tmp.channel);
			}
			m_even_delta.reset();
			if (m_uneven_delta.size) m_e.notify(sc_core::SC_ZERO_TIME);
		}

		if(m_ppq.get_size()){
			
			const sc_core::sc_time now = sc_core::sc_time_stamp();
			sc_core::sc_time top = m_ppq.top_time();

			while (m_ppq.get_size() && top == now) {
				PAYLOAD& tmp = m_ppq.top();
				trafficLogFile << name() << " 5 tmp.part = " << tmp.part << " m_ppq.get_size() = " << m_ppq.get_size() << std::endl;
				(tmp.layer->*m_call_back_functions[tmp.part])(tmp.trans, tmp.channel);
				m_ppq.delete_top();
				top = m_ppq.top_time();
			}

			if (m_ppq.get_size()) {
				m_e.notify(top - now);
			}
		}
		trafficLogFile << name() << " end of main " << std::endl;		
	}




private:
	OWNER*                        m_owner;

	time_ordered_list<PAYLOAD>    m_ppq;
	delta_list                    m_uneven_delta;
	delta_list                    m_even_delta;
	delta_list                    m_immediate_yield;
	waiting_list                  m_wait_list;

	std::map<int, function_cb>    m_call_back_functions;
	int                           number_of_cbfunction;
	sc_core::sc_event             m_e;
	int                           m_id;

};
}
#endif //NOC_NI_MANAGER
