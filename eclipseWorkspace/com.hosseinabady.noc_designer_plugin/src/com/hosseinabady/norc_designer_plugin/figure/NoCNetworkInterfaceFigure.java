package com.hosseinabady.norc_designer_plugin.figure;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

public class NoCNetworkInterfaceFigure extends NoCAbstractBaseFigure {
	private Label labelName = new Label();
	private NoCFixedAnchor initiatorAnchors;
	private NoCFixedAnchor targetAnchors;
	public static final int NETWORKINTERFACE_FIGURE_DEFHEIGHT = 80;
	public static final int NETWORKINTERFACE_FIGURE_DEFWIDTH = 80;
	
	public NoCNetworkInterfaceFigure() {
		ToolbarLayout layout = new ToolbarLayout();
		setLayoutManager(layout);
		
		labelName.setForegroundColor(ColorConstants.green);
		add(labelName, ToolbarLayout.ALIGN_CENTER);
		//setConstraint(labelName, new Rectangle(5, 5, -1, -1));
		
		//Bus port
		initiatorAnchors = new NoCFixedAnchor(this);
		//initiatorAnchors.setPlace(new Point(1, 2));
		initiatorAnchors.setPlace(1, 0);
		String s1 = "NIBUSinitiatorPort" + labelName.getText();
		putSourceAnchors(s1, initiatorAnchors);
		
		targetAnchors = new NoCFixedAnchor(this);
		//targetAnchors.setPlace(new Point(1, 2));
		targetAnchors.setPlace(1, 0);
		String s2 = "NIBUStargetPort" + labelName.getText();
		putTargetAnchors(s2, targetAnchors);
		
		
		//Router port
		initiatorAnchors = new NoCFixedAnchor(this);
		//initiatorAnchors.setPlace(new Point(1, 2));
		initiatorAnchors.setPlace(1, 2);
		String s3 = "NIRouterinitiatorPort" + labelName.getText();
		putSourceAnchors(s3, initiatorAnchors);
		
		targetAnchors = new NoCFixedAnchor(this);
		//targetAnchors.setPlace(new Point(1, 2));
		targetAnchors.setPlace(1, 2);
		String s4 = "NIRoutertargetPort" + labelName.getText();
		putTargetAnchors(s4, targetAnchors);
		
		
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
	
	public void setInitiatorAnchors(NoCFixedAnchor fa) {
		initiatorAnchors = fa;
	}
	
	public void setTargetAnchors(NoCFixedAnchor fa) {
		targetAnchors = fa;
	}
	
	public NoCFixedAnchor getInitiatorAnchors() {
		return initiatorAnchors;
	}
	
	public NoCFixedAnchor getTargetAnchors() {
		return targetAnchors;
	}
	
	public void paintFigure(Graphics g)	{
		Rectangle r = bounds;
		message = "PE";
		g.drawText(message, r.x + r.width/4, r.y + r.height/4);
		Rectangle rn = new Rectangle(r.x, r.y, r.width-1, r.height-1);
		g.drawRoundRectangle(rn, r.width/3, r.height/3);
		
		Rectangle anchorBusFigure = new Rectangle (r.x+1*r.width/2-r.width/10, r.y+0*r.height/2-r.width/10, r.width/5, r.width/5);
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorBusFigure);
		g.fillRectangle(anchorBusFigure);
		
		
		Rectangle anchorRouterFigure = new Rectangle (r.x+1*r.width/2-r.width/10, r.y+2*r.height/2-r.width/10, r.width/5, r.width/5);
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorRouterFigure);
		g.fillRectangle(anchorRouterFigure);
		
		
	}	
}
