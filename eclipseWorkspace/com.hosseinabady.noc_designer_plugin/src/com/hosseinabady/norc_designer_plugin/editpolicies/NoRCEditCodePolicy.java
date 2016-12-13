package com.hosseinabady.norc_designer_plugin.editpolicies;


import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.AbstractEditPolicy;

import com.hosseinabady.norc_designer_plugin.commands.NoRCRenameCommand;

public class NoRCEditCodePolicy extends AbstractEditPolicy {
	@Override
	public Command getCommand(Request request) {
		if (request.getType().equals("editcode"))
			return createRenameCommand(request);
		return null;
	}
	
	protected Command createRenameCommand(Request renameRequest) {
		NoRCRenameCommand command = new NoRCRenameCommand();
		command.setModel(getHost().getModel());
		command.setNewName((String)renameRequest.getExtendedData().get("editCode"));
		return command;
	}
}
