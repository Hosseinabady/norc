package com.hosseinabady.norc_designer_plugin.data;

public class NoRCTrafficPowerRecord {

	private String               task          = null;
    private int                  id            = 0;
    private double               time          = 0;
    private double               traffic       = 0;
    private double               power         = 0;
    private int                  channelNumber = 0;
    

    public NoRCTrafficPowerRecord(String task, int id, double time, double power, double traffic, int channelNumber) {
        this.task = task;
        this.id = id;
        this.time = time;
        this.power = power;
        this.traffic = traffic;
        this.channelNumber = channelNumber;
    }
    
    public NoRCTrafficPowerRecord() {
    	this.task          = null;
        this.id            = 0;
        this.time          = 0;
        this.power         = 0;
        this.traffic       = 0;
        this.channelNumber = 0;
    }


    public String getTask() {
        return task;
    }

    
    public int getId() {
    	return id;
    }
    
    public void setId(int id) {
    	this.id = id;
    }
    public double getTime() {
        return time;
    }

    public int getChannelNumbe() {
    	return channelNumber;
    }

   
    public void setChannelNumber(int channelNumber) {
    	this.channelNumber = channelNumber;
    }

    
    public void setTime(double time) {
        this.time = time;
    }

    
    public double getTraffic() {
        return traffic;
    }

    public void setTraffic(double traffic) {
        this.traffic = traffic;
    }

    
    public double getPower() {
        return power;
    }
    
    public void setPower(double power) {
        this.power = power;
    }
}
