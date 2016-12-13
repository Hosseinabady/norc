package com.hosseinabady.noc_designer_plugin.model;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

public abstract class NoCAbstractElements implements Cloneable, Serializable {

	transient private PropertyChangeSupport listeners = new PropertyChangeSupport(this);
	
	
	public static final String SIZE = "size";
	public static final String LOC = "location";
	public static final String PROPERTY_NAME = "Name";
	public static final String PROPERTY_LAYOUT = "NodeLayout";
	public static final String PROPERTY_ADD = "NodeAddChild";
	public static final String PROPERTY_REMOVE = "NodeRemoveChild";
	public static final String PROPERTY_RENAME = "NodeRename";
	public static final String PROPERTY_BACKGROUNDCOLOR = "NodeBackgroundColor";
	public static final String PROPERTY_FOREGROUNDCOLOR = "NodeForegroundColor";
	
	
	private static final long serialVersionUID = 1;
	
	public static final String SOURCES = "sources";
	public static final String TARGETS = "targets";
	
	public void addPropertyChangeListener(PropertyChangeListener listener) {
		listeners.addPropertyChangeListener(listener);
	}

	public PropertyChangeSupport getListeners() {
		return listeners;
	}

	protected void firePropertyChange(String propName, Object old, Object newValue) {
		listeners.firePropertyChange(propName, old, newValue);
	}
	
	public void removePropertyChangeListener(PropertyChangeListener listener) {
		listeners.removePropertyChangeListener(listener);
	}
	
	private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		listeners = new PropertyChangeSupport(this);
	}
	
}
