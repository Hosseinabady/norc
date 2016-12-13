package com.hosseinabady.norc_designer_plugin.figure;

import org.eclipse.draw2d.ColorConstants;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LineBorder;

import org.eclipse.draw2d.XYLayout;

import org.eclipse.draw2d.geometry.Rectangle;

public class NoCProcessingElementFigure extends NoCAbstractBaseFigure {

	private Label labelName = new Label();
	//private XYLayout   layout;
	//private NoRCFixedAnchor initiatorAnchors;
	//private NoRCFixedAnchor targetAnchors;
	
	public static final int PROCESSINGELEMENT_FIGURE_DEFHEIGHT = 80;
	public static final int PROCESSINGELEMENT_FIGURE_DEFWIDTH = 80;
	
	public NoCProcessingElementFigure() {
		XYLayout layout = new XYLayout();
		setLayoutManager(layout);
		
		labelName.setForegroundColor(ColorConstants.darkGray);
		add(labelName);
		setConstraint(labelName, new Rectangle(5, 5, -1, -1));
		
/*		initiatorAnchors = new NoRCFixedAnchor(this);
		initiatorAnchors.setPlace(new Point(1, 2));
		String s1 = "PEinitiatorPort" + labelName.getText();
		putSourceAnchors(s1, initiatorAnchors);
		
		
		targetAnchors = new NoRCFixedAnchor(this);
		targetAnchors.setPlace(new Point(1, 2));
		String s2 = "PEtargetPort" + labelName.getText();
		putTargetAnchors(s2, targetAnchors); */
		
		setForegroundColor(ColorConstants.darkBlue);
		setBackgroundColor(ColorConstants.lightBlue);
		
		setBorder(new LineBorder(6));
//		setOpaque(true);
	}
	
	public void setName(String text) {
		labelName.setText(text);
	}
	
	public void setLayout(Rectangle rect) {
		getParent().setConstraint(this, rect);
	}
	
/*	public void setInitiatorAnchors(NoRCFixedAnchor fa) {
		initiatorAnchors = fa;
	}
	
	public void setTargetAnchors(NoRCFixedAnchor fa) {
		targetAnchors = fa;
	}
	
	public NoRCFixedAnchor getInitiatorAnchors() {
		return initiatorAnchors;
	}
	
	public NoRCFixedAnchor getTargetAnchors() {
		return targetAnchors;
	}
	
	public void paintFigure(Graphics g)	{
		Rectangle r = bounds;
		message = "PE";
		g.drawText(message, r.x + r.width/4, r.y + r.height/4);
		Rectangle rn = new Rectangle(r.x, r.y, r.width-1, r.height-1);
		g.drawRectangle(rn);
		
		Rectangle anchorFigure = new Rectangle (r.x+1*r.width/2-r.width/10, r.y+2*r.height/2-r.width/10, r.width/5, r.width/5);
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorFigure);
		g.fillRectangle(anchorFigure);
		
		
	}*/	
}
