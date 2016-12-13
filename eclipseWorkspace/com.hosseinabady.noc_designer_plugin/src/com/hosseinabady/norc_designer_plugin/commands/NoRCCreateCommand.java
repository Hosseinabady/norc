package com.hosseinabady.norc_designer_plugin.commands;


import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;

public class NoRCCreateCommand extends Command {

	private NoCSystemOnChip parent;
	private NoCNode         child;
	private Rectangle        rect;
	
	public void execute() {
		if (rect != null) {
			child.setLocation(rect.getLocation());
			if (!rect.isEmpty())
				child.setSize(rect.getSize());
		}
		parent.addChild(child);
	}
	
	public void setParent(NoCSystemOnChip sa) {
		parent = sa;
	}
	
	public void setChild(NoCNode activity) {
		child = activity;
	}
	public void setConstraint(Rectangle bounds) {
		rect = bounds;
	}
	
	public void redo() {
		execute();
	}
	
	public void undo() {
		parent.removeChild(child);
	}
	
}
