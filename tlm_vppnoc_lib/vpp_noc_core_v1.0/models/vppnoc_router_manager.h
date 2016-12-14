#ifndef NOC_ROUTER_MANAGER_H
#define NOC_ROUTER_MANAGER_H



#include <vector>
#include <systemc>
#include <tlm.h>



namespace tlm_noc {

template <typename OWNER, typename TYPES=tlm::tlm_base_protocol_types>
class NoCRouterManager :public vppnoc_module {
public:


	typedef typename TYPES::tlm_payload_type    tlm_payload_type;
	typedef typename TYPES::tlm_phase_type      tlm_phase_type;
	struct payload_unit {
		tlm_payload_type*  trans;
		tlm_phase_type     phase;
		int                channel;
		unsigned int   part;
		payload_unit(tlm_payload_type*  t, tlm_phase_type     p, int c, unsigned int pa):
		trans(t), phase(p), channel(c), part(pa) {}
		payload_unit(){}

	};

	typedef  payload_unit                   PAYLOAD;

	typedef void (OWNER::*function_cb)(transaction_type& trans, unsigned int ch_no);

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

		bool is_exist(const PAYLOAD& p) {
			for (unsigned int i = 0; i < entries.size(); i++)
				if (entries[i].trans == p.trans &&
					entries[i].channel == p.channel &&
					entries[i].part == p.part)
					return true;
			return false;
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
			waited_element(){next = NULL; active = false;}
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
			e->active = false;


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
		bool is_exist(const PAYLOAD& p) {
			struct waited_element *iterator = list;
			while (iterator != nill) {
				if (iterator->p.trans == p.trans &&
					iterator->p.channel == p.channel &&
					iterator->p.part == p.part)
					return true;
				iterator = iterator->next;
			}
			return false;
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

		void delete_top_active() {
			struct waited_element *ancestor = nill;
			struct waited_element *iterator = list;
			while(iterator != nill) {
				if (iterator->active == true) {
					if (ancestor == nill) {
						struct waited_element *e=list;
						list = list->next;
						e->active = false;
						e->next = empties;
						empties = e;
						size--;
						return;
					} else {
						ancestor->next = iterator->next;
						struct waited_element *e=iterator;
						e->active = false;
						e->next = empties;
						empties = e;
						size--;
						return;
					}
				}
				ancestor = iterator;
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
					if (iterator->active == false) {
						iterator->active = true;
						add_active_status();
					}
				}
				iterator = iterator->next;
			}
		}
	};

	NoCRouterManager(sc_core::sc_module_name name, OWNER* _owner, int id) : vppnoc_module(name, id), m_owner(_owner), number_of_cbfunction(0)
	{
		sc_core::sc_spawn_options opts;
		opts.spawn_method();
		opts.set_sensitivity(&m_e);
		opts.dont_initialize();
		sc_core::sc_spawn(sc_bind(&NoCRouterManager::main_action, this), sc_core::sc_gen_unique_name("NoCRouterManager"), &opts);
	}
	~NoCRouterManager(){}

	int register_activity(function_cb f_cb) {
		call_back_functions[number_of_cbfunction] = f_cb;
		return number_of_cbfunction++;
	}



	void notify (tlm_payload_type& r_trans, tlm_phase_type& p, const sc_core::sc_time& when, const int& channel, const unsigned int& part) {
		//r_trans.acquire();
		if (when == sc_core::SC_ZERO_TIME) {
			if (sc_core::sc_delta_count() & (sc_dt::uint64)0x1)
				m_even_delta.insert(PAYLOAD(&r_trans, p, channel, part));
			else
				m_uneven_delta.insert(PAYLOAD(&r_trans, p, channel, part));
			m_e.notify(sc_core::SC_ZERO_TIME);
		} else {
			m_ppq.insert(PAYLOAD(&r_trans, p, channel, part), when+sc_core::sc_time_stamp());
			m_e.notify();
		}
	}

	void notify (tlm_payload_type& r_trans, tlm_phase_type& p, const int& channel, const unsigned int& part) {
		//r_trans.acquire();
		m_immediate_yield.insert(PAYLOAD(&r_trans, p, channel, part));
		m_e.notify();
	}
	void notify (tlm_payload_type& r_trans, const int& channel, const unsigned int& part) {
		//r_trans.acquire();
		if (part == 0) {
			m_wait_list.insert(PAYLOAD(&r_trans, 0, channel, part));
		} else if (part == 1) {
			m_wait_list.do_activate(channel);
		}
		m_e.notify();
	}
	void main_action() {
		while(m_wait_list.get_active_status()) {
			PAYLOAD& tmp = m_wait_list.top_active();
			(m_owner->*call_back_functions[tmp.part])(tmp.trans, tmp.channel);
			m_wait_list.delete_top_active();
			m_wait_list.rm_active_status();
		//	tmp.trans->release();
		}

		while(m_immediate_yield.next()) {
			PAYLOAD& tmp = m_immediate_yield.get();
			(m_owner->*call_back_functions[tmp.part])(tmp.trans, tmp.channel);
		//	tmp.trans->release();
		}
		m_immediate_yield.reset();

		if (sc_core::sc_delta_count() & (sc_dt::uint64) 0x1) {
			while (m_uneven_delta.next()) {
				PAYLOAD& tmp = m_uneven_delta.get();
				(m_owner->*call_back_functions[tmp.part])(tmp.trans, tmp.channel);
			//	tmp.trans->release();
			}
			m_uneven_delta.reset();
			if (m_even_delta.size) m_e.notify(sc_core::SC_ZERO_TIME);
		} else {
			while (m_even_delta.next()) {
				PAYLOAD& tmp = m_even_delta.get();
				(m_owner->*call_back_functions[tmp.part])(tmp.trans, tmp.channel);
			//	tmp.trans->release();
			}
			m_even_delta.reset();
			if (m_uneven_delta.size) m_e.notify(sc_core::SC_ZERO_TIME);
		}

		if(!m_ppq.get_size()) return;
		const sc_core::sc_time now = sc_core::sc_time_stamp();
		sc_core::sc_time top = m_ppq.top_time();
		if (top < now) {
			std::cout << " Fatal Error: A packet is behind the schedule in router manager" <<std::endl;
			assert(false); exit(1);
		}
		while (m_ppq.get_size() && top == now) {
			PAYLOAD& tmp = m_ppq.top();
			(m_owner->*call_back_functions[tmp.part])(tmp.trans, tmp.channel);
		//	tmp.trans->release();
			m_ppq.delete_top();
			top = m_ppq.top_time();
		}

		if (m_ppq.get_size()) {
			m_e.notify(top - now);
		}
	}
private:

	OWNER*                        m_owner;
	waiting_list                  m_wait_list;
	time_ordered_list<PAYLOAD>    m_ppq;
	delta_list                    m_uneven_delta;
	delta_list                    m_even_delta;
	delta_list                    m_immediate_yield;

	std::map<int, function_cb>    call_back_functions;
	int                           number_of_cbfunction;
	sc_core::sc_event             m_e;

};
}
#endif //NOC_ROUTER_MANAGER_H
