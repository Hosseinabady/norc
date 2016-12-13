package com.hosseinabady.norc_designer_plugin.commands;


import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.noc_designer_plugin.model.NoCStaticCore;

public class NoRCStaticCoreCreateCommand extends Command {
	private NoCProcessingElement pe;
	private NoCStaticCore       stCore;
	
	public NoRCStaticCoreCreateCommand() {
		super();
		pe     = null;
		stCore = null;
	}
	
	public void setNoRCProcessingElement(Object p) {
		if (p instanceof NoCProcessingElement) 
			this.pe = (NoCProcessingElement)p;
	}
	
	public void setNoRCStaticCore(Object dc) {
		if (dc instanceof NoCStaticCore) 
			this.stCore = (NoCStaticCore)dc;
	}
	
	public void setLayout(Rectangle r) {
		if(stCore == null) 
			return;
		stCore.setLayout(r);
	}
	
	@Override
	public boolean canExecute() {
		if(pe == null || stCore == null)
			return false;
		return true;
	}
	
	@Override
	public void execute() {
		pe.addChild(stCore);
	}
	
	@Override
	public boolean canUndo() {
		if (pe == null || stCore == null)
			return false;
		return pe.contains(stCore);
	}
	
	@Override
	public void undo() {
		pe.removeChild(stCore);
	}

}
