package com.hosseinabady.norc_designer_plugin.editpolicies;


import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.ComponentEditPolicy;
import org.eclipse.gef.requests.GroupRequest;

import com.hosseinabady.norc_designer_plugin.commands.NoRCDeleteCommand;

public class NoRCDeletePolicy extends ComponentEditPolicy {
	
	protected Command createDeleteCommand(GroupRequest deleteRequest) {
		NoRCDeleteCommand command = new NoRCDeleteCommand();
		command.setModel(getHost().getModel());
		command.setParentModel(getHost().getParent().getModel());
		return command;
	}
}
