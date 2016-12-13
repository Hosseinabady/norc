package com.hosseinabady.norc_designer_plugin.commands;


import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCPath;

public class NoRCPathCommand extends Command {
	protected NoCNode oldSource, source;
	protected NoCNode oldTarget, target;
	protected String oldSourceName, sourceName;
	protected String oldTargetName, targetName;
	protected NoCPath path;
	
	public NoRCPathCommand() {
	}
	
	public boolean canExecute() {
		return true;
	}

	public void execute() {
		if (source != null) {
			path.detachSource();
			path.setSource(source);
			path.setSourceName(sourceName);
			path.attachSource();
		}
		if (target != null) {
			path.detachTarget();
			path.setTarget(target);
			path.setTargetName(targetName);
			path.attachTarget();
		}
		
	}
	public void redo() {
		execute();
	}
	public void undo() {
		source = path.getSource();
		target = path.getTarget();
		
		sourceName = path.getSourceName();
		targetName = path.getTargetName();
		
		path.detachSource();
		path.detachTarget();
		path.setSource(oldSource);
		path.setTarget(oldTarget);
		path.setSourceName(oldSourceName);
		path.setTargetName(oldTargetName);
		
		path.attachSource();
		path.attachTarget();
	}
	
	public void setSource(NoCNode newSource) {
		source = newSource;
	}
	
	public void setSourceName(String newSourceName) {
		sourceName = newSourceName;
	}
	public String getSourceName() {
		return sourceName;
	}
	
	public void setTarget(NoCNode newTarget) {
		target = newTarget;
	}
	
	
	public void setTargetName(String newTargetName) {
		targetName = newTargetName;
	}
	
	public String getTargetName() {
		return targetName;
	}
	public void setPath(NoCPath p) {
		path = p;
		oldSource = p.getSource();
		oldTarget = p.getTarget();
		oldSourceName = p.getSourceName();
		oldTargetName = p.getTargetName(); 
	}
}
