package com.hosseinabady.norc_designer_plugin.editpart.tree;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.List;


import org.eclipse.gef.EditPolicy;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;

import com.hosseinabady.noc_designer_plugin.model.NoCDynamicCore;
import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCDeletePolicy;

public class NoRCDynamicCoreTreeEditPart extends NoRCAbstractTreeEditPart{
	
	protected List<NoCNode> getModelChildren() {
		return ((NoCDynamicCore)getModel()).getChildrenArray();
	}
	
	@Override
	protected void createEditPolicies() {
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new NoRCDeletePolicy());
		//installEditPolicy(EditPolicy.SELECTION_FEEDBACK_ROLE, new NoRCEditCodePolicy());
	}
	
	public void refreshVisuals() {
		NoCDynamicCore model = (NoCDynamicCore)getModel();
		setWidgetText(model.getName()+" ");
		setWidgetImage(PlatformUI.getWorkbench().getSharedImages().getImage(ISharedImages.IMG_DEF_VIEW));
	}
	@Override
	public void propertyChange(PropertyChangeEvent evt) {
		// TODO Auto-generated method stub
		if (evt.getPropertyName().equals(NoCNode.PROPERTY_ADD)) refreshChildren();
		if (evt.getPropertyName().equals(NoCNode.PROPERTY_REMOVE)) refreshChildren();
	}

	public void activate() {
		super.activate();
		((NoCNode)getModel()).addPropertyChangeListener(this);
	}
	
	public void deactivate() {
		((NoCNode)getModel()).removePropertyChangeListener(this);
		super.deactivate();
		
	}


}
