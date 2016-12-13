package com.hosseinabady.norc_designer_plugin.editors;

import java.util.ArrayList;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;

import com.hosseinabady.norc_designer_plugin.data.NoRCMappedTaskPatternRecord;
import com.hosseinabady.norc_designer_plugin.data.NoRCTaskRecord;
import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerRecord;

public class NoRCMappedTaskPatternEditor extends EditorPart {

	public static final String ID = "NoRC_Designer_Plugin.editors.MappedTaskPatternEditor";


	private int numberOfSuccessfulMappedTask = 0;
	private int numberOfFailedMappedTask = 0;
	private int numberOfSuccessfulReleaseTask = 0;
	
	public NoRCMappedTaskPatternEditor() {
		// TODO Auto-generated constructor stub
		super();

	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		// TODO Auto-generated method stub

	}

	@Override
	public void doSaveAs() {
		// TODO Auto-generated method stub

	}

	@Override
	public void init(IEditorSite site, IEditorInput input)
			throws PartInitException {
		// TODO Auto-generated method stub
		setSite(site);
        setInput(input);

	}

	@Override
	public boolean isDirty() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void createPartControl(Composite parent) {
		// TODO Auto-generated method stub

 
		ArrayList<Label> leftCore = new ArrayList<Label>();
		ArrayList<Label> rightCore = new ArrayList<Label>();

		int h = 60;
		int w = 30;
		int distance = 10;
	
		Group group1 = new Group(parent, SWT.BORDER | SWT.SCROLL_PAGE);
		group1.setBounds(300,300,parent.getSize().x, parent.getSize().y);
		group1.setText("NoRC Task Pattern");
		
		int MAX_ROW_SIZE = ((NoRCMappedTaskPatternEditorInput)getEditorInput()).getDataSet().getRowSize();
		int MAX_COL_SIZE = ((NoRCMappedTaskPatternEditorInput)getEditorInput()).getDataSet().getColumnSize();
		int iOffset = 50;
		int jOffset = 50;
		
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) {
				leftCore.add(new Label(group1, SWT.CENTER | SWT.BORDER | SWT.WRAP | SWT.BOLD));
				rightCore.add(new Label(group1, SWT.CENTER | SWT.BORDER | SWT.WRAP| SWT.BOLD));
				int index = i*MAX_COL_SIZE+j;
				leftCore.get(index).setSize(w,h);
				leftCore.get(index).setLocation(jOffset+j*(2*w+distance), iOffset+i*(h+distance));
				rightCore.get(index).setSize(w,h);
				rightCore.get(index).setLocation(jOffset+j*(2*w+distance)+w, iOffset+i*(h+distance));
		}
		
		ArrayList<NoRCMappedTaskPatternRecord> records = ((NoRCMappedTaskPatternEditorInput)getEditorInput()).getDataSet().getRecords();
				

		int applicationId = 0;
		int FIRSTCOLUMNLENGTH = 120;
		int SECONDCOLUMNLENGTH = 150;
		int THIRDCOLUMNLENGTH = 150;
		int FORTHCOLUMNLENGTH = 150;
		int FIFTHCOLUMNLENGTH = 150;
		
		Label  applicationLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
		applicationLabel.setText("Application Id");
		applicationLabel.setLocation(2*MAX_COL_SIZE*(w+distance), jOffset);
		applicationLabel.setSize(FIRSTCOLUMNLENGTH,20);
		
		Label  manhattanCostLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
		manhattanCostLabel.setText("Manhattan distance cost");
		manhattanCostLabel.setLocation(2*MAX_COL_SIZE*(w+distance)+FIRSTCOLUMNLENGTH, jOffset);
		manhattanCostLabel.setSize(SECONDCOLUMNLENGTH,20);
		
		
		Label  reconfigurationCostLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
		reconfigurationCostLabel.setLocation(2*MAX_COL_SIZE*(w+distance)+SECONDCOLUMNLENGTH+FIRSTCOLUMNLENGTH, jOffset);
		reconfigurationCostLabel.setText("Reconfiguration cost");
		reconfigurationCostLabel.setSize(THIRDCOLUMNLENGTH,20);
		
		
		Label  finalStatusLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
		finalStatusLabel.setLocation(2*MAX_COL_SIZE*(w+distance)+SECONDCOLUMNLENGTH+FIRSTCOLUMNLENGTH+THIRDCOLUMNLENGTH, jOffset);
		finalStatusLabel.setText("Final Status");
		finalStatusLabel.setSize(FORTHCOLUMNLENGTH,20);
		
