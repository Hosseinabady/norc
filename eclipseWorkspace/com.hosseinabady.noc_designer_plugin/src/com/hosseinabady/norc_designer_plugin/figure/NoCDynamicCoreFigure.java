package com.hosseinabady.norc_designer_plugin.figure;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

public class NoCDynamicCoreFigure extends NoCAbstractBaseFigure {
	private Label labelName = new Label();
	private NoCFixedAnchor initiatorAnchors;
	private NoCFixedAnchor targetAnchors;
	public static final int DYNAMICCORE_FIGURE_DEFHEIGHT = 80;
	public static final int DYNAMICCORE_FIGURE_DEFWIDTH = 80;
	
	public NoCDynamicCoreFigure() {
		ToolbarLayout layout = new ToolbarLayout();
		setLayoutManager(layout);
		
		labelName.setForegroundColor(ColorConstants.black);
		add(labelName, ToolbarLayout.ALIGN_CENTER);
		
		
		setForegroundColor(ColorConstants.green);
		setBackgroundColor(ColorConstants.lightGreen);
		
		//setBorder(new LineBorder(3));
		setOpaque(true);
		
		initiatorAnchors = new NoCFixedAnchor(this);
		//initiatorAnchors.setPlace(new Point(1, 0));
		initiatorAnchors.setPlace(1, 2);
		String s1 = "DynamicCoreinitiatorPort" + labelName.getText();
		putSourceAnchors(s1, initiatorAnchors);
		
		targetAnchors = new NoCFixedAnchor(this);
		//targetAnchors.setPlace(new Point(1, 0));
		targetAnchors.setPlace(1, 2);
		String s2 = "DynamicCoretargetPort" + labelName.getText();
		putTargetAnchors(s2, targetAnchors);
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
		message = "DynamicCore";
		g.drawText(message, r.x + r.width/4, r.y + r.height/4);
		Rectangle rn = new Rectangle(r.x, r.y, r.width-1, r.height-1);
		g.drawRoundRectangle(rn, r.width/3, r.height/3);
		
		Rectangle anchorFigure = new Rectangle (r.x+1*r.width/2-r.width/10, r.y+2*r.height/2-r.width/10, r.width/5, r.width/5);
		setBackgroundColor(ColorConstants.blue);
		g.drawRectangle(anchorFigure);
		g.fillRectangle(anchorFigure);
		
		
	}	
}
