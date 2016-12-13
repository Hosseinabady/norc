package com.hosseinabady.noc_designer_plugin.model;

import org.eclipse.draw2d.geometry.Rectangle;

public class NoCRouter extends NoCNode {

	
	public static final String PROPERTY_FLOOR = "ServiceFloor";
	private static final long serialVersionUID = 1;
	
	// Design Properties
	int noOfPort;
	int noOfChannelPerPort;
	int channelCapacity;
	 
	
	
	public static final String PROPERTY_NOOFPORT = "NomberOfPort";
	public static final String PROPERTY_NOOFCHANNELPERPORT = "NumberOfChannelPerPort";
	public static final String PROPERTY_CHANNELCAPACITY = "ChannelCapacity";
	
	
	public void setNoOfPort(int newNoOfPort) {
		int oldNoPort = this.noOfPort;
		this.noOfPort = newNoOfPort;
		getListeners().firePropertyChange(PROPERTY_NOOFPORT, oldNoPort, newNoOfPort);
	}
	
	public int getNoOfPort() {
		return this.noOfPort;
	}
	
	public void setNoOfChannelPerPort(int newNoOfChannelPerPort) {
		int oldNoOfChannelPerPort = this.noOfChannelPerPort;
		this.noOfPort = newNoOfChannelPerPort;
		getListeners().firePropertyChange(PROPERTY_NOOFCHANNELPERPORT, oldNoOfChannelPerPort, newNoOfChannelPerPort);
	}
	
	public int getNoOfChannelPerPort() {
		return this.noOfChannelPerPort;
	}
	
	public void setChannelCapacity(int newChannelCapacity) {
		int oldChannelCapacity = this.channelCapacity;
		this.channelCapacity = newChannelCapacity;
		getListeners().firePropertyChange(PROPERTY_CHANNELCAPACITY, oldChannelCapacity, newChannelCapacity);
	}
	
	public int getchannelCapacity() {
		return this.channelCapacity;
	}
	
	//////////////////////////////////////////
}
