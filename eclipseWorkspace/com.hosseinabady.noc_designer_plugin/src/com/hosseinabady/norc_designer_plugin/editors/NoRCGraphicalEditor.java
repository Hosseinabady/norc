package com.hosseinabady.norc_designer_plugin.editors;

import java.util.ArrayList;

import org.eclipse.gef.KeyHandler;



import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.Viewport;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.draw2d.parts.ScrollableThumbnail;
import org.eclipse.gef.ContextMenuProvider;
import org.eclipse.gef.DefaultEditDomain;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.KeyStroke;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.MouseWheelHandler;
import org.eclipse.gef.MouseWheelZoomHandler;
import org.eclipse.gef.editparts.ScalableRootEditPart;
import org.eclipse.gef.editparts.ZoomManager;
import org.eclipse.gef.palette.CombinedTemplateCreationEntry;
import org.eclipse.gef.palette.ConnectionCreationToolEntry;
import org.eclipse.gef.palette.MarqueeToolEntry;
import org.eclipse.gef.palette.PaletteGroup;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gef.palette.PaletteSeparator;
import org.eclipse.gef.palette.SelectionToolEntry;
import org.eclipse.gef.ui.actions.GEFActionConstants;
import org.eclipse.gef.ui.actions.ZoomInAction;
import org.eclipse.gef.ui.actions.ZoomOutAction;
import org.eclipse.gef.ui.parts.ContentOutlinePage;
import org.eclipse.gef.ui.parts.GraphicalEditorWithPalette;
import org.eclipse.gef.ui.parts.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.part.IPageSite;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;

import com.hosseinabady.noc_designer_plugin.NoCActivator;
import com.hosseinabady.noc_designer_plugin.NoCContextMenuProvider;
import com.hosseinabady.noc_designer_plugin.model.NoCBus;
import com.hosseinabady.noc_designer_plugin.model.NoCDynamicCore;
import com.hosseinabady.noc_designer_plugin.model.NoCNetworkInterface;
import com.hosseinabady.noc_designer_plugin.model.NoCProcessingElement;
import com.hosseinabady.noc_designer_plugin.model.NoCRouter;
import com.hosseinabady.noc_designer_plugin.model.NoCStaticCore;
import com.hosseinabady.noc_designer_plugin.model.NoCSystemOnChip;
import com.hosseinabady.norc_designer_plugin.editpart.NoCEditPartFactory;
import com.hosseinabady.norc_designer_plugin.editpart.NoCNodeCreationFactory;
import com.hosseinabady.norc_designer_plugin.editpart.tree.NoRCTreeEditPartFactory;

public class NoRCGraphicalEditor extends GraphicalEditorWithPalette {

	public static final String ID = "norc_designer_plugin.norcgraphicaleditor";

	private NoCSystemOnChip model; 
	private	KeyHandler  keyHandler;
	
	protected class OutlinePage extends ContentOutlinePage {
		private SashForm sash;
		private ScrollableThumbnail thumbnail;
		private DisposeListener disposeListener;
		
		public OutlinePage() {
			super(new TreeViewer());
		}
		
		public void createControl(Composite parent) {
			sash = new SashForm(parent, SWT.VERTICAL);
			
			getViewer().createControl(sash);
			
			getViewer().setEditDomain(getEditDomain());
			getViewer().setEditPartFactory(new NoRCTreeEditPartFactory());
			getViewer().setContents(model);
			
			getSelectionSynchronizer().addViewer(getViewer());
			
			Canvas canvas = new Canvas(sash, SWT.BORDER);
			LightweightSystem lws = new LightweightSystem(canvas);
			
			thumbnail = new ScrollableThumbnail((Viewport)((ScalableRootEditPart)getGraphicalViewer().getRootEditPart()).getFigure());
			thumbnail.setSource(((ScalableRootEditPart)getGraphicalViewer().getRootEditPart()).getLayer(LayerConstants.PRINTABLE_LAYERS));
			
			lws.setContents(thumbnail);
			
			disposeListener = new DisposeListener() {
				@Override
				public void widgetDisposed(DisposeEvent e) {
					if (thumbnail != null) {
						thumbnail.deactivate();
						thumbnail = null;
					}
				}
			};
			getGraphicalViewer().getControl().addDisposeListener(disposeListener);
		}
		
