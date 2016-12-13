package com.hosseinabady.norc_designer_plugin.handlers;


import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;

import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerDataSet;
import com.hosseinabady.norc_designer_plugin.data.parsers.NoRCRouterTrafficSAXParser;
import com.hosseinabady.norc_designer_plugin.editors.NoRCChartEditor;
import com.hosseinabady.norc_designer_plugin.editors.NoRCChartEditorInput;
import com.hosseinabady.norc_designer_plugin.editors.NoRCPowerPatternEditor;
import com.hosseinabady.norc_designer_plugin.editors.NoRCPowerPatternEditorInput;

public class PowerPatternHandler extends AbstractHandler {

	private int type = 0;
	public Object execute(ExecutionEvent event) throws ExecutionException {
		
		NoRCTrafficPowerDataSet dataSet        = new NoRCTrafficPowerDataSet();
		NoRCRouterTrafficSAXParser rtp         = new NoRCRouterTrafficSAXParser(); 

		dataSet = (rtp.run("D:\\Bristol_Research\\VPP_Project\\VPPNoRC\\log\\task_mapping_pattern_benchmark_4_lft_5_iteration_14.xml"));
		IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		IEditorInput input = new NoRCPowerPatternEditorInput(dataSet, type);
		try {
			
			page.openEditor(input, NoRCPowerPatternEditor.ID, true);
		
		} catch (PartInitException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}


		return null;
	}

}
