package com.hosseinabady.norc_designer_plugin.commands;




import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCRouter;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;

public class NoRCRouterCreateCommand extends Command {
	private NoCSystemOnChip en;
	private	NoCRouter srv;
	
	public NoRCRouterCreateCommand() {
		super();
		en = null;
		srv = null;
	}
	
	public void setRouter(Object s) {
		if (s instanceof NoCRouter)
			this.srv = (NoCRouter)s;
	}
	
	public void setSystemOnChip(Object e) {
		if (e instanceof NoCSystemOnChip)
			this.en = (NoCSystemOnChip)e;
	}
	
	public void setLayout(Rectangle r) {
		if (srv == null)
			return;
		srv.setLayout(r);
	}

	@Override
	public boolean canExecute() {
		if (srv == null || en == null)
			return false;
		return true;
	}
	
	@Override
	public void execute() {
		en.addChild(srv);
	}
	
	@Override
	public boolean canUndo() {
		if (en == null || srv == null)
			return false;
		return en.contains(srv);
	}
	
	@Override
	public void undo() {
		en.removeChild(srv);
	}
}
