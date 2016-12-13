package com.hosseinabady.norc_designer_plugin.editpart;


import java.beans.PropertyChangeEvent;
import java.util.List;


import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCDeletePolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCEditLayoutPolicy;
import com.hosseinabady.norc_designer_plugin.figure.NoCFigureFactory;
import com.hosseinabady.norc_designer_plugin.figure.NoCSystemOnChipFigure;


public class NoCSystemOnChipEditPart extends NoCNodeEditPart {

	@Override
	  public void propertyChange(PropertyChangeEvent evt)
	  {
	    String prop = evt.getPropertyName();
	    if (NoCSystemOnChip.PROPERTY_ADD.equals(prop))                     refreshChildren();
		if (evt.getPropertyName().equals(NoCSystemOnChip.PROPERTY_LAYOUT)) refreshVisuals();
		if (evt.getPropertyName().equals(NoCSystemOnChip.PROPERTY_ADD))    refreshChildren();
		if (evt.getPropertyName().equals(NoCSystemOnChip.PROPERTY_REMOVE)) refreshChildren();
		if (evt.getPropertyName().equals(NoCSystemOnChip.PROPERTY_RENAME)) refreshVisuals();
	  }
	
	@Override
	protected IFigure createFigure() {
		// TODO Auto-generated method stub
		//IFigure figure = new NoRCSystemOnChipFigure();
		//return figure;
		return NoCFigureFactory.createSystemOnChipFigure();
	}

	protected NoCSystemOnChip getSystemOnChip() {
		return(NoCSystemOnChip)getModel();
	}
	@Override
	protected void createEditPolicies() {
		// TODO Auto-generated method stub
		installEditPolicy(EditPolicy.LAYOUT_ROLE, new NoRCEditLayoutPolicy());
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new NoRCDeletePolicy());
		
	}
	
	protected void refreshVisuals() { 
		NoCSystemOnChipFigure figure = (NoCSystemOnChipFigure)getFigure();
		NoCSystemOnChip       model  = (NoCSystemOnChip)getModel();
		
		
		figure.setName(model.getName());

	}

	public List<NoCNode> getModelChildren() {
		return ((NoCSystemOnChip)getModel()).getChildrenArray();
	}

	  public void activate()
	  {
	    if (isActive())
	      return;
	    super.activate();
	    getSystemOnChip().addPropertyChangeListener(this);
	  }

	  public void deactivate()
	  {
	    if (!isActive())
	      return;
	    super.deactivate();
	    getSystemOnChip().removePropertyChangeListener(this);
	  }
	  
/*	@Override
	public void propertyChange(PropertyChangeEvent evt) {
		if (evt.getPropertyName().equals(NoRCNode.PROPERTY_LAYOUT)) refreshVisuals();
		if (evt.getPropertyName().equals(NoRCNode.PROPERTY_ADD)) refreshChildren();
		if (evt.getPropertyName().equals(NoRCNode.PROPERTY_REMOVE)) refreshChildren();
	}*/
}
