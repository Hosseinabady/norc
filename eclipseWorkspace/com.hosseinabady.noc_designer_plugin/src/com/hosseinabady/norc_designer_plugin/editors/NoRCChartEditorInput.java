package com.hosseinabady.norc_designer_plugin.editors;



import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;

import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerDataSet;


public class NoRCChartEditorInput implements IEditorInput {

	

    private NoRCTrafficPowerDataSet dataSet = null;


    
    public NoRCChartEditorInput(NoRCTrafficPowerDataSet dataSet, int type) {
        this.dataSet = dataSet;

    }
    
    
	@Override
	public boolean exists() {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	public ImageDescriptor getImageDescriptor() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getName() {
		// TODO Auto-generated method stub
		return "Chart";
	}

	@Override
	public IPersistableElement getPersistable() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getToolTipText() {
		// TODO Auto-generated method stub
		return "Chart";
	}

	@Override
	public Object getAdapter(Class adapter) {
		// TODO Auto-generated method stub
		return null;
	}

    public NoRCTrafficPowerDataSet getDataSet() {
        return dataSet;
    }
}
