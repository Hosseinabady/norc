package com.hosseinabady.norc_designer_plugin.commands;


import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;

import com.hosseinabady.noc_designer_plugin.model.NoCNode;

public class NoRCSetConstraintCommand extends Command {
	 private Point oldPos, newPos;
	  private Dimension oldSize, newSize;
	  private NoCNode act;
	 
	  public void execute()
	  {
	    oldSize = act.getSize();
	    oldPos = act.getLocation();
	    act.setLocation(newPos);
	    act.setSize(newSize);
	  }
	  
	  public void setLocation(Rectangle r)
	  {
	    setLocation(r.getLocation());
	    setSize(r.getSize());
	  }

	  public void setLocation(Point p)
	  {newPos = p;}

	  public void setPart(NoCNode part)
	  {this.act = part;}

	  public void setSize(Dimension p)
	  {newSize = p;}

	  public void redo()
	  {
	    act.setSize(newSize);
	    act.setLocation(newPos);
	  }

	  public void undo()
	  {
	    act.setSize(oldSize);
	    act.setLocation(oldPos);
	  }
}
