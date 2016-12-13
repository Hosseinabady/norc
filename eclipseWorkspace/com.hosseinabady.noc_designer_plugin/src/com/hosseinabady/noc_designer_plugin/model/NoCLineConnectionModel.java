package com.hosseinabady.noc_designer_plugin.model;

public class NoCLineConnectionModel extends NoCAbstractElements {
	
	protected NoCNode source, target;
	protected String   sourceName, targetName;
	private   static final long serialVersionID = 1;
	
	public void attachSource() {
		
		if (getSource() == null || getSource().getSourceConnections().contains(this))
			return;
		getSource().addSourceConnection(this);
	}


	public void attachTarget() {
		if (getTarget() == null || getTarget().getTargetConnections().contains(this))
			return;
		getTarget().addTargetConnection(this);

	}


	public void detachSource() {
		if (getSource() == null) 
			return;
		getSource().removeSourceConnection(this);

	}


	public void detachTarget() {
		if (getTarget() == null) 
			return;
		getTarget().removeTargetConnection(this);

	}

	public NoCNode getSource() {
		return source;
	}

	public NoCNode getTarget() {
		return target;
	}

	public void setSource(NoCNode model) {
		source = model;
	}

	public void setTarget(NoCNode model) {
		target = model;
	}
	public String getSourceName() {
		return sourceName;
	}
	
	public void setSourceName(String s) {
		sourceName = s;
	}
	public String getTargetName() {
		return targetName;
	}
	public void setTargetName(String s) {
		targetName = s;
	}

}
