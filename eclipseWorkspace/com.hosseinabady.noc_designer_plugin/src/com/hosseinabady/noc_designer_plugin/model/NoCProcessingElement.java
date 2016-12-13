package com.hosseinabady.noc_designer_plugin.model;

import java.util.ArrayList;
import java.util.List;

public class NoCProcessingElement extends NoCNode {

	public static final String PROPERTY_CAPITAL = "ProcessignElement";
	private String name;
	protected List children = new ArrayList();
	private static final long serialVersionUID = 1;
	
	public List getChildren() {
		return children;
	}
	
	public boolean addChild(NoCNode child) {
		children.add(child);
		firePropertyChange(PROPERTY_ADD, null, child);
		return true;
	}
	
	public boolean removeChild(NoCNode child) {
		children.remove(child);
		firePropertyChange(PROPERTY_REMOVE, null, child);
		return true;

	}
	
	public List getChildrenArray() {
		return this.children;
	}
	public void setName(String name) {
		this.name = name;
	}

	public String getName() {
		return this.name;
	}


	public boolean contains(NoCNode child) {
		return children.contains(child);
	}

//	protected List children = new ArrayList();


//	public boolean contains(NoRCNode child) {
//		return children.contains(child);
//	}
}
