package com.hosseinabady.norc_designer_plugin.commands;



import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCNetworkInterface;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;

public class NoRCNetworkInterfaceCreateCommand extends Command {
	private NoCProcessingElement pe;
	private NoCNetworkInterface       networkIntrf;
	
	public NoRCNetworkInterfaceCreateCommand() {
		super();
		pe     = null;
		networkIntrf = null;
	}
	
	public void setNoRCProcessingElement(Object p) {
		if (p instanceof NoCProcessingElement) 
			this.pe = (NoCProcessingElement)p;
	}
	
	public void setNoRCNetworkInterface(Object dc) {
		if (dc instanceof NoCNetworkInterface) 
			this.networkIntrf = (NoCNetworkInterface)dc;
	}
	
	public void setLayout(Rectangle r) {
		if(networkIntrf == null) 
			return;
		networkIntrf.setLayout(r);
	}
	
	@Override
	public boolean canExecute() {
		if(pe == null || networkIntrf == null)
			return false;
		return true;
	}
	
	@Override
	public void execute() {
		pe.addChild(networkIntrf);
	}
	
	@Override
	public boolean canUndo() {
		if (pe == null || networkIntrf == null)
			return false;
		return pe.contains(networkIntrf);
	}
	
	@Override
	public void undo() {
		pe.removeChild(networkIntrf);
	}

}