		public void init(IPageSite pageSite) {
			super.init(pageSite);
			
			IActionBars bars = getSite().getActionBars();
			bars.setGlobalActionHandler(ActionFactory.UNDO.getId(), getActionRegistry().getAction(ActionFactory.UNDO.getId()));
			bars.setGlobalActionHandler(ActionFactory.REDO.getId(), getActionRegistry().getAction(ActionFactory.REDO.getId()));
			bars.setGlobalActionHandler(ActionFactory.DELETE.getId(), getActionRegistry().getAction(ActionFactory.DELETE.getId()));
			bars.updateActionBars();
			
			getViewer().setKeyHandler(keyHandler);
			
			ContextMenuProvider provider = new NoCContextMenuProvider(getViewer(), getActionRegistry());
			getViewer().setContextMenu(provider);
		}
		public Control getControl() {
			return sash;
		}
		public void dispose() {
			getSelectionSynchronizer().removeViewer(getViewer());
			if (getGraphicalViewer().getControl() != null
					&& !getGraphicalViewer().getControl().isDisposed())
				getGraphicalViewer().getControl().removeDisposeListener(disposeListener);

			super.dispose();
		}
	}
	
	
	public NoCSystemOnChip CreateNoRCSystemOnChip() {
		
		NoCSystemOnChip psyNoRCSystemOnChip = new NoCSystemOnChip();
/*		
		psyNoRCSystemOnChip.setName("psyNoRC System On Chip");

		
		NoRCProcessingElement comptaProcessingElement = new NoRCProcessingElement();
		comptaProcessingElement.setName("comptaProcessingElement");
		comptaProcessingElement.setLayout(new Rectangle(30, 50, 250, 150));
		
		NoRCBus comptaBus = new NoRCBus();
		comptaBus.setName("comptaBus");
		comptaBus.setLayout(new Rectangle(100, 100, 90, 90));
		comptaProcessingElement.addChild(comptaBus);
		
		NoRCStaticCore comptaStaticCore = new NoRCStaticCore();
		comptaStaticCore.setName("comptaStaticCore");
		comptaStaticCore.setLayout(new Rectangle(40, 60, 90, 90));
		comptaProcessingElement.addChild(comptaStaticCore);
		
		NoRCDynamicCore comptaDynamicCore = new NoRCDynamicCore();
		comptaDynamicCore.setName("comptaDynamicCore");
		comptaDynamicCore.setLayout(new Rectangle(3, 6, 90, 90));
		comptaProcessingElement.addChild(comptaDynamicCore);
		
		NoRCNetworkInterface comptaNoRCNetworkInterface = new NoRCNetworkInterface();
		comptaNoRCNetworkInterface.setName("comptaNoRCNetworkInterface");
		comptaNoRCNetworkInterface.setLayout(new Rectangle(43, 6, 90, 90));
		comptaProcessingElement.addChild(comptaNoRCNetworkInterface);
		

		
		psyNoRCSystemOnChip.addChild(comptaProcessingElement);
	
	*/	
		return psyNoRCSystemOnChip;
	}
	
