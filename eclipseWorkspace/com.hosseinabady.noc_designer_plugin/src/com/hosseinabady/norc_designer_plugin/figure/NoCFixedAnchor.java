package com.hosseinabady.norc_designer_plugin.figure;

import org.eclipse.draw2d.AbstractConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.Rectangle;

public class NoCFixedAnchor extends AbstractConnectionAnchor {

	//private Point place;
	private double xCode, yCode; 
	
	public NoCFixedAnchor(IFigure owner) {
		super(owner);
	}
	
	
	@Override
	public Point getLocation(Point reference) {
		// TODO Auto-generated method stub
		Rectangle r = getOwner().getBounds();
		int x = (int) (r.x+xCode*r.width/2.0);
		int y = (int) (r.y+yCode*r.height/2.0);

		Point p = new PrecisionPoint(x, y);
		getOwner().translateToAbsolute(p);
		return p;
	}
	
	public void setPlace(double d, double e) {
		this.xCode = d;
		this.yCode = e;
	}

//	public Point getPlace() {
//		return this.place;
//	}
	
	

}
