#ifndef VPPNOC_MICROSCHEDULER_H
#define VPPNOC_MICROSCHEDULER_H

//#define USE_LOCAL_TIME

#include <vector>
#include <map>
#include <systemc>                                           

using namespace std;
using namespace sc_core;
using namespace sc_dt;


namespace vppnoc {

enum notify_type {JUST_NOTIFY, WAIT_ON_TIME, WAIT_ON_SIGNAL};
template <typename OWNER_TYPE, typename TYPES>
class vppnoc_process;

using namespace sc_core;
template <typename OWNER_TYPE = NoCProtocolLayer, typename TYPES=vppnoc_transaction>
class VPPNOCMicroScheduler :public sc_module {
public:
	typedef  typename TYPES    payload_type;
	typedef int (OWNER_TYPE::*function_cb)(TYPES& trans, sc_time & r_delay);

	struct payload_unit {
   	payload_type*           trans;
		vppnoc_signal*          signal;
		vppnoc_signal_value     value;
		vppnoc_process<OWNER_TYPE, TYPES>* process;
		function_cb             function;  
		payload_unit(payload_type*  t, vppnoc_signal* sig,vppnoc_signal_value val , vppnoc_process<OWNER_TYPE, TYPES>* prc, function_cb fp):
    trans(t), signal(sig), value(val), process(prc), function(fp){}
		payload_unit(){}
	};

	typedef  payload_unit                   PAYLOAD;

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
		vector<PAYLOAD> entries;
		unsigned int         out;
	};



