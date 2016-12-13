package com.hosseinabady.norc_designer_plugin.editpart;


import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;

import com.hosseinabady.noc_designer_plugin.model.NoCBus;
import com.hosseinabady.noc_designer_plugin.model.NoCDynamicCore;
import com.hosseinabady.noc_designer_plugin.model.NoCNetworkInterface;
import com.hosseinabady.noc_designer_plugin.model.NoCPath;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.noc_designer_plugin.model.NoCRouter;
import com.hosseinabady.noc_designer_plugin.model.NoCStaticCore;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;

public class NoCEditPartFactory implements EditPartFactory {

	@Override
	public EditPart createEditPart(EditPart context, Object model) {
		// TODO Auto-generated method stub
		AbstractGraphicalEditPart part = null;
		
		if (model instanceof NoCSystemOnChip) {
			part = new NoCSystemOnChipEditPart();
		} else if (model instanceof NoCProcessingElement) {
			part = new NoCProcessingElementEditPart();
		} else if (model instanceof NoCBus) {
			part = new NoCBusEditPart();
		} else if (model instanceof NoCRouter) {
			part = new NoCRouterEditPart();
		}  else if (model instanceof NoCStaticCore) { 
			part = new NoCStaticCoreEditPart();
		} else if (model instanceof NoCDynamicCore) { 
			part = new NoCDynamicCoreEditPart();
		} else if (model instanceof NoCNetworkInterface) { 
			part = new NoCNetworkInterfaceEditPart();
		}  else if (model instanceof NoCPath) {
			  part = new NoCPathEditPart();
		} 
		part.setModel(model);
		return part;
	}

}
