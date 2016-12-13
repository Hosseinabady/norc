package com.hosseinabady.norc_designer_plugin.handlers;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.eclipse.cdt.core.CCProjectNature;
import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.WriteAccessException;
import org.eclipse.cdt.core.settings.model.extension.CConfigurationData;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IProjectType;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.managedbuilder.internal.core.Configuration;
import org.eclipse.cdt.managedbuilder.internal.core.ManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.internal.core.ManagedProject;
import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceDescription;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.IWorkbenchWindow;

public class SystemCCodeGenerationHandler extends AbstractHandler  {

	private IWorkbenchWindow window;
	
	private static final String[] folder_names = {	"NoRC"
													, "NoRC/include"
													, "NoRC/protocol_data_units"
													, "NoRC/protocol_data_units/include"
													, "NoRC/router"
													, "NoRC/router/include"
													, "NoRC/router/src"
													, "NoRC/src"
													, "NoRC/tile"
													, "NoRC/tile/cores"
													, "NoRC/tile/cores/include"
													, "NoRC/tile/cores/src"
													, "NoRC/tile/include"
													, "NoRC/tile/network_interface"
													, "NoRC/tile/network_interface/include"
													, "NoRC/tile/network_interface/src"
													, "NoRC/tile/src"
													, "NoRC/utilities"
													, "NoRC/utilities/include"
													, "NoRC/utilities/src"
													, "SoC"
													, "SoC/include"
													, "SoC/src"
													, "model"
													, "model/include"
													, "model/modules"
													, "model/modules/include"
													, "model/protocol_data_unit"
													, "model/protocol_data_unit/include"
													, "model/tasks"
													, "model/tasks/include"
													, "offchip"
													, "offchip/include"
													, "offchip/src"
													, "tasks"
													, "tasks/applications"
													, "tasks/src"
													, "tasks/include"
												};

	private static final String[] file_names = {	 "NoRC/norc.h"
													, "NoRC/include/norc.h"
													, "NoRC/protocol_data_units/include/noc_flit_payload.h"
													, "NoRC/protocol_data_units/include/noc_message_payload.h"
													, "NoRC/protocol_data_units/include/noc_packet_payload.h"
													, "NoRC/protocol_data_units/include/noc_payload.h"
													, "NoRC/protocol_data_units/include/noc_phit_payload.h"
													, "NoRC/protocol_data_units/include/noc_resource_map.h"
													, "NoRC/router/include/router.h"
													, "NoRC/src/norc.cpp"
													, "NoRC/tile/cores/include/dummy_core.h"
													, "NoRC/tile/cores/include/rtr_core.h"
													, "NoRC/tile/cores/src/dummy_core.cpp"
													, "NoRC/tile/include/dummy_tile.h"
													, "NoRC/tile/include/rtr_tile.h"
													, "NoRC/tile/network_interface/include/interface.h"
													, "NoRC/tile/network_interface/include/network_layer.h"
													, "NoRC/tile/network_interface/include/physical_layer.h"
													, "NoRC/tile/network_interface/include/tile_interface.h"
													, "NoRC/tile/network_interface/include/transport_layer.h"
													, "NoRC/utilities/include/utilities.h"
													, "NoRC/utilities/src/utilities.cpp"
													, "SoC/src/soc.cpp"
													, "model/model.h"
													, "model/include/modelutilities.h"
													, "model/modules/include/noc_abstract_module.h"
													, "model/modules/include/noc_ambaahb_bus.h"
													, "model/modules/include/noc_connector.h"
													, "model/modules/include/noc_mem.h"
													, "model/modules/include/noc_mm.h"
													, "model/modules/include/noc_ni_manager.h"
													, "model/modules/include/noc_processing_element.h"
													, "model/modules/include/noc_protocol_layer.h"
													, "model/modules/include/noc_router_manager.h"
													, "model/modules/include/noc_rtr_core.h"
													, "model/modules/include/noc_tile.h"
													, "model/protocol_data_unit/include/noc_generic_mm.h"
													, "model/protocol_data_unit/include/noc_generic_payload.h"
													, "model/protocol_data_unit/include/noc_generic_payload_inc.h"
													, "model/protocol_data_unit/include/noc_tlm_extension_payload.h"
													, "model/tasks/include/noc_task.h"
													, "offchip/include/application_injector.h"
													, "tasks/applications/app1.app"
													, "tasks/applications/applications.app"
													, "tasks/applications/jpeg.app"
													, "tasks/include"
													, "tasks/include/blocking.h"
													, "tasks/include/converter.h"
													, "tasks/include/downsampler.h"
													, "tasks/include/fdct.h"
													, "tasks/include/huffman.h"
													, "tasks/include/imagewrite.h"
													, "tasks/include/jpegutil.h"
													, "tasks/include/quantizer.h"
													, "tasks/include/readimage.h"
													, "tasks/include/taska.h"
													, "tasks/include/taskb.h"
													, "tasks/include/taskc.h"
													, "tasks/include/taskmanager.h"
													, "tasks/include/zigzag.h"
													, "tasks/src/blocking.cpp"
													, "tasks/src/converter.cpp"
													, "tasks/src/downsampler.cpp"
													, "tasks/src/fdct.cpp"
													, "tasks/src/huffman.cpp"
													, "tasks/src/imagewrite.cpp"
													, "tasks/src/jpegutil.cpp"
													, "tasks/src/quantizer.cpp"
													, "tasks/src/readimage.cpp"
													, "tasks/src/taska.cpp"
													, "tasks/src/taskb.cpp"
													, "tasks/src/taskc.cpp"
													, "tasks/src/taskmanager.cpp"
													, "tasks/src/zigzag.cpp"
													, "Image1.ppm"
													, "Image2.ppm"
													}; 
													
	

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		// TODO Auto-generated method stub
		String pluginProjectTypeId        = "cdt.managedbuild.target.gnu.cygwin.exe";
		ICProjectDescription des          = null;
		ICConfigurationDescription cfgDes = null;
		
		
		IWorkspace workspace = ResourcesPlugin.getWorkspace();
		IWorkspaceRoot root  = workspace.getRoot();
		CoreModel coreModel  = CoreModel.getDefault();
		
