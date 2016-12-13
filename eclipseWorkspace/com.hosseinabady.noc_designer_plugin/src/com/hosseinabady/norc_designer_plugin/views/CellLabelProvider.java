package com.hosseinabady.norc_designer_plugin.views;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

public class CellLabelProvider extends LabelProvider implements
		ITableLabelProvider {

	@Override
	public Image getColumnImage(Object element, int columnIndex) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getColumnText(Object element, int columnIndex) {
		// TODO Auto-generated method stub
		Cell cell = (Cell)element;
		switch(columnIndex) {
		case 0:
			return cell.getCellAddress();
		case 1:
			return cell.getCellContent();
		default:
			throw new RuntimeException("Should not happen");
		}
	}

}
