package com.hosseinabady.norc_designer_plugin.editors;


import java.util.ArrayList;
import org.eclipse.swt.graphics.RGB;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;

import com.hosseinabady.norc_designer_plugin.data.NoRCMappedTaskPatternRecord;

public class NoRCPowerPatternEditor extends EditorPart {

	public static final String ID = "NoRC_Designer_Plugin.editors.PowerPatternEditor";
	
	public NoRCPowerPatternEditor() {
		// TODO Auto-generated constructor stub
		super();
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		// TODO Auto-generated method stub

	}

	@Override
	public void doSaveAs() {
		// TODO Auto-generated method stub

	}

	@Override
	public void init(IEditorSite site, IEditorInput input)
			throws PartInitException {
		// TODO Auto-generated method stub
		setSite(site);
        setInput(input);
	}

	@Override
	public boolean isDirty() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void createPartControl(Composite parent) {
		// TODO Auto-generated method stub
		ArrayList<Label> leftCore = new ArrayList<Label>();
		ArrayList<Label> rightCore = new ArrayList<Label>();

		int h = 60;
		int w = 30;
		int distance = 10;
	
		Group group1 = new Group(parent, SWT.BORDER | SWT.SCROLL_PAGE);
		group1.setBounds(300,300,parent.getSize().x, parent.getSize().y);
		group1.setText("NoRC Task Pattern");
		
		int MAX_ROW_SIZE = ((NoRCPowerPatternEditorInput)getEditorInput()).getDataSet().getRowSize();
		int MAX_COL_SIZE = ((NoRCPowerPatternEditorInput)getEditorInput()).getDataSet().getColumnSize();
		int iOffset = 50;
		int jOffset = 50;
		
		int[] ids       = ((NoRCPowerPatternEditorInput)getEditorInput()).getDataSet().getIds();
		double[] powers = ((NoRCPowerPatternEditorInput)getEditorInput()).getDataSet().getPowers();
		
		double maxColor     = powers[0];
		double minColor     = powers[0];
		double averageColor = 0.0;
		
		for (int i = 0; i < powers.length; i++ ) {
			if (maxColor < powers[i])
				maxColor = powers[i];
			if (minColor > powers[i])
				minColor = powers[i];
			averageColor = averageColor + powers[i];
		}
		int r, g, b;
		averageColor=averageColor/powers.length;
		double offsetColor = 0;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) {
				leftCore.add(new Label(group1, SWT.CENTER | SWT.BORDER | SWT.WRAP | SWT.BOLD));
				rightCore.add(new Label(group1, SWT.CENTER | SWT.BORDER | SWT.WRAP| SWT.BOLD));
				int index = i*MAX_COL_SIZE+j;
				int redColor;
				redColor = (int)(((minColor-minColor)/(maxColor-minColor))*(255.0-offsetColor) + offsetColor );
				for (int k = 0; k < ids.length; k++){
					if (index == ids[k]){
						redColor = (int)(((powers[k]-minColor)/(maxColor-minColor))*(255.0-offsetColor) + offsetColor );
						break;
					}
				}
				r = redColor;
				g = 255 - redColor;
				b = 255 - redColor;
				System.out.println("index = "+index+"r = "+r+" g = "+g+" b = "+b);
				leftCore.get(index).setSize(w,h);
				leftCore.get(index).setLocation(jOffset+j*(2*w+distance), iOffset+i*(h+distance));
				leftCore.get(index).setBackground(new Color(parent.getDisplay(), new RGB(r, g, b)));
				leftCore.get(index).setText("" + (index+1));
				rightCore.get(index).setSize(w,h);
				rightCore.get(index).setLocation(jOffset+j*(2*w+distance)+w, iOffset+i*(h+distance));
				rightCore.get(index).setBackground(new Color(parent.getDisplay(), new RGB(r, g, b)));
				rightCore.get(index).setText("" + (index+1));
		}
		
	}

	@Override
	public void setFocus() {
		// TODO Auto-generated method stub

	}

}
