package com.hosseinabady.norc_designer_plugin.editpolicies;


import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.gef.editpolicies.XYLayoutEditPolicy;
import org.eclipse.gef.requests.CreateRequest;

import com.hosseinabady.noc_designer_plugin.model.NoCBus;
import com.hosseinabady.noc_designer_plugin.model.NoCDynamicCore;
import com.hosseinabady.noc_designer_plugin.model.NoCNetworkInterface;
import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.noc_designer_plugin.model.NoCRouter;
import com.hosseinabady.noc_designer_plugin.model.NoCStaticCore;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;
import com.hosseinabady.norc_designer_plugin.commands.AbstractLayoutCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCBusChangeLayoutCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCBusCreateCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCCreateCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCDynamicCoreChangeLayoutCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCDynamicCoreCreateCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCNetworkInterfaceChangeLayoutCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCNetworkInterfaceCreateCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCProcessingElementChangeLayoutCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCProcessingElementCreateCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCRouterChangeLayoutCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCRouterCreateCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCStaticCoreChangeLayoutCommand;
import com.hosseinabady.norc_designer_plugin.commands.NoRCStaticCoreCreateCommand;
import com.hosseinabady.norc_designer_plugin.editpart.NoCBusEditPart;
import com.hosseinabady.norc_designer_plugin.editpart.NoCDynamicCoreEditPart;
import com.hosseinabady.norc_designer_plugin.editpart.NoCNetworkInterfaceEditPart;
import com.hosseinabady.norc_designer_plugin.editpart.NoCProcessingElementEditPart;
import com.hosseinabady.norc_designer_plugin.editpart.NoCRouterEditPart;
import com.hosseinabady.norc_designer_plugin.editpart.NoCStaticCoreEditPart;
import com.hosseinabady.norc_designer_plugin.editpart.NoCSystemOnChipEditPart;
import com.hosseinabady.norc_designer_plugin.figure.NoCBusFigure;
import com.hosseinabady.norc_designer_plugin.figure.NoCDynamicCoreFigure;
import com.hosseinabady.norc_designer_plugin.figure.NoCNetworkInterfaceFigure;
import com.hosseinabady.norc_designer_plugin.figure.NoCProcessingElementFigure;
import com.hosseinabady.norc_designer_plugin.figure.NoCRouterFigure;
import com.hosseinabady.norc_designer_plugin.figure.NoCStaticCoreFigure;

public class NoRCEditLayoutPolicy extends XYLayoutEditPolicy {

	@Override
	protected Command createChangeConstraintCommand(EditPart child,
			Object constraint) {
		// TODO Auto-generated method stub
		AbstractLayoutCommand command = null;
		
		if (child instanceof NoCBusEditPart) {
			command = new NoRCBusChangeLayoutCommand();
		} else if (child instanceof NoCProcessingElementEditPart) {
			command = new NoRCProcessingElementChangeLayoutCommand();
		} else if (child instanceof NoCDynamicCoreEditPart) {
			command = new NoRCDynamicCoreChangeLayoutCommand();
		} else if (child instanceof NoCStaticCoreEditPart) {
			command = new NoRCStaticCoreChangeLayoutCommand();
		} else if (child instanceof NoCRouterEditPart) {
			command = new NoRCRouterChangeLayoutCommand();
		} else if (child instanceof NoCNetworkInterfaceEditPart) {
			command = new NoRCNetworkInterfaceChangeLayoutCommand();
		}
		
		command.setModel(child.getModel());
		command.setConstraint((Rectangle)constraint);
		return command;
	}

