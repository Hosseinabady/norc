package com.hosseinabady.norc_designer_plugin.editpart;


import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.editparts.AbstractConnectionEditPart;

import com.hosseinabady.norc_designer_plugin.figure.NoCPathFigure;

public class NoCPathEditPart extends AbstractConnectionEditPart {

	@Override
	protected void createEditPolicies() {
		// TODO Auto-generated method stub

	}
	protected IFigure createFigure() {
		return new NoCPathFigure();
	}

}
