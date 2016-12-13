package com.hosseinabady.norc_designer_plugin.figure;

import org.eclipse.draw2d.ColorConstants;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.XYLayout;

import org.eclipse.draw2d.ToolbarLayout;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

public class NoCRouterFigure extends NoCAbstractBaseFigure {

	private Label labelName = new Label();
	public static final int numberOfPorts = 5;
	private NoCFixedAnchor[] initiatorAnchors = new NoCFixedAnchor[numberOfPorts];
	private NoCFixedAnchor[] targetAnchors    = new NoCFixedAnchor[numberOfPorts];
	private XYLayout layout;

	public static final int ROUTER_FIGURE_DEFHEIGHT = 80;
	public static final int ROUTER_FIGURE_DEFWIDTH = 80;

	//private Rectangle[] initiatorAnchorPoint = new Rectangle[numberOfPorts];
	//private Rectangle[] targetAnchorPoint = new Rectangle[numberOfPorts];
	
	public NoCRouterFigure() {
		//ToolbarLayout layout = new ToolbarLayout();
		layout = new XYLayout();
		setLayoutManager(layout);
	

		labelName.setForegroundColor(ColorConstants.black);
		add(labelName, ToolbarLayout.ALIGN_CENTER);
		setConstraint(labelName, new Rectangle(5, 5, -1, -1));
		setForegroundColor(ColorConstants.darkGray);
		setBackgroundColor(ColorConstants.lightBlue);
		//setBorder(new LineBorder(1));
		setOpaque(true);
		
		//*********************************************************
		//					Assign anchors
		//*********************************************************
		// Processing Element Port
		initiatorAnchors[0] = new NoCFixedAnchor(this);
		
		//initiatorAnchors[0].setPlace(new Point(0.5, 0.5));
		initiatorAnchors[0].setPlace(0, 0);
		String si0 = "initiatorPort"  + labelName.getText()+'0';
		putSourceAnchors(si0, initiatorAnchors[0]);
		
		targetAnchors[0] = new NoCFixedAnchor(this);
		//targetAnchors[0].setPlace(new Point(0, 0));
		targetAnchors[0].setPlace(0, 0);
		String st0 = "targetPort"  + labelName.getText()+'0';
		putTargetAnchors(st0, targetAnchors[0]);
		
		// West Port
		initiatorAnchors[1] = new NoCFixedAnchor(this);
		//initiatorAnchors[1].setPlace(new Point(0, 1));		
		initiatorAnchors[1].setPlace(0, 1);
		String si1 = "initiatorPort"  + labelName.getText()+'1';
		putSourceAnchors(si1, initiatorAnchors[1]);

		targetAnchors[1] = new NoCFixedAnchor(this);
		//targetAnchors[1].setPlace(new Point(0, 1));
		targetAnchors[1].setPlace(0, 1);
		String st1 = "targetPort"  + labelName.getText()+'1';
		putTargetAnchors(st1, targetAnchors[1]);
		
		// South Port
		initiatorAnchors[2] = new NoCFixedAnchor(this);
		//initiatorAnchors[2].setPlace(new Point(1, 2));
		initiatorAnchors[2].setPlace(1, 2);
		String si2 = "initiatorPort"  + labelName.getText()+'2';
		putSourceAnchors(si2, initiatorAnchors[2]);
		
		targetAnchors[2] = new NoCFixedAnchor(this);
		//targetAnchors[2].setPlace(new Point(1, 2));		
		targetAnchors[2].setPlace(1, 2);
		String st2 = "targetPort"  + labelName.getText()+'2';
		putTargetAnchors(st2, targetAnchors[2]);
		
		// East Port
		initiatorAnchors[3] = new NoCFixedAnchor(this);
		//initiatorAnchors[3].setPlace(new Point(2, 1));		
		initiatorAnchors[3].setPlace(2, 1);
		String si3 = "initiatorPort"  + labelName.getText()+'3';
		putSourceAnchors(si3, initiatorAnchors[3]);
		
		targetAnchors[3] = new NoCFixedAnchor(this);
		//targetAnchors[3].setPlace(new Point(2, 1));
		targetAnchors[3].setPlace(2, 1);
		String st3 = "targetPort"  + labelName.getText()+'3';
		putTargetAnchors(st3, targetAnchors[3]);
		
		// North Port
		initiatorAnchors[4] = new NoCFixedAnchor(this);
		//initiatorAnchors[4].setPlace(new Point(1, 0));
		initiatorAnchors[4].setPlace(1, 0);
		String si4 = "initiatorPort"  + labelName.getText()+'4';
		putSourceAnchors(si4, initiatorAnchors[4]);
		
		targetAnchors[4] = new NoCFixedAnchor(this);// South Port
		//targetAnchors[4].setPlace(new Point(1, 0));		
		targetAnchors[4].setPlace(1, 0);
		String st4 = "targetPort"  + labelName.getText()+'4';
		putTargetAnchors(st4, targetAnchors[4]);
		//*********************************************************
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

		g.drawText(message, r.x + r.width/4, r.y + r.height/4);
		g.drawRoundRectangle(new Rectangle(r.x, r.y, r.width-1, r.height-1), r.width/8, r.height/8);
	
		
		Rectangle anchorPEFigure = new Rectangle (r.x-r.width/10, r.y-r.width/10, r.width/5, r.width/5);  
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorPEFigure);
		g.fillRectangle(anchorPEFigure);
		
		Rectangle anchorWestFigure = new Rectangle (r.x+0*r.width/2-r.width/10, r.y+1*r.height/2-r.width/10, r.width/5, r.width/5);  
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorWestFigure);
		g.fillRectangle(anchorWestFigure);
		
		Rectangle anchorSouthFigure = new Rectangle (r.x+1*r.width/2-r.width/10, r.y+2*r.height/2-r.width/10, r.width/5, r.width/5);
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorSouthFigure);
		g.fillRectangle(anchorSouthFigure);
		
		Rectangle anchorEastFigure = new Rectangle (r.x+2*r.width/2-r.width/10, r.y+1*r.height/2-r.width/10, r.width/5, r.width/5);
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorEastFigure);
		g.fillRectangle(anchorEastFigure);
		
		Rectangle anchorNorthFigure = new Rectangle (r.x+1*r.width/2-r.width/10, r.y+0*r.height/2-r.width/10, r.width/5, r.width/5);
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorNorthFigure);
		g.fillRectangle(anchorNorthFigure);
		

	}	
}
