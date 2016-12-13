package com.hosseinabady.norc_designer_plugin.data;

import java.util.ArrayList;
import java.util.List;

public class NoRCMappedTaskPatternDataSet {

	   /**
     * The singleton instance
     */
    //private static NoRCDataSet instance = new NoRCDataSet();

	private int rowSize;
	private int columnSize;
	

    private ArrayList<NoRCMappedTaskPatternRecord> records = new ArrayList<NoRCMappedTaskPatternRecord>();

    public NoRCMappedTaskPatternDataSet() {
    	this.rowSize = 0;
    	this.columnSize = 0;

    }
    /**
     * Returns the singleton instance.
     * 
     * @return the singleton instance
     */

    public void setRowSize(int rowSize) { this.rowSize = rowSize; }
    public void setColumnSize(int columnSize) {	this.columnSize = columnSize; }

    public int getRowSize() { return this.rowSize; }
    
    public int getColumnSize() { return this.columnSize; }
    
    public void setRecords(List<NoRCMappedTaskPatternRecord> records) { this.records =  (ArrayList<NoRCMappedTaskPatternRecord>) records; }
    
    public ArrayList<NoRCMappedTaskPatternRecord> getRecords() { return this.records; }

    
    public void addRecord(NoRCMappedTaskPatternRecord rec) { records.add(rec); }


    
}
