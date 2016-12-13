package com.hosseinabady.norc_designer_plugin.editors;


import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;

import com.hosseinabady.norc_designer_plugin.data.NoRCMappedTaskPatternDataSet;
import com.hosseinabady.norc_designer_plugin.data.NoRCMappedTaskPatternRecord;
import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerDataSet;

public class NoRCMappedTaskPatternEditorInput implements IEditorInput {

	private NoRCMappedTaskPatternDataSet dataSet = null;
	private String name;
	
	public NoRCMappedTaskPatternEditorInput(NoRCMappedTaskPatternDataSet dataSet) {
		this.dataSet = dataSet;
		this.name = "MappedTaskPattern";
	}
	@Override
	public boolean exists() {
		// TODO Auto-generated method stub
		return (this.name != null);
	}

	@Override
	public String getName() {
		// TODO Auto-generated method stub
		return this.name;
	}
	
	@Override
	public ImageDescriptor getImageDescriptor() {
		// TODO Auto-generated method stub
		return ImageDescriptor.getMissingImageDescriptor();
	}

	@Override
	public IPersistableElement getPersistable() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getToolTipText() {
		// TODO Auto-generated method stub
		return this.name;
	}

	@Override
	public Object getAdapter(Class adapter) {
		// TODO Auto-generated method stub
		return null;
	}
	
	   public NoRCMappedTaskPatternDataSet getDataSet() {
	        return dataSet;
	    }

}
