package com.hosseinabady.norc_designer_plugin.editors;


import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;

import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerDataSet;

public class NoRCPowerPatternEditorInput implements IEditorInput {

    private NoRCTrafficPowerDataSet dataSet = null;
    
    
    public NoRCPowerPatternEditorInput(NoRCTrafficPowerDataSet dataSet, int type) {
        this.dataSet = dataSet;

    }
    
	@Override
	public boolean exists() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public ImageDescriptor getImageDescriptor() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getName() {
		// TODO Auto-generated method stub
		return "PowerPattern";
	}

	@Override
	public IPersistableElement getPersistable() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getToolTipText() {
		// TODO Auto-generated method stub
		return "PowerPattern";
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