	protected void configureGraphicalViewer() {
		double[] zoomLevels;
		ArrayList<String> zoomContributions;
		
		super.configureGraphicalViewer();
		GraphicalViewer viewer = getGraphicalViewer();
		viewer.setEditPartFactory(new NoCEditPartFactory());
		
		ScalableRootEditPart rootEditPart = new ScalableRootEditPart();
		viewer.setRootEditPart(rootEditPart);
		
		ZoomManager manager = rootEditPart.getZoomManager();
		getActionRegistry().registerAction(new ZoomInAction(manager));
		getActionRegistry().registerAction(new ZoomOutAction(manager));
		
		zoomLevels = new double[] {0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 10.0, 20.0};
		
		manager.setZoomLevels(zoomLevels);
		
		zoomContributions = new ArrayList<String>(); 
		zoomContributions.add(ZoomManager.FIT_ALL);
		zoomContributions.add(ZoomManager.FIT_HEIGHT);
		zoomContributions.add(ZoomManager.FIT_WIDTH);
		manager.setZoomLevelContributions(zoomContributions);
		
		KeyHandler keyHandler = new KeyHandler(); 
		keyHandler.put(KeyStroke.getPressed(SWT.DEL, 127, 0), getActionRegistry().getAction(ActionFactory.DELETE.getId()));
		keyHandler.put(KeyStroke.getPressed('+', SWT.KEYPAD_ADD, 0), getActionRegistry().getAction(GEFActionConstants.ZOOM_IN));
		keyHandler.put(KeyStroke.getPressed('-', SWT.KEYPAD_SUBTRACT, 0), getActionRegistry().getAction(GEFActionConstants.ZOOM_OUT));
		
		viewer.setProperty(MouseWheelHandler.KeyGenerator.getKey(SWT.NONE), MouseWheelZoomHandler.SINGLETON);
		
		viewer.setKeyHandler(keyHandler);
		
		ContextMenuProvider provider = new NoCContextMenuProvider(viewer, getActionRegistry());
		viewer.setContextMenu(provider);
				
	}
	
	public Object getAdapter(Class type) {
		if (type == ZoomManager.class)
			return ((ScalableRootEditPart)getGraphicalViewer().getRootEditPart()).getZoomManager();
		if (type == IContentOutlinePage.class) {
			return new OutlinePage();
		}
		else
			return super.getAdapter(type);
	}
		
	public NoRCGraphicalEditor() {
		setEditDomain(new DefaultEditDomain(this));
	}
	
	@Override
	protected void initializeGraphicalViewer() {
		// TODO Auto-generated method stub
		GraphicalViewer viewer = getGraphicalViewer();
		model = CreateNoRCSystemOnChip();
		viewer.setContents(model);
		
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		// TODO Auto-generated method stub

	}

	protected PaletteRoot getPaletteRoot() {
		// 
		PaletteRoot root = new PaletteRoot();
		
		// 
		PaletteGroup manipGroup = new PaletteGroup("Manipulation tools");
		root.add(manipGroup);
		
		// 
		SelectionToolEntry selectionToolEntry = new SelectionToolEntry();
		manipGroup.add(selectionToolEntry);
		manipGroup.add(new MarqueeToolEntry());

		manipGroup.add(new ConnectionCreationToolEntry(
				"Connection_tool", "used to connect multiple components", null,
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/connection.png"), AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/connection.png")) );
		
		PaletteSeparator sep2 = new PaletteSeparator();
		root.add(sep2);

		PaletteGroup instGroup = new PaletteGroup("SoC Element");
		root.add(instGroup);
		
		instGroup.add(new CombinedTemplateCreationEntry("Router", "Router",
				NoCRouter.class, new NoCNodeCreationFactory(NoCRouter.class),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/router.png"),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/router.png")));

		instGroup.add(new CombinedTemplateCreationEntry("Processing Element", "Processing Element",
				NoCProcessingElement.class, new NoCNodeCreationFactory(NoCProcessingElement.class),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png"),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png")));
		
		PaletteSeparator sep3 = new PaletteSeparator();
		instGroup.add(sep3);
		
		instGroup.add(new CombinedTemplateCreationEntry("AMBA BUS", "Internal BUS",
				NoCBus.class, new NoCNodeCreationFactory(NoCBus.class),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png"),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png")));
		
		instGroup.add(new CombinedTemplateCreationEntry("Dynamic Core", "Dynamic Core",
				NoCDynamicCore.class, new NoCNodeCreationFactory(NoCDynamicCore.class),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png"),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png")));
		
		instGroup.add(new CombinedTemplateCreationEntry("Static Core", "Static Core",
				NoCStaticCore.class, new NoCNodeCreationFactory(NoCStaticCore.class),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png"),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png")));
		
		instGroup.add(new CombinedTemplateCreationEntry("Network Interface", "Network Interface",
				NoCNetworkInterface.class, new NoCNodeCreationFactory(NoCNetworkInterface.class),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png"),
				AbstractUIPlugin.imageDescriptorFromPlugin(NoCActivator.PLUGIN_ID, "icons/processingelement.png")));
		
			root.setDefaultEntry(selectionToolEntry);
		return root;
	}
}
