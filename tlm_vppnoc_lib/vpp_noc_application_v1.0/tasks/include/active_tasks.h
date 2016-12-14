#ifndef ACTIVE_TASKS_H
#define ACTIVE_TASKS_H

#include <tasks/vppnoc_task.h>

class ActiveTask {
	NoCTask*      m_task;
	bool          m_mapped;
	int           m_tile_id;
	int           m_core_id;

public:
	ActiveTask() {
		m_task = NULL;
		m_mapped = false;
		m_tile_id = -1;
		m_core_id = -1;
	}

	void setTask(NoCTask* tsk) {
		m_task = tsk;
	}
	void setMapped(bool mp) {
		m_mapped = mp;
	}
	void setTileId(int ti) {
		m_tile_id=ti;
	}
	void setCoreId(int ci) {
		m_core_id = ci;
	}

	NoCTask* getTask() {
		return m_task;
	}
	bool getMapped() {
		return m_mapped;
	}
	int getTileId() {
		return m_tile_id;
	}
	int getCoreId() {
		return m_core_id;
	}



};

#endif