package com.hosseinabady.norc_designer_plugin.data;

import java.util.ArrayList;

import org.eclipse.swt.graphics.RGB;

public class NoRCMappedTaskPatternRecord {

	private int applicationId;
	private int manhattanCost;
	private int reconfigurationCost;
	private RGB backgroundRgb;
	private RGB forgroundRgb;
	private String mappingStatus;
	private ArrayList<NoRCTaskRecord> tasks = new ArrayList<NoRCTaskRecord>();
	
	
	public NoRCMappedTaskPatternRecord(int applicationId, int manhattanCost, int reconfigurationCost, String mappingStatus ){
		this.applicationId = applicationId;
		this.manhattanCost = manhattanCost;
		this.reconfigurationCost = reconfigurationCost;
		this.mappingStatus = mappingStatus;
	}
	
	public NoRCMappedTaskPatternRecord(){
		this.applicationId = 0;
		this.manhattanCost = -1;
		this.reconfigurationCost = -1;
    	this.mappingStatus = null;
	}
	
	public void setApplicationId(int applicationId) {
		this.applicationId = applicationId;;
	}
	public int getApplicationId() {
		return applicationId;
	}
	
	public void setManhattanCost(int manhattanCost) {
		this.manhattanCost = manhattanCost;
	}
	public int getManhattanCost() {
		return manhattanCost;
	}
	
	public void setReconfigurationCost(int reconfigurationCost) {
		this.reconfigurationCost = reconfigurationCost;
	}
	public int getReconfigurationCost() {
		return reconfigurationCost;
	}
	
	public void setTasks(ArrayList<NoRCTaskRecord> tasks) {
		this.tasks = tasks;
	}
	public ArrayList<NoRCTaskRecord> getTasks() {
		return tasks;
	}
    public void setMappingStatus(String mappingStatus) { this.mappingStatus = mappingStatus; }
    public String getMappingStatus() { return this.mappingStatus; }
	public void setBackgroundRgb(RGB backgroundRgb) { 
		this.backgroundRgb = backgroundRgb;
	}
	
	public RGB getBackgroundRgb() { 
		return this.backgroundRgb;
	}
	
	public void setForgroundRgb(RGB forgroundRgb) { 
		this.forgroundRgb = forgroundRgb;
	}
	
	public RGB getForgroundRgb() { 
		return this.forgroundRgb;
	}
	
}
