package com.hosseinabady.norc_designer_plugin.data.parsers;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;


import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;
import org.eclipse.swt.graphics.RGB;

import com.hosseinabady.norc_designer_plugin.data.NoRCMappedTaskPatternDataSet;
import com.hosseinabady.norc_designer_plugin.data.NoRCMappedTaskPatternRecord;
import com.hosseinabady.norc_designer_plugin.data.NoRCTaskRecord;
import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerRecord;

public class NoRCMappedTaskPatternSAXParser extends DefaultHandler {

	NoRCMappedTaskPatternDataSet     dataSet;
	List<NoRCMappedTaskPatternRecord> records;
	
	private String tempVal;
	private int    currentApplicationIndex;
	private int    currentTaskIndex;
	int            currentApplicationId;
	
	Random generator = new Random();

	//to maintain context

	
	public NoRCMappedTaskPatternSAXParser(){
		records = new ArrayList<NoRCMappedTaskPatternRecord>();
		currentApplicationIndex = -1;
		currentTaskIndex   = -1;
	}
	
	public NoRCMappedTaskPatternDataSet run(String fileName) {
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
	private NoRCMappedTaskPatternDataSet printData(){

		dataSet.setRecords(records);
		return dataSet;
	}
	

	//Event Handlers
	public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
		//reset
		if(qName.equalsIgnoreCase("NoRCPlatform")) {
			dataSet =  new NoRCMappedTaskPatternDataSet();
			int rowSize = Integer.parseInt(attributes.getValue("","RowSize"));
			dataSet.setRowSize(rowSize);
			int columnSize = Integer.parseInt(attributes.getValue("","ColumnSize"));
			dataSet.setColumnSize(columnSize);
			
		} else if(qName.equalsIgnoreCase("Task")) {
			currentTaskIndex++;
			int taskId = Integer.parseInt(attributes.getValue("","Id"));
			NoRCTaskRecord  tskRecord = new NoRCTaskRecord();
			
			tskRecord.setTaskId(taskId);
			records.get(currentApplicationIndex).getTasks().add(tskRecord);
		} else if(qName.equalsIgnoreCase("Application")) {
			currentApplicationIndex++;
			currentTaskIndex = -1;
			currentApplicationId = Integer.parseInt(attributes.getValue("","Id"));
			NoRCMappedTaskPatternRecord rc = new NoRCMappedTaskPatternRecord();
			rc.setApplicationId(currentApplicationId);
			RGB backgroundRgb = new RGB(generator.nextInt(255), generator.nextInt(255), generator.nextInt(255));
			rc.setBackgroundRgb(backgroundRgb);
			RGB forgroundRgb = new RGB(255-backgroundRgb.red, 255-backgroundRgb.green, 255-backgroundRgb.blue);
			rc.setForgroundRgb(forgroundRgb);
			String applicationStatus = attributes.getValue("","Status");
			rc.setMappingStatus(applicationStatus);
			records.add(rc);
		} else if(qName.equalsIgnoreCase("Location")) {
			int tileId = Integer.parseInt(attributes.getValue("","TileId"));
			int coreId = Integer.parseInt(attributes.getValue("","CoreId"));
			String status = attributes.getValue("","Status");
		
			records.get(currentApplicationIndex).getTasks().get(currentTaskIndex).setTileId(tileId);
			records.get(currentApplicationIndex).getTasks().get(currentTaskIndex).setCoreId(coreId);
			records.get(currentApplicationIndex).getTasks().get(currentTaskIndex).setStatus(status);
			
		} else if(qName.equalsIgnoreCase("SimulationTime")) {
			int time = Integer.parseInt(attributes.getValue("","Value"));
			records.get(currentApplicationIndex).getTasks().get(currentTaskIndex).setTime(time);
		} else if(qName.equalsIgnoreCase("Cost")) {
			int manhattanCost = Integer.parseInt(attributes.getValue("","Manhattan"));
			records.get(currentApplicationIndex).setManhattanCost(manhattanCost);
			int reconfigurationCost = Integer.parseInt(attributes.getValue("","Reconfiguration"));
			records.get(currentApplicationIndex).setReconfigurationCost(reconfigurationCost);
		}
		
	}
	

	public void characters(char[] ch, int start, int length) throws SAXException {
		tempVal = new String(ch,start,length);
	}
	
	public void endElement(String uri, String localName, String qName) throws SAXException {

	}
	

	
}
