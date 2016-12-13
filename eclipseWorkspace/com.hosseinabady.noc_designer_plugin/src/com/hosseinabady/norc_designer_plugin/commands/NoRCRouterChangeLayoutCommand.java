package com.hosseinabady.norc_designer_plugin.commands;


import org.eclipse.draw2d.geometry.Rectangle;

import com.hosseinabady.noc_designer_plugin.model.NoCRouter;

public class NoRCRouterChangeLayoutCommand extends AbstractLayoutCommand {

	private NoCRouter model;
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
		this.model = (NoCRouter)model;
		this.oldLayout = ((NoCRouter)model).getLayout();
	}
	public void undo() {
		this.model.setLayout(this.oldLayout);
	}
}
