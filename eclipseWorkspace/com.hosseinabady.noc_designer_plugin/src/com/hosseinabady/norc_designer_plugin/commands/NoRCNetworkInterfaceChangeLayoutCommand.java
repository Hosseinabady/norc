package com.hosseinabady.norc_designer_plugin.commands;


import org.eclipse.draw2d.geometry.Rectangle;

import com.hosseinabady.noc_designer_plugin.model.NoCNetworkInterface;

public class NoRCNetworkInterfaceChangeLayoutCommand extends
		AbstractLayoutCommand {

	private NoCNetworkInterface model;
	private Rectangle layout;
	private Rectangle oldLayout;
	
	public void execute() {
		model.setLayout(layout);
	}
	
	@Override
	public void setConstraint(Rectangle rect) {
		// TODO Auto-generated method stub
		this.layout = rect;
	}

	@Override
	public void setModel(Object model) {
		// TODO Auto-generated method stub
		this.model = (NoCNetworkInterface)model;
		this.oldLayout = ((NoCNetworkInterface)model).getLayout();
	}
	public void undo() {
		this.model.setLayout(this.oldLayout);
	}
}
