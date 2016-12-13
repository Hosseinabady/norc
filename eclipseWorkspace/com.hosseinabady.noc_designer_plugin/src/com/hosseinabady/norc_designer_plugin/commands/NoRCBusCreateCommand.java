package com.hosseinabady.norc_designer_plugin.commands;



import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCBus;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;

public class NoRCBusCreateCommand extends Command {
	private NoCProcessingElement pe;
	private NoCBus               bus;
	
	public NoRCBusCreateCommand() {
		super();
		pe  = null;
		bus = null;
	}
	
	public void setNoRCProcessingElement(Object p) {
		if ( p instanceof NoCProcessingElement)
			this.pe = (NoCProcessingElement)p;
	}

	public void setNoRCBus(Object b) {
		if (b instanceof NoCBus)
			this.bus = (NoCBus)b;
	}
	
	public void setLayout(Rectangle r) {
		if (bus == null)
			return;
		bus.setLayout(r);
	}
	
	@Override
	public boolean canExecute() {
		if (pe == null || bus == null)
			return false;
		return true;
	}
	
	@Override
	public void execute() {
		pe.addChild(bus);
	}
	
	@Override
	public boolean canUndo() {
		if (pe == null || bus == null)
			return false;
		return pe.contains(bus);
	}
	
	@Override
	public void undo() {
		pe.removeChild(bus);
	}
	
}