	class waiting_list {
	public:
		struct waited_element {
			struct waited_element   *next;
			PAYLOAD                  p;
			bool                     active;
			waited_element(PAYLOAD& p, bool a) : p(p), active(a) {}
			waited_element(): active(false){}
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
			struct waited_element *ancestor = nill;
			struct waited_element *iterator = list;
			while(iterator != nill) {
				if (iterator->active == true) {
					if (ancestor != nill) {
						struct waited_element *e=iterator;
						ancestor->next = iterator->next;
						e->next = empties;
						e->active = false;
						empties = e;
						size--;
						return;
					} else {
						struct waited_element *e=iterator;
						list = iterator->next;
						e->next = empties;
						e->active = false;
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
      return iterator->p;
		}

		sc_time next_time() {
//			return list->next->t;
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


    PAYLOAD *find_payload(vppnoc_signal* sig) {
			struct waited_element *iterator = list;
			while(iterator != nill) {
				if (iterator->p.signal == sig && iterator->p.value == *sig) {
          do_activate(sig);
          return &(iterator->p);
				}
				iterator = iterator->next;
      }
      return NULL;
    }

		void do_activate(vppnoc_signal* sig) {
			struct waited_element *iterator = list;
			while(iterator != nill) {
				if (iterator->p.signal == sig && iterator->p.value == *sig) {
          if (iterator->active == false) {
					  iterator->active = true;
//					  add_active_status();
          }
					return;
				}
				iterator = iterator->next;
			}
		}
	};
	SC_HAS_PROCESS(VPPNOCMicroScheduler); 
	VPPNOCMicroScheduler
  (
    sc_module_name name
  , int id
  , OWNER_TYPE* ownr
  ) 
  : 
    sc_module(name)
  , m_id(id)
  , m_owner(ownr)
  , m_e_notify_exist(false)
	{
		SC_METHOD(main_notify_action);
		sensitive << m_e_notify;

		SC_METHOD(main_wait_action);
		sensitive << m_e_wait;
	}
	~VPPNOCMicroScheduler(){}


  unsigned long int sc_delta_count() {
         return m_local_delta++;
  }



#ifdef USE_LOCAL_TIME
  void notify(payload_type& trans, sc_time& when, vppnoc_process<OWNER_TYPE, TYPES>* proc) {
    trans.acquire();
    sc_time delay = when;
    sc_time trans_time_stamp = trans.get_time_stamp();
    sc_time now = sc_time_stamp();
    if (trans_time_stamp < now)
    {
      trans_time_stamp = now;
    }
    if (proc->get_ahead_of_time()) {
      delay = SC_ZERO_TIME;
    } else {
      delay = (trans_time_stamp - now)+when;
    }
    trans.set_time_stamp(trans_time_stamp+when);
		if (delay == SC_ZERO_TIME) {
      if (proc->get_ahead_of_time()) {
        //main_notify_action();
        (*(proc))(trans);
      } else {
			  if (sc_delta_count() & (uint64)0x1) {
				  m_even_delta.insert(PAYLOAD(&trans, NULL, 0, proc, 0));
			  } else {
  				m_uneven_delta.insert(PAYLOAD(&trans, NULL, 0, proc, 0));
	  		}
        m_e_notify.notify(SC_ZERO_TIME);
        m_e_notify_exist = true;
      }
		} else {
      if (proc->get_ahead_of_time()) {
        (*(proc))(trans);
      } else {
        m_ppq_notify.insert(PAYLOAD(&trans, NULL, 0, proc, 0), delay+now);
  		  m_e_notify.notify(delay);
        m_e_notify_exist = true;
      }
		}
	}
#else
  void notify(payload_type& trans, sc_time& when, vppnoc_process<OWNER_TYPE, TYPES>* proc) {
    trans.acquire();
    sc_time delay = when;
    sc_time trans_time_stamp = trans.get_time_stamp();
    sc_time now = sc_time_stamp();

    trans.set_time_stamp(trans_time_stamp+when);
		if (delay == SC_ZERO_TIME) {
			if (sc_delta_count() & (uint64)0x1) {
				m_even_delta.insert(PAYLOAD(&trans, NULL, 0, proc, 0));
			} else {
				m_uneven_delta.insert(PAYLOAD(&trans, NULL, 0, proc, 0));
			}
      m_e_notify.notify(SC_ZERO_TIME);
		} else {
      m_ppq_notify.insert(PAYLOAD(&trans, NULL, 0, proc, 0), delay+now);
		  m_e_notify.notify(delay);
  	}
	}
  
#endif

  void wait(payload_type& trans, sc_time& when, vppnoc_signal* s, vppnoc_signal_value sv, vppnoc_process<OWNER_TYPE, TYPES>* proc)
  {
    trans.acquire();
		m_wait_list.insert(PAYLOAD(&trans, s, sv, proc, 0));
  }

  void notify(vppnoc_signal* signal, sc_time when = SC_ZERO_TIME)
  {
		PAYLOAD *pl = m_wait_list.find_payload(signal);
    if (pl) {
      if (pl->trans->get_time_stamp() < sc_time_stamp())
        pl->trans->set_time_stamp(sc_time_stamp());
      m_wait_list.delete_top_action();
      notify(*(pl->trans), when, pl->process);
      
    }
	}

	void wait(payload_type& trans, sc_time& when, vppnoc_process<OWNER_TYPE, TYPES>* proc)
	{
		trans.acquire();
    trans.set_time_stamp(when+sc_time_stamp());
		if (when == SC_ZERO_TIME) {
			cout << "SC_ZERO_TIME is not llegal for wait statement" << endl;
			exit(1);
		} else {
      m_ppq_wait.insert(PAYLOAD(&trans, NULL, 0, proc, 0), when+sc_time_stamp());
			m_e_wait.notify();
		}
	}
 
	void 
  main_notify_action
  (
  ) 
  {
    int tmp_cnt = 0;
    m_e_notify_exist = false;
    sc_time now_tmp = sc_time_stamp();
      /*check waiting list for an active waiting process*/
      while(m_wait_list.get_active_status()) {
			  PAYLOAD tmp = m_wait_list.top_active();
			  m_wait_list.delete_top_action();
			  m_wait_list.rm_active_status();
			  if (tmp.process)
				  (*(tmp.process))(*tmp.trans);
			  else
				  (m_owner->*(tmp.function))(*tmp.trans, sc_time(0, SC_NS));
			  if (tmp.trans)
				  tmp.trans->release();
		  }

      /*  */
		  while(m_immediate_yield.next()) {
			  PAYLOAD& tmp = m_immediate_yield.get();
			  if (tmp.process)
				  (*(tmp.process))(*tmp.trans);
			  else
				  (m_owner->*(tmp.function))(*tmp.trans, sc_time(0, SC_NS));
			  if (tmp.trans)
				  tmp.trans->release();
		  }
		  m_immediate_yield.reset();

		  if (sc_delta_count() & (uint64) 0x1) {
			  while (m_uneven_delta.next()) {
				  PAYLOAD& tmp = m_uneven_delta.get();
				  if (tmp.process)
					  (*(tmp.process))(*tmp.trans);
				  else
					  (m_owner->*(tmp.function))(*tmp.trans, sc_time(0, SC_NS));
				  if (tmp.trans)
					  tmp.trans->release();
			  }
			  m_uneven_delta.reset();

			  if (m_even_delta.size) 
        {

          m_e_notify.notify(SC_ZERO_TIME);
          m_e_notify_exist = true;

        }
		  } else {
			  while (m_even_delta.next()) {
				  PAYLOAD& tmp = m_even_delta.get();
				  if (tmp.process)
					  (*(tmp.process))(*tmp.trans);
				  else
					  (m_owner->*(tmp.function))(*tmp.trans, sc_time(0, SC_NS));
				  if (tmp.trans)
					  tmp.trans->release();
			  }
			  m_even_delta.reset();
			  if (m_uneven_delta.size)
        {

          m_e_notify.notify(SC_ZERO_TIME);
          m_e_notify_exist = true;

        }
		  }
		if(m_ppq_notify.get_size()){
      sc_time top = m_ppq_notify.top_time();
      const sc_time now = sc_time_stamp();
		  while (m_ppq_notify.get_size() && top == now) {
		    PAYLOAD& tmp = m_ppq_notify.top();
			  if (tmp.process) {
			    if (!m_ppq_wait.is_exist_process(tmp))
					    (*(tmp.process))(*tmp.trans);	
			    } else
				    (m_owner->*(tmp.function))(*tmp.trans, sc_time(0, SC_NS));
			    if (tmp.trans)
				    tmp.trans->release();
			    m_ppq_notify.delete_top();
			    top = m_ppq_notify.top_time();
		    }
		    if (m_ppq_notify.get_size()) {
			    m_e_notify.notify(top - now);
          m_e_notify_exist = true;
		    }
		  }
	}


	void main_wait_action() {
		if(m_ppq_wait.get_size()){
      const sc_time now = sc_time_stamp();
  
     sc_time top = m_ppq_wait.top_time();
     while (m_ppq_wait.get_size() && top == now) {
  	   PAYLOAD& tmp = m_ppq_wait.top();
	     if (tmp.process)
		    (*(tmp.process))(*tmp.trans);	
		    else
			    (m_owner->*(tmp.function))(*tmp.trans, sc_time(0, SC_NS));
//		    if (tmp.trans)
//			    tmp.trans->release();
			  m_ppq_wait.delete_top();
			  top = m_ppq_wait.top_time();
		  }

		  if (m_ppq_wait.get_size()) {
	  	  m_e_wait.notify(top - now);
		  }
		}
	}


private:
	OWNER_TYPE*                   m_owner;
	time_ordered_list<PAYLOAD>    m_ppq_notify;
	time_ordered_list<PAYLOAD>    m_ppq_wait;
	delta_list                    m_uneven_delta;
	delta_list                    m_even_delta;
	delta_list                    m_immediate_yield;
	waiting_list                  m_wait_list;
	sc_event                      m_e_notify;
  bool                          m_e_notify_exist;
	sc_event                      m_e_wait;
	int                           m_id;
  unsigned long int             m_local_delta;

};
}
#endif //VPPNOC_MICROSCHEDULER_H
