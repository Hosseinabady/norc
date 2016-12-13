package com.hosseinabady.norc_designer_plugin.figure;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.ManhattanConnectionRouter;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.PolylineDecoration;

public class NoCPathFigure extends PolylineConnection {
	private Label labelName = new Label();
	
	public NoCPathFigure() {
		setTargetDecoration(new PolylineDecoration());
		setConnectionRouter(new ManhattanConnectionRouter());
		
	}
	
	public void setName(String name) {
		labelName.setText(name);
	}

}