		Label  simulationTimeLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
		simulationTimeLabel.setLocation(2*MAX_COL_SIZE*(w+distance)+SECONDCOLUMNLENGTH+FIRSTCOLUMNLENGTH+THIRDCOLUMNLENGTH+FORTHCOLUMNLENGTH, jOffset);
		simulationTimeLabel.setText("Simulation time");
		simulationTimeLabel.setSize(FIFTHCOLUMNLENGTH,20);
		int location_index = 0;
		for (int i = 0; i < records.size(); i++) {
			ArrayList<NoRCTaskRecord> tasks = records.get(i).getTasks();
			applicationId = records.get(i).getApplicationId();
			String applicationStatus = records.get(i).getMappingStatus();
			int manhatanCost = records.get(i).getManhattanCost();
			int reconfigurationCost = records.get(i).getReconfigurationCost();
			int simulationTime = tasks.get(tasks.size()-1).getTime();

			
			location_index++;
			applicationLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
			applicationLabel.setBackground(new Color(parent.getDisplay(), records.get(i).getBackgroundRgb()));
			applicationLabel.setForeground(new Color(parent.getDisplay(), records.get(i).getForgroundRgb()));
			applicationLabel.setText(""+applicationId+"  " + applicationStatus);
			applicationLabel.setSize(FIRSTCOLUMNLENGTH,20);
			applicationLabel.setLocation(2*MAX_COL_SIZE*(w+distance), jOffset+(location_index+1)*20);
				
				
			manhattanCostLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
			manhattanCostLabel.setText("" +manhatanCost);
			manhattanCostLabel.setSize(SECONDCOLUMNLENGTH,20);
			manhattanCostLabel.setLocation(2*MAX_COL_SIZE*(w+distance)+FIRSTCOLUMNLENGTH, jOffset+(location_index+1)*20);
				
			reconfigurationCostLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
			reconfigurationCostLabel.setText("" +reconfigurationCost);
			reconfigurationCostLabel.setSize(THIRDCOLUMNLENGTH,20);
			reconfigurationCostLabel.setLocation(2*MAX_COL_SIZE*(w+distance)+SECONDCOLUMNLENGTH+FIRSTCOLUMNLENGTH, jOffset+(location_index+1)*20);
				
			finalStatusLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
			finalStatusLabel.setLocation(2*MAX_COL_SIZE*(w+distance)+SECONDCOLUMNLENGTH+FIRSTCOLUMNLENGTH+THIRDCOLUMNLENGTH, jOffset+(location_index+1)*20);
			if (applicationStatus.equals("attempt to map") && manhatanCost != -1) {
				numberOfSuccessfulMappedTask++;
				finalStatusLabel.setText("Mapping successful");
			}
			else if (applicationStatus.equals("attempt to map") && manhatanCost == -1) {
				numberOfFailedMappedTask++;
				finalStatusLabel.setText("Mapping failed");
			}
			else if (applicationStatus.equals("attempt to release") && manhatanCost == -1) {
				numberOfSuccessfulReleaseTask++;
				finalStatusLabel.setText("Release successful");
			}
			else if (applicationStatus.equals("attempt to release") && manhatanCost != -1)
				finalStatusLabel.setText("Release failed");
			finalStatusLabel.setSize(FORTHCOLUMNLENGTH,20);
				
			simulationTimeLabel = new Label(group1, SWT.LEFT | SWT.BORDER | SWT.WRAP | SWT.BOLD);
			simulationTimeLabel.setLocation(2*MAX_COL_SIZE*(w+distance)+SECONDCOLUMNLENGTH+FIRSTCOLUMNLENGTH+THIRDCOLUMNLENGTH+FORTHCOLUMNLENGTH, jOffset+(location_index+1)*20);
			simulationTimeLabel.setText(""+simulationTime/1000+"  ns");
			simulationTimeLabel.setSize(FIFTHCOLUMNLENGTH,20);
		
			System.out.print("applicationId = "+applicationId+"  " + applicationStatus);
			System.out.print("\t" + manhatanCost);
			System.out.print("\t" +reconfigurationCost);
			System.out.println("\t"+simulationTime/1000+"  ns");
			
			
			
			for (int j = 0; j < tasks.size(); j++) {
				int index = tasks.get(j).getTileId() - 1;
				if (tasks.get(j).getCoreId() == 0) {
					if (tasks.get(j).getStatus().equals("mapped")) {
						leftCore.get(index).setBackground(new Color(parent.getDisplay(), records.get(i).getBackgroundRgb()));
						leftCore.get(index).setForeground(new Color(parent.getDisplay(), records.get(i).getForgroundRgb()));
						leftCore.get(index).setText("\n(" +(index+1) + ")\n\n"+tasks.get(j).getTaskId()+","+applicationId);
					} else {
						leftCore.get(index).setBackground(ColorConstants.menuBackground);
						leftCore.get(index).setText("");
					}
				} else {
					if (tasks.get(j).getStatus().equals("mapped")) {
						rightCore.get(index).setBackground(new Color(parent.getDisplay(), records.get(i).getBackgroundRgb()));
						rightCore.get(index).setForeground(new Color(parent.getDisplay(), records.get(i).getForgroundRgb()));
	
						rightCore.get(index).setText("\n(" + (index+1) + ")\n\n"+tasks.get(j).getTaskId()+","+applicationId);
					} else {
						rightCore.get(index).setBackground(ColorConstants.menuBackground);
						rightCore.get(index).setText("");
					}
						
				}
			}
		}
		System.out.println("Number Of Successful Mapped Task = " + numberOfSuccessfulMappedTask);
		System.out.println("Number Of Failed Mapped Task = " + numberOfFailedMappedTask);
		System.out.println("Number Of Successful Released Task = " + numberOfSuccessfulReleaseTask);
	}

	
	
	
	
	
	
	



	@Override
	public void setFocus() {
		// TODO Auto-generated method stub

	}

}