	@Override
	protected Command getCreateCommand(CreateRequest request) {
		// TODO Auto-generated method stub
		if (request.getType()== REQ_CREATE && getHost() instanceof NoCSystemOnChipEditPart) {
			if ((Class<?>)(request.getNewObject().getClass()) == NoCRouter.class) {
				NoRCRouterCreateCommand cmd = new NoRCRouterCreateCommand();
  			    
				cmd.setSystemOnChip(getHost().getModel());
				cmd.setRouter(request.getNewObject());
					
				Rectangle constraint = (Rectangle)getConstraintFor(request);
				constraint.x = (constraint.x < 0) ? 0 : constraint.x;
				constraint.y = (constraint.y < 0) ? 0 : constraint.y;
				constraint.width = (constraint.width <= 0) ? NoCRouterFigure.ROUTER_FIGURE_DEFWIDTH : constraint.width;
				constraint.width = (constraint.width <= 0) ? NoCRouterFigure.ROUTER_FIGURE_DEFHEIGHT : constraint.height;
				cmd.setLayout(constraint);
				return cmd;
			} else if ((Class<?>)(request.getNewObject().getClass()) == NoCProcessingElement.class) {
				NoRCProcessingElementCreateCommand cmd = new NoRCProcessingElementCreateCommand();
  			    
				cmd.setSystemOnChip(getHost().getModel());
				cmd.setProcessingElement(request.getNewObject());
					
				Rectangle constraint = (Rectangle)getConstraintFor(request);
				constraint.x = (constraint.x < 0) ? 0 : constraint.x;
				constraint.y = (constraint.y < 0) ? 0 : constraint.y;
				constraint.width = (constraint.width <= 0) ? NoCProcessingElementFigure.PROCESSINGELEMENT_FIGURE_DEFWIDTH : constraint.width;
				constraint.width = (constraint.width <= 0) ? NoCProcessingElementFigure.PROCESSINGELEMENT_FIGURE_DEFHEIGHT : constraint.height;
				cmd.setLayout(constraint);
				return cmd;
			}
		}
		
		if (request.getType()== REQ_CREATE && getHost() instanceof NoCProcessingElementEditPart) {
			if ((Class<?>)(request.getNewObject().getClass()) == NoCBus.class) {
				NoRCBusCreateCommand cmd = new NoRCBusCreateCommand();
  			    
				cmd.setNoRCProcessingElement(getHost().getModel());
				cmd.setNoRCBus(request.getNewObject());
					
				Rectangle constraint = (Rectangle)getConstraintFor(request);
				constraint.x = (constraint.x < 0) ? 0 : constraint.x;
				constraint.y = (constraint.y < 0) ? 0 : constraint.y;
				constraint.width = (constraint.width <= 0) ? NoCBusFigure.BUS_FIGURE_DEFWIDTH : constraint.width;
				constraint.width = (constraint.width <= 0) ? NoCBusFigure.BUS_FIGURE_DEFHEIGHT : constraint.height;
				cmd.setLayout(constraint);
				return cmd;
			} else if ((Class<?>)(request.getNewObject().getClass()) == NoCDynamicCore.class) {
				NoRCDynamicCoreCreateCommand cmd = new NoRCDynamicCoreCreateCommand();
  			    
				cmd.setNoRCProcessingElement(getHost().getModel());
				cmd.setNoRCDynamicCore(request.getNewObject());
					
				Rectangle constraint = (Rectangle)getConstraintFor(request);
				constraint.x = (constraint.x < 0) ? 0 : constraint.x;
				constraint.y = (constraint.y < 0) ? 0 : constraint.y;
				constraint.width = (constraint.width <= 0) ? NoCDynamicCoreFigure.DYNAMICCORE_FIGURE_DEFWIDTH : constraint.width;
				constraint.width = (constraint.width <= 0) ? NoCDynamicCoreFigure.DYNAMICCORE_FIGURE_DEFHEIGHT : constraint.height;
				cmd.setLayout(constraint);
				return cmd;
			} else if ((Class<?>)(request.getNewObject().getClass()) == NoCStaticCore.class) {
				NoRCStaticCoreCreateCommand cmd = new NoRCStaticCoreCreateCommand();
  			    
				cmd.setNoRCProcessingElement(getHost().getModel());
				cmd.setNoRCStaticCore(request.getNewObject());
					
				Rectangle constraint = (Rectangle)getConstraintFor(request);
				constraint.x = (constraint.x < 0) ? 0 : constraint.x;
				constraint.y = (constraint.y < 0) ? 0 : constraint.y;
				constraint.width = (constraint.width <= 0) ? NoCStaticCoreFigure.STATICCORE_FIGURE_DEFWIDTH : constraint.width;
				constraint.width = (constraint.width <= 0) ? NoCStaticCoreFigure.STATICCORE_FIGURE_DEFHEIGHT : constraint.height;
				cmd.setLayout(constraint);
				return cmd;
			} else if ((Class<?>)(request.getNewObject().getClass()) == NoCNetworkInterface.class) {
				NoRCNetworkInterfaceCreateCommand cmd = new NoRCNetworkInterfaceCreateCommand();
  			    
				cmd.setNoRCProcessingElement(getHost().getModel());
				cmd.setNoRCNetworkInterface(request.getNewObject());
					
				Rectangle constraint = (Rectangle)getConstraintFor(request);
				constraint.x = (constraint.x < 0) ? 0 : constraint.x;
				constraint.y = (constraint.y < 0) ? 0 : constraint.y;
				constraint.width = (constraint.width <= 0) ? NoCNetworkInterfaceFigure.NETWORKINTERFACE_FIGURE_DEFWIDTH : constraint.width;
				constraint.width = (constraint.width <= 0) ? NoCNetworkInterfaceFigure.NETWORKINTERFACE_FIGURE_DEFHEIGHT : constraint.height;
				cmd.setLayout(constraint);
				return cmd;
			}
		}
			
		return null;
	}

	protected Command createAddCommand(EditPart childEditPart, Object constraint)  {
	    NoCNode part = (NoCNode) childEditPart.getModel();
	    Rectangle rect = (Rectangle)constraint;
	    NoRCCreateCommand create = new NoRCCreateCommand();
	    create.setParent((NoCSystemOnChip)(getHost().getModel()));
	    create.setChild(part);
	    create.setConstraint(rect);
	    return create;
	  }
	
	protected Command getDeleteDependantCommand(Request request) {
		return null;
	}
	  
	protected EditPolicy createChildEditPolicy(EditPart child) {
		ResizableEditPolicy policy = new ResizableEditPolicy();
	    policy.setResizeDirections(PositionConstants.EAST |
	    PositionConstants.WEST | PositionConstants.NORTH |
	    PositionConstants.SOUTH);
	    return policy;
	 }
}
