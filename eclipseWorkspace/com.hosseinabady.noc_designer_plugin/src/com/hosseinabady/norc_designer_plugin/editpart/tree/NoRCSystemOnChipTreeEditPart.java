package com.hosseinabady.norc_designer_plugin.editpart.tree;

import java.beans.PropertyChangeEvent;
import java.util.List;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;



public class NoRCSystemOnChipTreeEditPart extends NoRCAbstractTreeEditPart {

	public void activate() {
		super.activate();
		((NoCSystemOnChip)getModel()).addPropertyChangeListener(this);
	}
	
	public void deactivate() {
		((NoCSystemOnChip)getModel()).removePropertyChangeListener(this);
		super.deactivate();
		
	}
	
	protected List<NoCNode> getModelChildren() {
		return ((NoCSystemOnChip)getModel()).getChildrenArray();
	}
	@Override
	public void propertyChange(PropertyChangeEvent evt) {
		// TODO Auto-generated method stub
		if (evt.getPropertyName().equals(NoCNode.PROPERTY_LAYOUT)) refreshVisuals();
		if (evt.getPropertyName().equals(NoCNode.PROPERTY_ADD)) refreshChildren();
		if (evt.getPropertyName().equals(NoCNode.PROPERTY_REMOVE)) refreshChildren();
		//if (evt.getPropertyName().equals(NoRCNode.PROPERTY_RENAME)) refreshVisuals();
	}

}
