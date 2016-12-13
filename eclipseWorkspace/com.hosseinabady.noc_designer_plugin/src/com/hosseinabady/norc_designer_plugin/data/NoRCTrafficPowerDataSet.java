package com.hosseinabady.norc_designer_plugin.data;

import java.util.ArrayList;
import java.util.List;

public class NoRCTrafficPowerDataSet {

    private ArrayList<NoRCTrafficPowerRecord> records = new ArrayList<NoRCTrafficPowerRecord>();
    
	private int rowSize;
	private int columnSize;

	
	public NoRCTrafficPowerDataSet() {
    	this.rowSize = 0;
    	this.columnSize = 0;
	}
	
    public void setRowSize(int rowSize) { this.rowSize = rowSize; }
    public void setColumnSize(int columnSize) {	this.columnSize = columnSize; }

    public int getRowSize() { return this.rowSize; }
    
    public int getColumnSize() { return this.columnSize; }
	
    public void setRecords(List<NoRCTrafficPowerRecord> records) {
    	this.records =  (ArrayList<NoRCTrafficPowerRecord>) records;
	}

    public void addRecord(NoRCTrafficPowerRecord rec) {
    	records.add(rec);
    }

    private ArrayList<NoRCTrafficPowerRecord> getRecords() {
        return records;
    }

    public String[] getTasks() {
        int size = getRecords().size();
        String tasks[] = new String[size];
        for (int i = 0; i < size; i++) {
            NoRCTrafficPowerRecord record = records.get(i);
            tasks[i] = record.getTask();
        }
        return tasks;
    }

    public int[] getIds() {
        int size = getRecords().size();
        int ids[] = new int[size];
        for (int i = 0; i < size; i++) {
        	NoRCTrafficPowerRecord record = records.get(i);
            ids[i] = record.getId();
        }
        return ids;
    }
    
    public double[] getTraffics() {
        int size = getRecords().size();
        double traffics[] = new double[size];
        for (int i = 0; i < size; i++) {
        	NoRCTrafficPowerRecord record = records.get(i);
        	traffics[i] = record.getTraffic();
        }
        return traffics;
    }


    public double[] getPowers() {
        int size = getRecords().size();
        double powers[] = new double[size];
        for (int i = 0; i < size; i++) {
        	NoRCTrafficPowerRecord record = records.get(i);
            powers[i] = record.getPower();
        }
        return powers;
    }

    public double[] getTimes() {
        int size = getRecords().size();
        double times[] = new double[size];
        for (int i = 0; i < size; i++) {
        	NoRCTrafficPowerRecord record = records.get(i);
        	times[i] = record.getTime();
        }
        return times;
    }

}
