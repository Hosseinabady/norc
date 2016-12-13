package com.hosseinabady.norc_designer_plugin.data.parsers;


import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;


import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import com.hosseinabady.norc_designer_plugin.data.NoRCMappedTaskPatternDataSet;
import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerDataSet;
import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerRecord;

public class NoRCRouterTrafficSAXParser extends DefaultHandler {

	NoRCTrafficPowerDataSet     dataSet;
	List<NoRCTrafficPowerRecord> records;
	
	private String tempVal;
	
	private int currentRecord;

	//to maintain context

	
	public NoRCRouterTrafficSAXParser(){
		records = new ArrayList<NoRCTrafficPowerRecord>();
	}
	
	public NoRCTrafficPowerDataSet run(String fileName) {
		
		parseDocument(fileName);
		return printData();
	}

	private void parseDocument(String fileName) {
		
		//get a factory
		SAXParserFactory spf = SAXParserFactory.newInstance();
		try {
		
			//get a new instance of parser
			SAXParser sp = spf.newSAXParser();
			
			//parse the file and also register this class for call backs
			sp.parse(fileName, this);
			
		}catch(SAXException se) {
			se.printStackTrace();
		}catch(ParserConfigurationException pce) {
			pce.printStackTrace();
		}catch (IOException ie) {
			ie.printStackTrace();
		}
	}

	/**
	 * Iterate through the list and print
	 * the contents
	 */
	private NoRCTrafficPowerDataSet printData(){
		//-------------sorting records------
		List<NoRCTrafficPowerRecord> sortedRecords;
		sortedRecords = new ArrayList<NoRCTrafficPowerRecord>();
		int index = 0;
		while (records.size() > 0){
			index++;
			for(int i = 0; i < records.size(); i++) {
				NoRCTrafficPowerRecord tmp = records.get(i);
				if (tmp.getId() == index) {
					sortedRecords.add(records.get(i));
					records.remove(tmp);
				}
			}
		}
		//----------------------------------
		dataSet.setRecords(sortedRecords);
		return dataSet;
	}
	

	//Event Handlers
	public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
		//reset
		if(qName.equalsIgnoreCase("NoRCPlatform")) {
			dataSet =  new NoRCTrafficPowerDataSet();
			int rowSize = Integer.parseInt(attributes.getValue("","RowSize"));
			dataSet.setRowSize(rowSize);
			int columnSize = Integer.parseInt(attributes.getValue("","ColumnSize"));
			dataSet.setColumnSize(columnSize);
			
		}

	}
	

	public void characters(char[] ch, int start, int length) throws SAXException {
		tempVal = new String(ch,start,length);
	}
	
	public void endElement(String uri, String localName, String qName) throws SAXException {

		if(qName.equalsIgnoreCase("Router")) {
			//add it to the list
			//myEmpls.add(tempEmp);
		}else if (qName.equalsIgnoreCase("Module")) {
			//tempEmp.setModule(tempVal);
		}else if (qName.equalsIgnoreCase("Id")) {
			currentRecord = Integer.parseInt(tempVal);
			int i;
			for (i = 0; i < records.size(); i++)
				if (records.get(i).getId() == currentRecord) {
					records.get(i).setTraffic(records.get(i).getTraffic()+1);
					break;
				}
			if (i == records.size()) {
				NoRCTrafficPowerRecord rc = new NoRCTrafficPowerRecord();
				rc.setId(currentRecord);
				rc.setTraffic(1.0);
				rc.setPower(1.0);
				records.add(rc);
			}
		} else if (qName.equalsIgnoreCase("Power")) {
			int i;
			for (i = 0; i < records.size(); i++)
				if (records.get(i).getId() == currentRecord) {
					records.get(i).setPower(Double.parseDouble(tempVal));
					break;
				}
			if (i == records.size()) {
				System.out.println("Fatal Error Core: 101");
			}
			
		} else if (qName.equalsIgnoreCase("Time")) {
			int i;
			for (i = 0; i < records.size(); i++)
				if (records.get(i).getId() == currentRecord) {
					records.get(i).setTime(Double.parseDouble(tempVal));
					break;
				}
			if (i == records.size()) {
				System.out.println("Fatal Error Core: 101");
			}
		}  else if (qName.equalsIgnoreCase("Channel")) {
			int i;
			for (i = 0; i < records.size(); i++)
				if (records.get(i).getId() == currentRecord) {
					records.get(i).setChannelNumber(Integer.parseInt(tempVal));
					break;
				}
			if (i == records.size()) {
				System.out.println("Fatal Error Core: 101");
			}
		}
	}
}
