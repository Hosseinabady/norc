package com.hosseinabady.norc_designer_plugin.editpart;

import java.beans.PropertyChangeEvent;
import java.util.ArrayList;
import java.util.List;



import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;

import com.hosseinabady.noc_designer_plugin.model.NoCBus;
import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCDeletePolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCEditCodePolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCEditLayoutPolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCNodePolicy;
import com.hosseinabady.norc_designer_plugin.figure.NoCBusFigure;


public class NoCBusEditPart extends NoCNodeEditPart {

	@Override
	protected IFigure createFigure() {
		// TODO Auto-generated method stub
		IFigure figure = new NoCBusFigure();
		return figure;
	}

	@Override
	protected void createEditPolicies() {
		// TODO Auto-generated method stub
		installEditPolicy(EditPolicy.LAYOUT_ROLE, new NoRCEditLayoutPolicy());
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new NoRCDeletePolicy());
		installEditPolicy(EditPolicy.SELECTION_FEEDBACK_ROLE, new NoRCEditCodePolicy());
		installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE, new NoRCNodePolicy());
	}

	protected void refreshVisuals() {
		NoCBusFigure figure = (NoCBusFigure)getFigure();
		NoCBus       model  = (NoCBus)getModel();
		
		figure.setName(model.getName());
		figure.setLayout(model.getLayout());
	}
	
	public List<NoCNode> getModelChildren() {
		return new ArrayList<NoCNode>();
	}
	
	@Override
	public void propertyChange(PropertyChangeEvent evt) {
		if (evt.getPropertyName().equals(NoCBus.PROPERTY_LAYOUT)) refreshVisuals();
		if (evt.getPropertyName().equals(NoCBus.PROPERTY_RENAME)) refreshVisuals();
	    if (evt.getPropertyName().equals(NoCBus.PROPERTY_COLOR)) refreshVisuals();
	    //if (evt.getPropertyName().equals(NoRCBus.PROPERTY_FLOOR)) refreshVisuals();
	    if (evt.getPropertyName().equals(NoCBus.TARGETS))    	refreshTargetConnections();
	    if (evt.getPropertyName().equals(NoCBus.SOURCES))     	refreshSourceConnections();
	}	
}
