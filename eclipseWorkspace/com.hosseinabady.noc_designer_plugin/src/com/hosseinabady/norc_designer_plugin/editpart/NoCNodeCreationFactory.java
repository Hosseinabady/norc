package com.hosseinabady.norc_designer_plugin.editpart;



import org.eclipse.gef.requests.CreationFactory;

import com.hosseinabady.noc_designer_plugin.model.NoCBus;
import com.hosseinabady.noc_designer_plugin.model.NoCDynamicCore;
import com.hosseinabady.noc_designer_plugin.model.NoCNetworkInterface;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.noc_designer_plugin.model.NoCRouter;
import com.hosseinabady.noc_designer_plugin.model.NoCStaticCore;

public class NoCNodeCreationFactory implements CreationFactory {

private Class<?> template;
	
	public NoCNodeCreationFactory(Class<?> t) {
		this.template = t;
	}
	
	//@Override
	public Object getNewObject() {	
		if (template == null)
			return null;
		if (template == NoCRouter.class) {
			NoCRouter srv = new NoCRouter();
			srv.setName("Router");
			srv.setChannelCapacity(1);
			srv.setNoOfChannelPerPort(1);
			srv.setNoOfPort(5);
			return srv;
		} else if (template == NoCProcessingElement.class) {
			NoCProcessingElement pe = new NoCProcessingElement();
			pe.setName("Processing Element");
			return pe;
		} else if (template == NoCBus.class) {
			NoCBus bus = new NoCBus();
			bus.setName("Bus");
			return bus;
		} else if (template == NoCDynamicCore.class) {
			NoCDynamicCore dynamicCore = new NoCDynamicCore();
			dynamicCore.setName("Dynamic Core");
			return dynamicCore;
		} else if (template == NoCNetworkInterface.class) {
				NoCNetworkInterface networkIntrf = new NoCNetworkInterface();
				networkIntrf.setName("Network Interface");
				return networkIntrf;
		} else if (template == NoCStaticCore.class) {
			NoCStaticCore staticCore = new NoCStaticCore();
			staticCore.setName("Network Interface");
			return staticCore;
	    }
	    return null;
	}

	//@Override
	public Object getObjectType() {
		return template;
	}


}
