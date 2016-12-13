package com.hosseinabady.noc_designer_plugin;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;



public class Perspective implements IPerspectiveFactory {
	private static final String ID_TABS_FOLDER = "TutoGEF.tabs";
	
	public void createInitialLayout(IPageLayout layout) {
		String editorArea = layout.getEditorArea();
		layout.setEditorAreaVisible(true);
		
		IFolderLayout tabs = layout.createFolder(
				ID_TABS_FOLDER, IPageLayout.RIGHT, 0.8f, editorArea);
		tabs.addView(IPageLayout.ID_OUTLINE);
		tabs.addView(IPageLayout.ID_PROP_SHEET);
		layout.addStandaloneView("example.view", true , IPageLayout.LEFT, 0.2f, editorArea);

	}
}
