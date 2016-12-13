package com.hosseinabady.norc_designer_plugin.figure;

import org.eclipse.draw2d.ColorConstants;

import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LineBorder;

import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;

public class NoCSystemOnChipFigure extends NoCAbstractBaseFigure {
	
	private Label labelName = new Label();
	private Label labelAddress = new Label();
	
    private XYLayout layout;
	
	public NoCSystemOnChipFigure() {
		layout = new XYLayout();
		setLayoutManager(layout);
		
		labelName.setForegroundColor(ColorConstants.blue);
		add(labelName);
		setConstraint(labelName, new Rectangle(5, 5, -1, -1));
		
		labelAddress.setForegroundColor(ColorConstants.lightBlue);
		add(labelAddress);
		setConstraint(labelAddress, new Rectangle(5, 17, -1, -1));
		
		
		setForegroundColor(ColorConstants.black);
		setBorder(new LineBorder(5));
	}
	public void setLayout(Rectangle rect) {
		setBounds(rect);
	}
	
	public void setName(String text) {
		labelName.setText(text);
	}
	
	public void setAddress(String text) {
		labelAddress.setText(text);
	}

}
