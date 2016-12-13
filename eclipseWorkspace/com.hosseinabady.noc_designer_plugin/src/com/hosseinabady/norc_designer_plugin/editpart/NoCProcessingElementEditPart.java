package com.hosseinabady.norc_designer_plugin.editpart;

import java.beans.PropertyChangeEvent;
import java.util.List;


import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCDeletePolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCEditLayoutPolicy;
import com.hosseinabady.norc_designer_plugin.figure.NoCProcessingElementFigure;


public class NoCProcessingElementEditPart extends NoCNodeEditPart {

	@Override
	protected IFigure createFigure() {
		// TODO Auto-generated method stub
		IFigure figure = new NoCProcessingElementFigure();
		return figure;
	}

	@Override
	protected void createEditPolicies() {
		// TODO Auto-generated method stub
		installEditPolicy(EditPolicy.LAYOUT_ROLE, new NoRCEditLayoutPolicy());
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new NoRCDeletePolicy());
	}

	protected void refreshVisuals() {
		NoCProcessingElementFigure figure = (NoCProcessingElementFigure)getFigure();
		NoCProcessingElement       model  = (NoCProcessingElement)getModel();
		
		figure.setName(model.getName());
		figure.setLayout(model.getLayout());
	}
	
	public List<NoCNode> getModelChildren() {
		return ((NoCProcessingElement)getModel()).getChildrenArray();
	}
	
	protected NoCProcessingElement getNoRCProcessingElement() {
		return(NoCProcessingElement)getModel();
	}
	
	@Override
	public void propertyChange(PropertyChangeEvent evt) {
		if (evt.getPropertyName().equals(NoCNode.PROPERTY_LAYOUT)) refreshVisuals();
		if (evt.getPropertyName().equals(NoCNode.PROPERTY_ADD))    refreshChildren();
		if (evt.getPropertyName().equals(NoCNode.PROPERTY_REMOVE)) refreshChildren();
		
	}
	  public void activate()
	  {
	    if (isActive())
	      return;
	    super.activate();
	    getNoRCProcessingElement().addPropertyChangeListener(this);
	  }

	  public void deactivate()
	  {
	    if (!isActive())
	      return;
	    super.deactivate();
	    getNoRCProcessingElement().removePropertyChangeListener(this);
	  }
}
