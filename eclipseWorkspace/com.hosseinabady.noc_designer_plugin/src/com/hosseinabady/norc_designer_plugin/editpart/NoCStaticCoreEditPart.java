package com.hosseinabady.norc_designer_plugin.editpart;

import java.beans.PropertyChangeEvent;
import java.util.ArrayList;
import java.util.List;


import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCStaticCore;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCDeletePolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCEditCodePolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCEditLayoutPolicy;
import com.hosseinabady.norc_designer_plugin.editpolicies.NoRCNodePolicy;
import com.hosseinabady.norc_designer_plugin.figure.NoCStaticCoreFigure;


public class NoCStaticCoreEditPart extends NoCNodeEditPart {

	@Override
	protected IFigure createFigure() {
		// TODO Auto-generated method stub
		IFigure figure = new NoCStaticCoreFigure();
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
		NoCStaticCoreFigure  figure = (NoCStaticCoreFigure)getFigure();
		NoCStaticCore        model  = (NoCStaticCore)getModel();
		
		
		figure.setName(model.getName());
		figure.setLayout(model.getLayout());
	}
	
	public List<NoCNode> getModelChildren() {
		return new ArrayList<NoCNode>();
	}
	
	@Override
	public void propertyChange(PropertyChangeEvent evt) {
		if (evt.getPropertyName().equals(NoCStaticCore.PROPERTY_LAYOUT)) refreshVisuals();
		if (evt.getPropertyName().equals(NoCStaticCore.PROPERTY_RENAME)) refreshVisuals();
	    if (evt.getPropertyName().equals(NoCStaticCore.PROPERTY_COLOR)) refreshVisuals();
	    //if (evt.getPropertyName().equals(NoRCBus.PROPERTY_FLOOR)) refreshVisuals();
	    if (evt.getPropertyName().equals(NoCStaticCore.TARGETS)) refreshTargetConnections();
	    if (evt.getPropertyName().equals(NoCStaticCore.SOURCES)) refreshSourceConnections();
	}
}
