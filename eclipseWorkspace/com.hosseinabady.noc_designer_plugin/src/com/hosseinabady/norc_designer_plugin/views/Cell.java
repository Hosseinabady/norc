package com.hosseinabady.norc_designer_plugin.views;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

public class Cell {
	private String cellAddress;
	private String cellContent;
	public PropertyChangeSupport propertyChangeSupport = new PropertyChangeSupport(this); 
	
	public Cell() {
		
	}
	
	public Cell(String cellAddress, String cellContent) {
		super();
		this.cellAddress   = cellAddress;
		this.cellContent   = cellContent;
	}
	
	public void addPropertyChangeListener(String propertyName, PropertyChangeListener listener) {
		propertyChangeSupport.addPropertyChangeListener(propertyName, listener);
	}
	
	public void removePropertyChangeListener(PropertyChangeListener listener) {
		propertyChangeSupport.removePropertyChangeListener(listener);
	}
	
	public String getCellAddress() {
		return cellAddress;
	}
	
	public String getCellContent() {
		return cellContent;
	}
	
	public void setCellAddress(String cellAddress) {
		propertyChangeSupport.firePropertyChange("cell_address", this.cellAddress, this.cellAddress = cellAddress);
	}
	
	public void setCellContent(String cellContent) {
		propertyChangeSupport.firePropertyChange("cell_contenct", this.cellContent, this.cellContent = cellContent);
	}
	
	@Override
	public String toString() {
		return cellAddress + " " + cellContent;
	}
}
