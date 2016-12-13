package com.hosseinabady.norc_designer_plugin.editpart;


import java.beans.PropertyChangeListener;
import java.util.List;


import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.NodeEditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.requests.DropRequest;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCPath;
import com.hosseinabady.norc_designer_plugin.figure.NoCAbstractBaseFigure;

public abstract class NoCNodeEditPart extends AbstractGraphicalEditPart implements
		NodeEditPart, PropertyChangeListener {

	public void activate() {
		if (isActive())
			return;
		super.activate();
		((NoCNode)getModel()).addPropertyChangeListener(this);
	}
	
	public void deactivate() {
		if (!isActive())
			return;
		super.deactivate();
		((NoCNode)getModel()).removePropertyChangeListener(this);
	}
	@Override
	public void performRequest(Request req) {
		if (req.getType().equals(RequestConstants.REQ_OPEN)) {
			try {
				IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
				page.showView(IPageLayout.ID_PROP_SHEET);
			}
			catch (PartInitException e) {
				e.printStackTrace();
			}
		}
	}

	public ConnectionAnchor getSourceConnectionAnchor(ConnectionEditPart connEditPart) {
		NoCPath path= (NoCPath) connEditPart.getModel();
		return getAbstractBaseFigure().getSourceConnectionAnchor(path.getSourceName());
	}
	
	public ConnectionAnchor getSourceConnectionAnchor(Request request) {

		Point pt = new Point(((DropRequest)request).getLocation());
		return getAbstractBaseFigure().getSourceConnectionAnchorAt(pt);
	}
	
	public ConnectionAnchor getTargetConnectionAnchor(ConnectionEditPart connEditPart) {
		NoCPath path = (NoCPath) connEditPart.getModel();
		return getAbstractBaseFigure().getTargetConnectionAnchor(path.getTargetName());
	}
	
	public ConnectionAnchor getTargetConnectionAnchor(Request request) {
		Point pt = new Point(((DropRequest)request).getLocation());
		return getAbstractBaseFigure().getTargetConnectionAnchorAt(pt);
	}
	
	public String mapSourceConnectionAnchorToName(ConnectionAnchor c) {
		return getAbstractBaseFigure().getSourceAnchoreName(c);
	}
	
	public String mapTargetConnectionAnchorToName(ConnectionAnchor c) {
		return getAbstractBaseFigure().getTargetAnchorName(c);
	}
	
	protected NoCAbstractBaseFigure getAbstractBaseFigure() {
		return (NoCAbstractBaseFigure) getFigure();
	}
	

	protected NoCNode getNode() {
		return (NoCNode) getModel();
	}
	
	protected List getModelSourceConnections() {
		return getNode().getSourceConnections();
	}
	
	protected List getModelTargetConnections() {
		return getNode().getTargetConnections();
	}
}