		String name = promptForValue("Enter Project Name:", null);
		final IProject newProjectHandle = root.getProject(name);
		IProject project = null;
		
		
		IWorkspaceDescription workspaceDesc = workspace.getDescription();
		workspaceDesc.setAutoBuilding(false);
		try {
			workspace.setDescription(workspaceDesc);
		} catch (CoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		IProjectDescription description = workspace.newProjectDescription(newProjectHandle.getName());
		try {
			project = CCorePlugin.getDefault().createCDTProject(description, newProjectHandle, new NullProgressMonitor());
			//project = BuildSystemTestHelper.createProject(newProjectHandle.getName());
			
		
		} catch (OperationCanceledException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		try {
			if (!project.getProject().hasNature(CCProjectNature.CC_NATURE_ID)) {
//				try {
					//CProjectHelper.addNatureToProject(project.getProject(), CCProjectNature.CC_NATURE_ID, null);
					IProjectDescription idescription = project.getProject().getDescription();
					String[] prevNatures = idescription.getNatureIds();
					String[] newNatures = new String[prevNatures.length + 1];
					System.arraycopy(prevNatures, 0, newNatures, 0, prevNatures.length);
					newNatures[prevNatures.length] = CCProjectNature.CC_NATURE_ID;
					idescription.setNatureIds(newNatures);
					project.getProject().setDescription(idescription, null);

			}
		} catch (CoreException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		
		//Inser Toolchain
 		try {
			des = coreModel.createProjectDescription(project, false);
		} catch (CoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		{
			ManagedBuildInfo info = ManagedBuildManager.createBuildInfo(project);
			IProjectType type = ManagedBuildManager.getProjectType(pluginProjectTypeId);

			ManagedProject mProj = new ManagedProject(project, type);
			info.setManagedProject(mProj);

			
			IConfiguration cfgs[] = type.getConfigurations();

			
			for (IConfiguration configuration : cfgs) {
				String id = ManagedBuildManager.calculateChildId(configuration.getId(), null);
				Configuration config = new Configuration(mProj, (Configuration)configuration, id, false, true, false);
				
				CConfigurationData data = config.getConfigurationData();

				try {
					cfgDes = des.createConfiguration(ManagedBuildManager.CFG_DATA_PROVIDER_ID, data);
				} catch (WriteAccessException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (CoreException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}

		}

		// Persist the project
		try {
			coreModel.setProjectDescription(project, des);
		} catch (CoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		IFolder folder = null;
		for (String folder_name : folder_names) {
			folder= project.getFolder(folder_name);
			try {
				folder.create(true, false, new NullProgressMonitor());
			} catch (CoreException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		IFile file = null;
		for (String file_name : file_names) {
			file= project.getFile(file_name);
			try {
				file.create(new InputStream() {
					@Override
					public int read() throws IOException {
						return -1;
					}}, true, new NullProgressMonitor());
			} catch (CoreException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			String infile_name = "D:/Bristol_Research/NoRC_Designer_Plugin/code_templates/"+file_name;
			String outfile_name = file.getLocation().toString();
			File f1 = new File(infile_name);
		    File f2 = new File(outfile_name);
		    InputStream in;
			try {
				in = new FileInputStream(f1);
			
				OutputStream out = new FileOutputStream(f2);

				byte[] buf = new byte[1024];
				int len;
				while ((len = in.read(buf)) > 0){
					out.write(buf, 0, len);
				}
				in.close();
				out.close();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		
		MessageDialog.openInformation(
				window.getShell(),
				"NoRC_Designer_Plugin Plug-in",
				"NoRC Designer, University of Bristol");
		return null;
	}

	private String promptForValue(String text, String oldValue) {
		InputDialog dlg = new InputDialog(null, 
			"List View", text, oldValue, null);
		if (dlg.open() != Window.OK)
			return null;
		return dlg.getValue();
	}

}
