package com.hosseinabady.norc_designer_plugin.figure;

import org.eclipse.draw2d.IFigure;

public class NoCFigureFactory {
	public static IFigure createRouterFigure()
	  {
	    return new NoCRouterFigure();
	  }

	  public static IFigure createProcessingElemen()
	  {
	    return new NoCProcessingElementFigure();
	  }
	  
	  public static IFigure createNetworkInterface()
	  {
	    return new NoCNetworkInterfaceFigure();
	  }
	  
	  public static IFigure createStaticCore()
	  {
	    return new NoCStaticCoreFigure();
	  }
	  
	  public static IFigure createDynamicCore()
	  {
	    return new NoCDynamicCoreFigure();
	  }
	  
	  public static IFigure createBus()
	  {
	    return new NoCBusFigure();
	  }

	  public static NoCPathFigure createPathFigure()
	  {
	    return new NoCPathFigure();
	  }
	  
	  public static NoCSystemOnChipFigure createSystemOnChipFigure()
	  {
	    return new NoCSystemOnChipFigure();
	  }
}
