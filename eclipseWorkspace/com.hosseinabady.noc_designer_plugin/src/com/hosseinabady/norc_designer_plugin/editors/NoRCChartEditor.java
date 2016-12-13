package com.hosseinabady.norc_designer_plugin.editors;



import org.eclipse.birt.chart.model.Chart;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;

import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerDataSet;
import com.hosseinabady.norc_designer_plugin.widget.chart.NoRCBarChartViewer;


public class NoRCChartEditor extends EditorPart {
	public static final String ID = "NoRC_Designer_Plugin.editors.ChartEditor";
	public static NoRCBarChartViewer c3dViewer;
	public static NoRCBarChartViewer c3dViewer2;
	public NoRCChartEditor() {
		super();
		// TODO Auto-generated constructor stub
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
        setSite(site);
        setInput(input);
        setPartName("Bar Chart");
		// TODO Auto-generated method stub

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

		c3dViewer = new NoRCBarChartViewer( parent, SWT.NO_BACKGROUND, ((NoRCChartEditorInput)getEditorInput()).getDataSet(), NoRCChartTypeConstants.TRAFFIC);
		c3dViewer.setLayoutData( new GridData( GridData.FILL_BOTH ) );
		c3dViewer.addPaintListener( c3dViewer );
		c3dViewer.addControlListener( c3dViewer );
		
/*		c3dViewer2 = new NoRCBarChartViewer( parent, SWT.NO_BACKGROUND, ((NoRCChartEditorInput)getEditorInput()).getDataSet(),NoRCChartTypeConstants.POWER);
		c3dViewer2.setLayoutData( new GridData( GridData.FILL_BOTH ) );
		c3dViewer2.addPaintListener( c3dViewer2 );
		c3dViewer2.addControlListener( c3dViewer2 );*/
		

		
	}

	@Override
	public void setFocus() {
		// TODO Auto-generated method stub

	}
}
