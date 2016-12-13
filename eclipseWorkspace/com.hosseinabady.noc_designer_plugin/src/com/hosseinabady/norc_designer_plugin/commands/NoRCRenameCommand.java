package com.hosseinabady.norc_designer_plugin.commands;


import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;

public class NoRCRenameCommand extends Command {
	  private NoCNode model;
	  private String oldName;
	  private String newName;

	  public boolean canExecute() {
			return true;
		}
	  
	  public void execute() {
		  this.oldName = model.getName();
		  this.model.setName(newName);
	  }

	  public void setModel(Object model) {
		  this.model = (NoCNode)model;
	  }
	  
	  public void setNewName(String newName) {
		  this.newName = newName;
	  }

	  public void undo() {
		  this.model.setName(oldName);
	  }

}
