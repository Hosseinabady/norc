package com.hosseinabady.norc_designer_plugin.property;

import java.util.ArrayList;


import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.ColorPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

import com.hosseinabady.noc_designer_plugin.model.NoCAbstractElements;
import com.hosseinabady.noc_designer_plugin.model.NoCNode;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.noc_designer_plugin.model.NoCRouter;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;

public class NoRCNodePropertySource implements IPropertySource {
	
	private NoCAbstractElements node;
	
	public NoRCNodePropertySource(NoCNode node) {
		this.node = node;
	}
	@Override
	public Object getEditableValue() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public IPropertyDescriptor[] getPropertyDescriptors() {
		ArrayList<IPropertyDescriptor> properties = new ArrayList<IPropertyDescriptor>();
//		if (node instanceof NoRCProcessingElement)
//			properties.add(new TextPropertyDescriptor(NoRCNode.PROPERTY_RENAME, "Name"));
//		else
			properties.add(new TextPropertyDescriptor(NoCNode.PROPERTY_RENAME, "Name"));
		if (node instanceof NoCRouter) {
			properties.add(new ColorPropertyDescriptor(NoCRouter.PROPERTY_COLOR, "Color"));
			properties.add(new TextPropertyDescriptor(NoCNode.PROPERTY_LAYOUT, "Layout"));
			properties.add(new TextPropertyDescriptor(NoCRouter.PROPERTY_CHANNELCAPACITY, "Nomber of channel per port"));
			properties.add(new TextPropertyDescriptor(NoCRouter.PROPERTY_NOOFPORT, "Nomber of port"));
			properties.add(new TextPropertyDescriptor(NoCRouter.PROPERTY_CHANNELCAPACITY, "Channel Capacity"));
			
		}
		else if (node instanceof NoCSystemOnChip) {
			properties.add(new TextPropertyDescriptor(NoCSystemOnChip.PROPERTY_CAPITAL, "Capital"));
		}		

		return properties.toArray(new IPropertyDescriptor[0]); 
		
	}

	@Override
	public Object getPropertyValue(Object id) {
		if (id.equals(NoCNode.PROPERTY_RENAME))
			return ((NoCNode)node).getName();
		if (id.equals(NoCNode.PROPERTY_LAYOUT))
			return ((NoCNode)node).getLayout();
		if (id.equals(NoCRouter.PROPERTY_CHANNELCAPACITY))
			return ((NoCRouter)node).getchannelCapacity();
		if (id.equals(NoCRouter.PROPERTY_NOOFCHANNELPERPORT))
			return ((NoCRouter)node).getNoOfChannelPerPort();
		if (id.equals(NoCRouter.PROPERTY_NOOFPORT))
			return ((NoCRouter)node).getNoOfPort();
		if (id.equals(NoCNode.PROPERTY_COLOR))
			return ((NoCNode)node).getColor(); 
		

		return null;
	}

	@Override	
	public boolean isPropertySet(Object id) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void resetPropertyValue(Object id) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setPropertyValue(Object id, Object value) {
		if (id.equals(NoCNode.PROPERTY_RENAME))
			((NoCNode)node).setName((String)value);
		else if (id.equals(NoCRouter.PROPERTY_LAYOUT)) {
			((NoCNode)node).setLayout((Rectangle)value);
		}
		else if (id.equals(NoCRouter.PROPERTY_COLOR)) {
			Color newColor = new Color(null, (RGB)value);
			((NoCRouter)node).setColor(newColor);
		}
		else if (id.equals(NoCRouter.PROPERTY_FLOOR)) {
			try {
				Integer floor = Integer.parseInt((String)value);
			}
			catch (NumberFormatException e) { }
		}
		else if (id.equals(NoCSystemOnChip.PROPERTY_CAPITAL)) {
			try {
				Integer capital = Integer.parseInt((String)value);
			}
			catch (NumberFormatException e) { }
		}
		else if (id.equals(NoCRouter.PROPERTY_CHANNELCAPACITY)) {
			try {
				Integer iValue = Integer.parseInt((String)value);
				((NoCRouter)node).setChannelCapacity((int)iValue);
			}
			catch (NumberFormatException e) { }
			
		}
		
		else if (id.equals(NoCRouter.PROPERTY_NOOFCHANNELPERPORT)) {
			try {
				Integer iValue = Integer.parseInt((String)value);
				((NoCRouter)node).setNoOfChannelPerPort((int)iValue);
			}
			catch (NumberFormatException e) { }
		}
		
		else if (id.equals(NoCRouter.PROPERTY_NOOFPORT)) {
			try {
				Integer iValue = Integer.parseInt((String)value);
				((NoCRouter)node).setNoOfPort((int)iValue);
			}
			catch (NumberFormatException e) { }
		}

	}

}
