package com.hosseinabady.norc_designer_plugin.editors;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;

public class NoRCEditorInput implements IEditorInput {

	public String name = null;
	
	public NoRCEditorInput(String name) {
		this.name = name;
	}
	
	@Override
	public boolean exists() {
		// TODO Auto-generated method stub
		return (this.name != null);
	}

	public boolean equals(Object o) {
		if (!(o instanceof NoRCEditorInput))
			return false;
		return ((NoRCEditorInput)o).getName().equals(getName());
	}
	@Override
	public ImageDescriptor getImageDescriptor() {
		// TODO Auto-generated method stub
		return ImageDescriptor.getMissingImageDescriptor();
	}

	@Override
	public String getName() {
		// TODO Auto-generated method stub
		return this.name;
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

}
