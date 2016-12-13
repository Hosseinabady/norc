package com.hosseinabady.norc_designer_plugin.figure;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.Rectangle;

public class NoCBusFigure extends NoCAbstractBaseFigure {

	private Label labelName = new Label();
	
	public static final int numberOfPorts = 3;
	//private XYLayout layout;
	private NoCFixedAnchor[] initiatorAnchors = new NoCFixedAnchor[numberOfPorts];
	private NoCFixedAnchor[] targetAnchors    = new NoCFixedAnchor[numberOfPorts];
		
	public static final int BUS_FIGURE_DEFHEIGHT = 10;
	public static final int BUS_FIGURE_DEFWIDTH = 80;
	
	public NoCBusFigure() {
		ToolbarLayout layout = new ToolbarLayout();
		setLayoutManager(layout);
		
		labelName.setForegroundColor(ColorConstants.green);
		add(labelName, ToolbarLayout.ALIGN_CENTER);
		//setConstraint(labelName, new Rectangle(5, 5, -1, -1));
	
		//---For Network Interface
		initiatorAnchors[0] = new NoCFixedAnchor(this);
		//initiatorAnchors[0].setPlace(new Point(2/3, 0));
		initiatorAnchors[0].setPlace(8.0/5, 2);
		String si0 = "NIinitiatorPort" + labelName.getText() + '0';
		putSourceAnchors(si0, initiatorAnchors[0]);
		
		targetAnchors[0] = new NoCFixedAnchor(this);
		//targetAnchors[0].setPlace(new Point(4/3, 0));
		targetAnchors[0].setPlace(8.0/5, 2);
		String st0 = "NItargetPort" + labelName.getText() + '0';
		putTargetAnchors(st0, targetAnchors[0]);
		
		//--For Static Core
		initiatorAnchors[1] = new NoCFixedAnchor(this);
		//initiatorAnchors[1].setPlace(new Point(2/3, 0));
		initiatorAnchors[1].setPlace(4.0/7, 2);
		String si1 = "StaticCoreinitiatorPort" + labelName.getText() + '1';
		putSourceAnchors(si1, initiatorAnchors[1]);
		
		targetAnchors[1] = new NoCFixedAnchor(this);
		//targetAnchors[1].setPlace(new Point(2/3, 0));
		targetAnchors[1].setPlace(4.0/7, 2);
		String st1 = "StaticCoretargetPort" + labelName.getText() + '1';
		putTargetAnchors(st1, targetAnchors[1]);
		
		//--For Dynamic Core
		initiatorAnchors[2] = new NoCFixedAnchor(this);
		//initiatorAnchors[2].setPlace(new Point(1, 0));
		initiatorAnchors[2].setPlace(1, 0);
		String si2 = "StaticCoreinitiatorPort" + labelName.getText() + '2';
		putSourceAnchors(si2, initiatorAnchors[2]);
		
		targetAnchors[2] = new NoCFixedAnchor(this);
		//targetAnchors[2].setPlace(new Point(2, 2));
		targetAnchors[2].setPlace(1, 0);
		String st2 = "StaticCoretargetPort" + labelName.getText() + '2';
		putTargetAnchors(st2, targetAnchors[2]);
		
		
		
		setForegroundColor(ColorConstants.darkGray);
		setBackgroundColor(ColorConstants.lightGray);
		
		//setBorder(new LineBorder(2));
		setOpaque(true);
	}
	
	public void setName(String text) {
		labelName.setText(text);
	}
	
	public void setLayout(Rectangle rect) {
		getParent().setConstraint(this, rect);
	}
	public void setInitiatorAnchors(NoCFixedAnchor fa, int i) {
		initiatorAnchors[i] = fa;
	}
	
	public void setTargetAnchors(NoCFixedAnchor fa, int i) {
		targetAnchors[i] = fa;
	}
	
	public NoCFixedAnchor getInitiatorAnchors(int i) {
		return initiatorAnchors[i];
	}
	
	public NoCFixedAnchor getTargetAnchors(int i) {
		return targetAnchors[i];
	}
	public void paintFigure(Graphics g)	{
		Rectangle r = bounds;
		
		setBackgroundColor(ColorConstants.cyan);
		g.drawText(message, r.x + r.width/4, r.y + r.height/4);
		//g.drawRoundRectangle(new Rectangle(r.x, r.y, r.width-1, r.height-1), r.width/8, r.height/8);
		PointList pl = new PointList(6);
		pl.addPoint(r.x, r.y+r.height/2);
		pl.addPoint((int)(r.x+r.width/10.0), r.y);
		pl.addPoint((int)(r.x+9*r.width/10.0), r.y);
		pl.addPoint(r.x+r.width, r.y+r.height/2);
		pl.addPoint((int)(r.x+9.0*r.width/10), r.y+r.height-1);
		pl.addPoint((int)(r.x+1.0*r.width/10), r.y+r.height-1);
		g.drawPolygon(pl);
		
		
		Rectangle anchorNIFigure = new Rectangle ((int)(r.x+(8.0/5)*r.width/2-r.width/10), r.y+2*r.height/2-r.height/10, r.width/10, r.height/10);  
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorNIFigure);
		g.fillRectangle(anchorNIFigure);
		
		Rectangle anchorStaticFigure = new Rectangle ((int)(r.x+(4.0/7)*r.width/2-r.width/10), r.y+2*r.height/2-r.height/10, r.width/10, r.height/10);
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorStaticFigure);
		g.fillRectangle(anchorStaticFigure);
		
		Rectangle anchorDynamicFigure = new Rectangle (r.x+1*r.width/2-r.width/10, r.y+0*r.height/2-r.height/10, r.width/5, r.height/5);
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorDynamicFigure);
		g.fillRectangle(anchorDynamicFigure);
		
		
	}	
}
