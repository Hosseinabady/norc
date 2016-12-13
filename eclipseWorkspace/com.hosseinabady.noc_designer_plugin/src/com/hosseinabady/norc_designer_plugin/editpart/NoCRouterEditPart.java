package com.hosseinabady.norc_designer_plugin.editpart;

import java.beans.PropertyChangeEvent;
import java.util.ArrayList;
import java.util.List;


import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCRouter;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCDeletePolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCEditCodePolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCEditLayoutPolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCNodePolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCRenamePolicy;
import com.hosseinabady.norc_designer_plugin.figure.NoCRouterFigure;


public class NoCRouterEditPart extends NoCNodeEditPart {

	@Override
	protected IFigure createFigure() {
		// TODO Auto-generated method stub
		IFigure figure = new NoCRouterFigure();
		return  figure;
	}

	@Override
	protected void createEditPolicies() {
		// TODO Auto-generated method stub
		installEditPolicy(EditPolicy.LAYOUT_ROLE, new NoRCEditLayoutPolicy());
		installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE, new NoRCNodePolicy());
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new NoRCDeletePolicy());
		installEditPolicy(EditPolicy.NODE_ROLE, new NoRCRenamePolicy());
		installEditPolicy(EditPolicy.SELECTION_FEEDBACK_ROLE, new NoRCEditCodePolicy());
	}

	protected void refreshVisuals() {
		NoCRouterFigure figure = (NoCRouterFigure)getFigure();
		NoCRouter       model  = (NoCRouter)getModel();
		
		figure.setName(model.getName());
		figure.setLayout(model.getLayout());
		figure.setBackgroundColor(model.getColor());
	}
	
	public List<NoCNode> getModelChildren() {
		return new ArrayList<NoCNode>();
	}
	@Override
	public void propertyChange(PropertyChangeEvent evt) {
		if (evt.getPropertyName().equals(NoCRouter.PROPERTY_LAYOUT)) refreshVisuals();
		if (evt.getPropertyName().equals(NoCRouter.PROPERTY_RENAME)) refreshVisuals();
	    if (evt.getPropertyName().equals(NoCRouter.PROPERTY_COLOR))  refreshVisuals();
	    if (evt.getPropertyName().equals(NoCRouter.PROPERTY_FLOOR))  refreshVisuals();
	    if (evt.getPropertyName().equals(NoCRouter.TARGETS))         refreshTargetConnections();
	    if (evt.getPropertyName().equals(NoCRouter.SOURCES))         refreshSourceConnections();
	    
	}
	
}
