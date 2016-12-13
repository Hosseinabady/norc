package com.hosseinabady.norc_designer_plugin.editpart.tree;


import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;

import com.hosseinabady.noc_designer_plugin.model.NoCBus;
import com.hosseinabady.noc_designer_plugin.model.NoCDynamicCore;
import com.hosseinabady.noc_designer_plugin.model.NoCNetworkInterface;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.noc_designer_plugin.model.NoCRouter;
import com.hosseinabady.noc_designer_plugin.model.NoCStaticCore;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;

public class NoRCTreeEditPartFactory implements EditPartFactory {

	@Override
	public EditPart createEditPart(EditPart context, Object model) {
		// TODO Auto-generated method stub
		EditPart part = null;
		
		if (model instanceof NoCSystemOnChip)
			part = new NoRCSystemOnChipTreeEditPart();
		else if (model instanceof NoCRouter)
			part = new NoRCRouterTreeEditPart();
		else if (model instanceof NoCProcessingElement)
			part = new NoRCProcessingElementTreeEditPart();
		else if (model instanceof NoCBus)
			part = new NoRCBusTreeEditPart();
		else if (model instanceof NoCStaticCore)
			part = new NoRCStaticCoreTreeEditPart();
		else if (model instanceof NoCDynamicCore)
			part = new NoRCDynamicCoreTreeEditPart();
		else if (model instanceof NoCNetworkInterface)
			part = new NoRCNetworkInterfaceTreeEditPart();
		

		
		if (part != null)
			part.setModel(model);
		return part;
	}

}
