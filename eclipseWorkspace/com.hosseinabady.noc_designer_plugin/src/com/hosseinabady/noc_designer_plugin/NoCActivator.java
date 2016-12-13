package com.hosseinabady.noc_designer_plugin;



import org.eclipse.jface.resource.ImageDescriptor;
import org.osgi.framework.BundleContext;

import org.eclipse.ui.plugin.AbstractUIPlugin;

public class NoCActivator extends AbstractUIPlugin {
	// The plug-in ID
	public static final String PLUGIN_ID = "NoRC_IDE";

	// The shared instance
	private static NoCActivator plugin;
	
	/**
	 * The constructor
	 */
	public NoCActivator() {
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	public void stop(BundleContext context) throws Exception {
		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static NoCActivator getDefault() {
		return plugin;
	}

	/**
	 * Returns an image descriptor for the image file at the given
	 * plug-in relative path
	 *
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path) {
		return imageDescriptorFromPlugin(PLUGIN_ID, path);
	}
}
