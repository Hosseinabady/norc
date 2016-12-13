#pragma once
#ifndef MODELUTILITIES_H_
#define MODELUTILITIES_H_

#define  MAX_RTR_MODULE  2
typedef  int NOC_DATA_VOLUME_TYPE;
class NoCPosition {
private:
	int x;
	int y;



public:
	NoCPosition():x(0), y(0) {}
  NoCPosition(int ix, int iy):x(ix), y(iy) {}

  bool operator==(NoCPosition p) { return ((x == p.x)&&(y==p.y));}
	void setX(int x) {
		this->x = x;
	}

	int getX() {
		return this->x;
	}

	void setY(int y) {
		this->y = y;
	}

	int getY() {
		return this->y;
	}
};


template <typename PAYLOAD>
class time_ordered_list {
public:
	struct element {
		struct element    *next;
		PAYLOAD          p;
		sc_core::sc_time t;
		sc_dt::uint64    d;
		element(PAYLOAD& p, sc_core::sc_time t, sc_dt::uint64 d) : p(p), t(t), d(d) {}
		element(){}
	};
	element   *nill;
	element   *empties;
	element   *list;
	unsigned int size;

	time_ordered_list() : nill(new element()), empties(NULL), list(nill),size(0) {}

	~time_ordered_list() {
		while(size) {
			delete_top();
		}

		while(empties) {
			struct element *e = empties->next;
			delete empties;
			empties = e;
		}
		delete nill;
	}
	void insert(const PAYLOAD & p, sc_core::sc_time t) {
		if (!empties) {
			empties = new struct element();
			empties->next = NULL;
		}
		struct element *e = empties;
		empties = empties->next;
		e->p = p;
		e->t = t;
		e->d = sc_core::sc_delta_count();

		struct element *ancestor = nill;
		struct element *iterator = list;
		while(iterator != nill && iterator->t <= t ) {
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
	
	bool is_exist_process(const PAYLOAD& p) {
		struct element *iterator = list;
		while (iterator != nill) {
			if (iterator->p.process == p.process)
				return true;
			iterator = iterator->next;
		}
		return false;
	}

	bool is_exist(const PAYLOAD& p) {
		struct element *iterator = list;
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
			struct element *e=list;
			list = list->next;
			e->next = empties;
			empties = e;
			size--;
		}
	}
	unsigned int get_size() {
		return size;
	}

	PAYLOAD &top() {
		return list->p;
	}

	sc_core::sc_time top_time() {
		return list->t;
	}

	sc_dt::uint64& top_delta() {
		return list->d;
	}

	sc_core::sc_time nect_time() {
		return list->next->t;
	}
};


#endif /* MODELUTILITIES_H_ */

