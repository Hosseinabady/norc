#ifndef APPLICATION_H
#define APPLICATION_H

#include <systemc>

namespace vppnoc {
class NoCApplication {

	int	                m_id;
	char                m_name[20];
	int                 m_size;
	char                m_file_name[20];
	sc_core::sc_time    m_nominal_period;
	sc_core::sc_time    m_minimum_period;
	sc_core::sc_time    m_maximum_period;
	double              m_elastic_coefficient;
	int                 m_task_manager_tile_id;
	int                 m_task_manager_core_id;
	int                 m_state;
	int                 m_linkSize;


public:
	NoCApplication() {
		m_id= -1;
		strcpy_s(m_name, "\0");
		strcpy_s(m_file_name, "\0");
		m_task_manager_tile_id = -1;
		m_task_manager_core_id = -1;
		m_state = 0;
	}

	void setFileName(char* fn) { strcpy_s(m_file_name, fn); }
	char* getFileName() { return m_file_name; }

	void setName(char* n) { strcpy_s(m_name, n); }
	char* getName() { return m_name; }

	void setId(int id) {m_id = id;}
	int getId() { return m_id; }

	void setSize(int size) {m_size = size;}
	int getSize() { return m_size; }
	
	sc_core::sc_time  getNominalPeriod() {return m_nominal_period;}
	void setNominalPeriod(sc_core::sc_time p) {m_nominal_period = p;}

	sc_core::sc_time  getMinimumPeriod() {return m_minimum_period;}
	void setMinimumPeriod(sc_core::sc_time p) { m_minimum_period = p;}

	sc_core::sc_time  getMaximumPeriod() {return m_maximum_period;}
	void setMaximumPeriod(sc_core::sc_time p) {m_maximum_period = p;}

	double getElasticCoefficient() {return m_elastic_coefficient;}
	void   setElasticCoefficient(double e) {m_elastic_coefficient = e;}


	void setTaskManagerTileId(int id) { m_task_manager_tile_id = id; }
	int getTaskManagerTileId() { return m_task_manager_tile_id; }
	
	void setTaskManagerCoreId(int id) { m_task_manager_core_id = id; }
	int getTaskManagerCoreId() { return m_task_manager_core_id; }

	void setState(int st) { m_state = st; }
	int getState() { return m_state; }

	void setLinkSize(int als) {
		m_linkSize = als;
	}

	int getLinkSize() {
		return m_linkSize;
	}
};

}
#endif