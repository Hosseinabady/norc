package com.hosseinabady.noc_designer_plugin.model;


import java.util.ArrayList;
import java.util.List;
import java.util.Vector;



import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.ui.views.properties.IPropertySource;

import com.hosseinabady.norc_designer_plugin.property.NoRCNodePropertySource;


public class NoCNode extends NoCAbstractElements  implements IAdaptable{

	//Graphical properties
	private String        name;
	private Rectangle     layout;
	private Point         location = new Point(0, 0);
	private Dimension     size = new Dimension(-1, -1);
	private List<NoCNode> children;
	private NoCNode       parent;
	private Color         color;
	
	// Design properties 
	
	////////////////////////////SystemC Information//////////////////////////////
	private String systemCFileName;
	
	/////////////////////////////////////////////////////////////////////////////
	
	private static final long serialVersionUID = 1;
	
	public  static final String PROPERTY_LAYOUT = "NoRCNodeLayout"; 
	
	public  static final String PROPERTY_ADD = "NoRCNodeAddChild";
	public  static final String PROPERTY_REMOVE = "NoRCNodeRemoveChild";
	public  static final String PROPERTY_COLOR = "NoRCNodeColor";
	public  static final String PROPERTY_WIDTH_X = "NoRCNodeWidthX";
	public  static final String PROPERTY_HIGHT_Y = "NoRCNodeWidthY";
	
	private Vector<NoCLineConnectionModel> sourceConnections = new Vector<NoCLineConnectionModel>(2);
	private Vector<NoCLineConnectionModel> targetConnections = new Vector<NoCLineConnectionModel>(2);
	
	private IPropertySource propertySource = null;
	
	
	public NoCNode() {
		this.name = "Unknown";
		this.layout = new Rectangle(10, 10, 200, 200);
		this.children = new ArrayList<NoCNode>();
		this.parent = null;
		this.color = new Color(null, 
				(new Double(Math.random() * 128)).intValue() + 128 , 
				(new Double(Math.random() * 128)).intValue() + 128 , 
				(new Double(Math.random() * 128)).intValue() + 128 );

	}
	
	public void setName(String name) {
		if (name.equals(this.name)) return;
		String oldName = this.name;
		this.name = name;
		firePropertyChange(PROPERTY_RENAME, oldName, this.name);
	}
	
	public String getName() {
		return this.name;
	}
	
	public void setSystemCFileName(String systemCFileName) {
		if (systemCFileName.equals(this.systemCFileName)) return;
		String oldName = this.systemCFileName;
		this.systemCFileName = systemCFileName;
		firePropertyChange(PROPERTY_RENAME, oldName, this.systemCFileName);
	}
	
	public String getSystemCFileName() {
		return this.systemCFileName;
	}
	
	public Dimension getSize() {
		return size;
	}
	
	public void setSize(Dimension dim) {
		if (size.equals(dim)) return;
		size = dim;
		firePropertyChange(SIZE, null, size);
	}
	
	public Point getLocation() {
		return location;
	}
	
	public void setLocation(Point place) {
		if (location.equals(place)) return;
		location = place;
		firePropertyChange(LOC, null, place);
	}
	
	
	public Color getColor() {
		return this.color;
	}
	
	public void setColor(Color newColor) {
		Color oldColor = this.color;
		this.color = newColor;
		getListeners().firePropertyChange(PROPERTY_COLOR, oldColor, newColor);
	}
	public void setLayout(Rectangle newLayout) {
		Rectangle oldLayout = this.layout;
		this.layout = newLayout;
		getListeners().firePropertyChange(PROPERTY_LAYOUT, oldLayout, newLayout);
	}
	
	public Rectangle getLayout() {
		return this.layout;
	}
	
	public boolean addChild(NoCNode child) {
		boolean b = this.children.add(child);
		if (b) {
			child.setParent(this);
			getListeners().firePropertyChange(PROPERTY_ADD, null, child);
		}
		return b;
	}
	
	public boolean removeChild(NoCNode child) {
		boolean b = this.children.remove(child);
		if (b) {
			getListeners().firePropertyChange(PROPERTY_REMOVE, child, null);
		}
		return b;
	}
	
	public List<NoCNode> getChildrenArray() {
		return this.children;
	}
	
	public void setParent(NoCNode parent) {
		this.parent = parent;
	}
	
	public NoCNode getParent() {
		return this.parent;
	}
	
	
	@Override
	public Object getAdapter(Class adapter) {
		if (adapter == IPropertySource.class) {
			if (propertySource == null)
				propertySource = new NoRCNodePropertySource(this);
			return propertySource;
		}
		return null;
	}
	public boolean contains(NoCNode child) {
		return children.contains(child);
	}
	
	 public Vector getConnections()
	  {	
	  	Vector v = getSourceConnections();
	  	v.addAll(getTargetConnections());
	  	return v;
	  }
	  
	  public Vector getSourceConnections()
	  {
	    return (Vector<?>)sourceConnections.clone();
	  }
	
	  public void addSourceConnection(NoCLineConnectionModel p)
	  {
		  sourceConnections.add(p);
		  firePropertyChange(SOURCES, null, p);
	
	  }
	
	  public void removeSourceConnection(NoCLineConnectionModel p)
	  {
	    sourceConnections.remove(p);
	    firePropertyChange(SOURCES, null, p);

	  }
	
	  public Vector getTargetConnections()
	  {
	    return targetConnections;
	  }
	  
	  public void addTargetConnection(NoCLineConnectionModel p)
	  {
		  targetConnections.add(p);
		  firePropertyChange(TARGETS, null, p);
	  }
	  

	  public void removeTargetConnection(NoCLineConnectionModel p)
	  {
	    targetConnections.remove(p);
	    firePropertyChange(TARGETS, null, p);
	  }
	  public Object getEditableValue()
	  {return null;}
	  
	  public boolean isPropertySet(Object id)
	  {return true;}
	  
	  public void resetPropertyValue(Object id)
	  {}
}

