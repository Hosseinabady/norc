package com.hosseinabady.norc_designer_plugin.views;


import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ColumnViewer;
import org.eclipse.jface.viewers.EditingSupport;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TextCellEditor;

import com.hosseinabady.norc_designer_plugin.device_driver.memory_read_write;

public class CellEditingSupport extends EditingSupport {

	private CellEditor editor;
	private int column;
	private memory_read_write lxc = new memory_read_write();
	
	public CellEditingSupport(ColumnViewer viewer, int column) {
		super(viewer);
		editor = new TextCellEditor(((TableViewer) viewer).getTable());
		this.column = column;
	}
	@Override
	protected boolean canEdit(Object element) {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	protected CellEditor getCellEditor(Object element) {
	
		return editor;
	}

	@Override
	protected Object getValue(Object element) {
		Cell cell = (Cell) element;
		switch(this.column) {
		case 0:
			return cell.getCellAddress();
		case 1: 
			return cell.getCellContent();
		default:
			break;
		}
		return null;
	}

	@Override
	protected void setValue(Object element, Object value) {
		Cell pers = (Cell) element;
		
		String cellValue;
		
		switch (this.column) {
		case 0:
			cellValue = lxc.rw_mem((String)value, pers.getCellContent(), 1);
			pers.setCellAddress(String.valueOf(value));
			pers.setCellContent(String.valueOf(cellValue));
			break;
		case 1:
			pers.setCellContent(String.valueOf(value));
			cellValue = lxc.rw_mem(pers.getCellAddress(), (String)value, 0);
			break;
		default:
			break;
		}
		
		getViewer().update(element, null);
	}

}
