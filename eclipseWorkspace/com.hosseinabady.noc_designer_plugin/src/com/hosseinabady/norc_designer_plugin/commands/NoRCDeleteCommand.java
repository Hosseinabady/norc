package com.hosseinabady.norc_designer_plugin.commands;

import java.util.ArrayList;
import java.util.List;



import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCAbstractElements;
import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCPath;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;

public class NoRCDeleteCommand extends Command {

	
	private NoCNode child;
	private NoCNode parent;
	private List sourceConnections = new ArrayList();
	private List targetConnections = new ArrayList();
	
	private void deleteConnections (NoCAbstractElements a) {
		if (a instanceof NoCSystemOnChip) {
			List children = ((NoCSystemOnChip)a).getChildren();
			for (int i = 0; i < children.size(); i++) {
				deleteConnections((NoCNode)children.get(i));
			} 
		} else if (a instanceof NoCProcessingElement) {
			List children = ((NoCProcessingElement)a).getChildren();
			for (int i = 0; i < children.size(); i++) {
				deleteConnections((NoCNode)children.get(i));
			} 
		} else{
			sourceConnections.addAll(((NoCNode)a).getSourceConnections());
			for (int i = 0; i < sourceConnections.size(); i++) {
				NoCPath path = (NoCPath)sourceConnections.get(i);
				path.detachSource();
				path.detachTarget();
			}
			targetConnections.addAll(((NoCNode)a).getTargetConnections());
			for (int i = 0; i < targetConnections.size(); i++) {
				NoCPath path = (NoCPath)targetConnections.get(i);
				path.detachSource();
				path.detachTarget();
			}
		} 
	}
	
	public void execute() {
		deleteConnections(child);
		this.parent.removeChild(child);
	}
	
	private void restoreConnections() {
		for (int i = 0; i < sourceConnections.size(); i++) {
			NoCPath path = (NoCPath)sourceConnections.get(i);
			path.getTarget().addTargetConnection(path);
			path.getSource().addSourceConnection(path);
		}
		sourceConnections.clear();
		for (int i = 0; i < targetConnections.size(); i++) {
			NoCPath path = (NoCPath)targetConnections.get(i);
			path.getSource().addSourceConnection(path);
			path.getTarget().addTargetConnection(path);
		}
		targetConnections.clear();
	}
	
	
	public void setModel(Object model) {
		this.child = (NoCNode)model;
	}
	
	public void setParentModel(Object model) {
		//parent =(NoRCSystemOnChip)model;
		parent =(NoCNode)model;
	}
	
	public void redo() {
		execute();
	}
	public void setChild(NoCNode a) {
		child = a;
	}
	
	public void setParent(NoCSystemOnChip a) {
		parent = a;
	}
	
	public void undo() {
		this.parent.addChild(child);
		restoreConnections();
	}
	
	
}
