package com.hosseinabady.norc_designer_plugin.editpolicies;


import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.GraphicalNodeEditPolicy;
import org.eclipse.gef.requests.CreateConnectionRequest;
import org.eclipse.gef.requests.ReconnectRequest;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCPath;
import com.hosseinabady.norc_designer_plugin.commands.NoRCPathCommand;
import com.hosseinabady.norc_designer_plugin.editpart.NoCNodeEditPart;
import com.hosseinabady.norc_designer_plugin.figure.NoCAbstractBaseFigure;
import com.hosseinabady.norc_designer_plugin.figure.NoCFigureFactory;

public class NoRCNodePolicy extends GraphicalNodeEditPolicy {

	  protected Connection createDummyConnection(Request req)
	  {
	    PolylineConnection conn = NoCFigureFactory.createPathFigure();
	    return conn;
	  }
	  
	  protected Command getConnectionCompleteCommand(CreateConnectionRequest request)
	  {
	    NoRCPathCommand command = (NoRCPathCommand)request.getStartCommand();
	    command.setTarget(getActivity());
	    ConnectionAnchor ctor = getActivityPart().
	      getTargetConnectionAnchor(request);
	    if (ctor == null)
	      return null;
	    command.setTargetName(getActivityPart().
	      mapTargetConnectionAnchorToName(ctor));
	    //System.out.println(command.getSourceName());
	    //System.out.println(command.getTargetName());
	    return command;
	  }

	  protected Command getConnectionCreateCommand
	    (CreateConnectionRequest request)
	  {
	    NoRCPathCommand command = new NoRCPathCommand();
	    command.setPath(new NoCPath());
	    command.setSource(getActivity());
	    ConnectionAnchor ctor = getActivityPart().
	      getSourceConnectionAnchor(request);
	    command.setSourceName(getActivityPart().
	    mapSourceConnectionAnchorToName(ctor));
	    request.setStartCommand(command);
	    return command;
	  }

	  protected NoCNodeEditPart getActivityPart()
	  {return (NoCNodeEditPart) getHost();}

	  protected NoCNode getActivity()
	  {return (NoCNode) getHost().getModel();}

	  protected Command getReconnectTargetCommand
	    (ReconnectRequest request)
	  {
	    NoRCPathCommand cmd = new NoRCPathCommand();
	    cmd.setPath((NoCPath)request.getConnectionEditPart().getModel());
	    ConnectionAnchor ctor = getActivityPart().
	      getTargetConnectionAnchor(request);
	    cmd.setTarget(getActivity());
	      cmd.setTargetName(getActivityPart().
	    mapTargetConnectionAnchorToName(ctor));
	    return cmd;
	  }

	  protected Command getReconnectSourceCommand
	    (ReconnectRequest request)
	  {

	    NoRCPathCommand cmd = new NoRCPathCommand();
	    cmd.setPath((NoCPath)request.getConnectionEditPart().getModel());
	    ConnectionAnchor ctor = getActivityPart().
	      getSourceConnectionAnchor(request);
	    cmd.setSource(getActivity());
	    cmd.setSourceName(getActivityPart().
	      mapSourceConnectionAnchorToName(ctor));
	    return cmd;
	  }

	  protected NoCAbstractBaseFigure getActivityFigure()
	  {
	    return (NoCAbstractBaseFigure)((GraphicalEditPart)getHost()).getFigure();
	  }

}
