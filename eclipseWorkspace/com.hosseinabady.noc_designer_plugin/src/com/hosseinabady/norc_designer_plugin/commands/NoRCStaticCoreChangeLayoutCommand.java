package com.hosseinabady.norc_designer_plugin.commands;


import org.eclipse.draw2d.geometry.Rectangle;

import com.hosseinabady.noc_designer_plugin.model.NoCStaticCore;

public class NoRCStaticCoreChangeLayoutCommand extends AbstractLayoutCommand {

	private NoCStaticCore model;
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
		this.model = (NoCStaticCore)model;
		this.oldLayout = ((NoCStaticCore)model).getLayout();
	}
	public void undo() {
		this.model.setLayout(this.oldLayout);
	}
}
