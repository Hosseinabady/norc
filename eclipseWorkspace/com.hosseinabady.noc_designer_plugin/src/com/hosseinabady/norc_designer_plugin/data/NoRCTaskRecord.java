package com.hosseinabady.norc_designer_plugin.data;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;

public class NoRCTaskRecord {

	private int taskId;
	private int tileId;
	private int coreId;
	private String status;
	private int time;

	
	
	public NoRCTaskRecord(int taskId, int tileId, int coreId, String status, int time) {
		this.taskId = taskId;
		this.tileId = tileId;
		this.coreId = coreId;
		this.status = status;
		this.time = time;
	}
	
	public NoRCTaskRecord() {
		this.taskId = 0;
		this.tileId = 0;
		this.coreId = 0;
		this.status = null;
		this.time = 0;
	}
	public int getTaskId() {
		return taskId;
	}
	

	public int getTileId() {
		return tileId;
	}
	public int getCoreId() {
		return coreId;
	}
	public String getStatus() {
		return status;
	}
	
	public int getTime() {
		return time;
	}
	
	public void setTaskId(int taskId) {
		this.taskId = taskId;;
	}
	

	public void setTileId(int tileId) {
		this.tileId = tileId;
	}
	public void setCoreId(int coreId) {
		this.coreId = coreId;
	}
	public void setStatus(String status) {
		this.status = status;
	}
	
	public void setTime(int time) {
		this.time = time;
	}
	

}

