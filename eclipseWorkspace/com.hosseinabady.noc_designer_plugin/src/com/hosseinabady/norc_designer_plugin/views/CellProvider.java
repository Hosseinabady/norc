package com.hosseinabady.norc_designer_plugin.views;

import java.util.ArrayList;
import java.util.List;

public class CellProvider {
	private static CellProvider content;
	private List<Cell> cells;
	
	private CellProvider() {
		cells = new ArrayList<Cell>();
		
		Cell cell;
		cell = new Cell("00000000", "00000001");
		cells.add(cell);
		cell = new Cell("00000004", "ABABBCBC");
		cells.add(cell);
		cell = new Cell("00000008", "ABABBCBC");
		cells.add(cell);
		cell = new Cell("0000000C", "23EF0000");
		cells.add(cell);
		cell = new Cell("00000010", "AAAABBBB");
		cells.add(cell);
		cell = new Cell("00000014", "ABABBCBC");
		cells.add(cell);
	}
	
	public static synchronized CellProvider getInstance() {
		if(content != null) {
			return content;
		}
		
		content = new CellProvider();
		return content;
	}
	
	public List<Cell> getCells() {
		return cells;
	}
}
