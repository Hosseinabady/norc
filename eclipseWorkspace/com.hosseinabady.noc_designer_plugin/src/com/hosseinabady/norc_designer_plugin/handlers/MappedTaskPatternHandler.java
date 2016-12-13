package com.hosseinabady.norc_designer_plugin.handlers;


import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;

import com.hosseinabady.norc_designer_plugin.data.NoRCMappedTaskPatternDataSet;
import com.hosseinabady.norc_designer_plugin.data.parsers.NoRCMappedTaskPatternSAXParser;
import com.hosseinabady.norc_designer_plugin.editors.NoRCMappedTaskPatternEditor;
import com.hosseinabady.norc_designer_plugin.editors.NoRCMappedTaskPatternEditorInput;
import com.hosseinabady.norc_designer_plugin.views.TaskView;

public class MappedTaskPatternHandler extends AbstractHandler {
	
	public Object execute(ExecutionEvent event) throws ExecutionException {
		
		NoRCMappedTaskPatternDataSet dataSet = new NoRCMappedTaskPatternDataSet();
		IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		
		NoRCMappedTaskPatternSAXParser rtp = new NoRCMappedTaskPatternSAXParser(); 

		dataSet = (rtp.run("D:\\Bristol_Research\\VPP_Project\\VPPNoRC\\log\\task_mapping_pattern_benchmark_0_lft_5_iteration_1.xml"));
		//dataSet = (rtp.run("D:\\Bristol_Research\\VPP_Project\\VPPNoRC\\VPPNoRC_VisualStudio\\Chou_and_Marculescu\\OutPutPattern_10.xml"));
		IEditorInput input = new NoRCMappedTaskPatternEditorInput(dataSet);
		
		try {
			page.openEditor(input, NoRCMappedTaskPatternEditor.ID, true);
		} catch (PartInitException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

/*		try {
			page.showView(TaskView.ID);
		} catch (PartInitException e) {
			e.printStackTrace();
		}*/
		return null;
	}

}
