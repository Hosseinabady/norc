package com.hosseinabady.norc_designer_plugin.commands;


import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCDynamicCore;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;

public class NoRCDynamicCoreCreateCommand extends Command {
	private NoCProcessingElement pe;
	private NoCDynamicCore       dyCore;
	
	public NoRCDynamicCoreCreateCommand() {
		super();
		pe     = null;
		dyCore = null;
	}
	
	public void setNoRCProcessingElement(Object p) {
		if (p instanceof NoCProcessingElement) 
			this.pe = (NoCProcessingElement)p;
	}
	
	public void setNoRCDynamicCore(Object dc) {
		if (dc instanceof NoCDynamicCore) 
			this.dyCore = (NoCDynamicCore)dc;
	}
	
	public void setLayout(Rectangle r) {
		if(dyCore == null) 
			return;
		dyCore.setLayout(r);
	}
	
	@Override
	public boolean canExecute() {
		if(pe == null || dyCore == null)
			return false;
		return true;
	}
	
	@Override
	public void execute() {
		pe.addChild(dyCore);
	}
	
	@Override
	public boolean canUndo() {
		if (pe == null || dyCore == null)
			return false;
		return pe.contains(dyCore);
	}
	
	@Override
	public void undo() {
		pe.removeChild(dyCore);
	}

}
