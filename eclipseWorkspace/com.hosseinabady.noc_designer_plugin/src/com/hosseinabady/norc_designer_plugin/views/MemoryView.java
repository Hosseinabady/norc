package com.hosseinabady.norc_designer_plugin.views;

import java.util.Iterator;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.GroupMarker;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.layout.TableColumnLayout;
import org.eclipse.jface.viewers.ColumnPixelData;
import org.eclipse.jface.viewers.ColumnWeightData;
import org.eclipse.jface.viewers.IBaseLabelProvider;
import org.eclipse.jface.viewers.IContentProvider;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TableViewerColumn;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Table;

import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.part.ViewPart;



public class MemoryView extends ViewPart {
	public static final String ID = "norc_designer_plugin.views.memory_view";
	private TableViewer viewer;
	
	private TableViewerColumn addColumn;
	private TableViewerColumn valueColumn;
	
	Action addItemAction, deleteItemAction, selectAllAction;

	public MemoryView() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public void createPartControl(Composite parent) {
		// TODO Auto-generated method stub
		createTableViewer(parent);

		
		viewer.setInput(CellProvider.getInstance().getCells());
		
		
		createActions();
		createMenu();
		createToolbar();
		createContextMenu();
		hookGlobalActions();
	}

	@Override
	public void setFocus() {
		viewer.getControl().setFocus();
		// TODO Auto-generated method stub

	}
	
	private void createTableViewer(Composite parent) {
		
	      viewer =
	            new TableViewer(parent, SWT.H_SCROLL | SWT.V_SCROLL
	                  | SWT.MULTI | SWT.FULL_SELECTION);
	      final Table table = viewer.getTable();

//	      TableColumnLayout layout = new TableColumnLayout();
//	      parent.setLayout(layout);

	      addColumn = new TableViewerColumn(viewer, SWT.NONE);
	      addColumn.getColumn().setText("Address");
	      addColumn.getColumn().setWidth(200);
	      addColumn.setEditingSupport(new CellEditingSupport(viewer, 0));

	      valueColumn = new TableViewerColumn(viewer, SWT.NONE);
	      valueColumn.getColumn().setText("Value");
	      valueColumn.getColumn().setWidth(200);
	      valueColumn.setEditingSupport(new CellEditingSupport(viewer, 1));
    
	      
	      
	      table.setHeaderVisible(true);
	      table.setLinesVisible(true);

	      viewer.setContentProvider(new CellContentProvider());
	      viewer.setLabelProvider(new CellLabelProvider());

	      getSite().setSelectionProvider(viewer);
	   }

	public void createActions() {
		addItemAction = new Action("Add...") {
			public void run() { 
				addItem();
			}
		};
		deleteItemAction = new Action("Delete") {
			public void run() {
				deleteItem();
			}
		};
		selectAllAction = new Action("Select All") {
			public void run() {
				selectAll();
			}
		};
		viewer.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				updateActionEnablement();
			}
		});
	}
	/**
	 * Create menu.
	 */
	private void createMenu() {
		IMenuManager mgr = getViewSite().getActionBars().getMenuManager();
		mgr.add(selectAllAction);
	}
	
	/**
	 * Create toolbar.
	 */
	private void createToolbar() {
		IToolBarManager mgr = getViewSite().getActionBars().getToolBarManager();
		mgr.add(addItemAction);
		mgr.add(deleteItemAction);
	}
		
	/**
	 * Create context menu.
	 */
	private void createContextMenu() {
		// Create menu manager.
		MenuManager menuMgr = new MenuManager();
		menuMgr.setRemoveAllWhenShown(true);
		menuMgr.addMenuListener(new IMenuListener() {
			public void menuAboutToShow(IMenuManager mgr) {
				fillContextMenu(mgr);
			}
		});
		
		// Create menu.
		Menu menu = menuMgr.createContextMenu(viewer.getControl());
		viewer.getControl().setMenu(menu);
		
		// Register menu for extension.
		getSite().registerContextMenu(menuMgr, viewer);
	}

	/**
	 * Hook global actions
	 */
	private void hookGlobalActions() {
		IActionBars bars = getViewSite().getActionBars();
		bars.setGlobalActionHandler(IWorkbenchActionConstants.SELECT_ALL, selectAllAction);
		bars.setGlobalActionHandler(IWorkbenchActionConstants.DELETE, deleteItemAction);
		viewer.getControl().addKeyListener(new KeyAdapter() {
			public void keyPressed(KeyEvent event) {
				if (event.character == SWT.DEL && 
					event.stateMask == 0 && 
					deleteItemAction.isEnabled()) 
				{
					deleteItemAction.run();
				}
			}
		});
	}
		
	private void fillContextMenu(IMenuManager mgr) {
		mgr.add(addItemAction);
		mgr.add(new GroupMarker(IWorkbenchActionConstants.MB_ADDITIONS));
		mgr.add(deleteItemAction);
		mgr.add(new Separator());
		mgr.add(selectAllAction);
	}

	private void updateActionEnablement() {
		IStructuredSelection sel = 
			(IStructuredSelection)viewer.getSelection();
		deleteItemAction.setEnabled(sel.size() > 0);
	}
	
	/**
	 * Add item to list.
	 */
	private void addItem() {
		String name = promptForValue("Enter address:", null);
		if (name != null) {

//			****
//			viewer.setSelection(new StructuredSelection(****));
		}
	}
	
	/**
	 * Remove item from list.
	 */
	private void deleteItem() {
		IStructuredSelection sel = 
			(IStructuredSelection)viewer.getSelection();
		Iterator iter = sel.iterator();
		while (iter.hasNext()) {
//			Word word = (Word)iter.next();
			//input.remove(word);
		}
	}

	/**
	 * Select all items.
	 */
	private void selectAll() {
//		viewer.getList().selectAll();
		updateActionEnablement();
	}
		
	/**
	 * Ask user for value.
	 */
	private String promptForValue(String text, String oldValue) {
		InputDialog dlg = new InputDialog(getSite().getShell(), 
			"List View", text, oldValue, null);
		if (dlg.open() != Window.OK)
			return null;
		return dlg.getValue();
	}

}
