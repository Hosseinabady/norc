package com.hosseinabady.norc_designer_plugin.editpart.tree;

import java.beans.PropertyChangeEvent;
import java.util.List;

import org.eclipse.gef.EditPolicy;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCDeletePolicy;

public class NoRCProcessingElementTreeEditPart extends NoRCAbstractTreeEditPart {

	protected List<NoCNode> getModelChildren() {
		return ((NoCProcessingElement)getModel()).getChildrenArray();
	}
	
	@Override
	protected void createEditPolicies() {
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new NoRCDeletePolicy());
		//installEditPolicy(EditPolicy.SELECTION_FEEDBACK_ROLE, new NoRCEditCodePolicy());
	}
	
	public void refreshVisuals() {
		NoCProcessingElement model = (NoCProcessingElement)getModel();
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
