package com.hosseinabady.norc_designer_plugin.views;


import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TableViewerColumn;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.ui.part.ViewPart;

import com.hosseinabady.norc_designer_plugin.data.NoRCMappedTaskPatternDataSet;
public class TaskView extends ViewPart {

	public  final static String ID ="NoRC_Designer_Plugin.taskView";  
	private TableViewer taskViewer;
	private TableViewerColumn taskColorColumn;
	private TableViewerColumn taskNameColumn;
	private TableViewerColumn manhattanDistanceColumn;
	private TableViewerColumn reconfigurationCostColumn;
	private TableViewerColumn statusColumn;
	private TableViewerColumn startSimulationTimeColumn;
	private TableViewerColumn EndSimulationTimeColumn;
	
	private NoRCMappedTaskPatternDataSet dataSet;
	
	public TaskView() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public void createPartControl(Composite parent) {
		// TODO Auto-generated method stub
		taskViewer =
	            new TableViewer(parent, SWT.H_SCROLL | SWT.V_SCROLL
	                  | SWT.MULTI | SWT.FULL_SELECTION);
	      final Table table = taskViewer.getTable();

//	      TableColumnLayout layout = new TableColumnLayout();
//	      parent.setLayout(layout);

	      taskColorColumn = new TableViewerColumn(taskViewer, SWT.NONE);
	      taskColorColumn.getColumn().setText("Task Color");
	      taskColorColumn.getColumn().setWidth(100);
	      //taskColorColumn.setEditingSupport(new CellEditingSupport(taskViewer, 0));
	      
	      taskNameColumn = new TableViewerColumn(taskViewer, SWT.NONE);
	      taskNameColumn.getColumn().setText("Task Name");
	      taskNameColumn.getColumn().setWidth(100);
	      //addColumn.setEditingSupport(new CellEditingSupport(viewer, 0));

	      manhattanDistanceColumn = new TableViewerColumn(taskViewer, SWT.NONE);
	      manhattanDistanceColumn.getColumn().setText("Manhattan Distance");
	      manhattanDistanceColumn.getColumn().setWidth(100);
	     // valueColumn.setEditingSupport(new CellEditingSupport(viewer, 1));
  
	      reconfigurationCostColumn = new TableViewerColumn(taskViewer, SWT.NONE);
	      reconfigurationCostColumn.getColumn().setText("Reconfiguration Cost");
	      reconfigurationCostColumn.getColumn().setWidth(150);
	      
	      statusColumn = new TableViewerColumn(taskViewer, SWT.NONE);
	      statusColumn.getColumn().setText("Task Status");
	      statusColumn.getColumn().setWidth(150);
	      
	      startSimulationTimeColumn = new TableViewerColumn(taskViewer, SWT.NONE);
	      startSimulationTimeColumn.getColumn().setText("Start SimulationTime (ns)");
	      startSimulationTimeColumn.getColumn().setWidth(150);
	      
	      EndSimulationTimeColumn = new TableViewerColumn(taskViewer, SWT.NONE);
	      EndSimulationTimeColumn.getColumn().setText("End Simulation Time (ns)");
	      EndSimulationTimeColumn.getColumn().setWidth(150);
	      
	      
	      table.setHeaderVisible(true);
	      table.setLinesVisible(true);

	      
	     
	}

	@Override
	public void setFocus() {
		// TODO Auto-generated method stub

	}

}
