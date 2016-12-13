package com.hosseinabady.norc_designer_plugin.device_driver;

public class memory_read_write {
	public native String rw_mem(String address, String rw_dara, int rw_mode);
	
//	public static void main(String args[]) {
//		memory_read_write lxc = new memory_read_write();
//		String input = lxc.rw_mem("00000000", "ABABABAB", 0);
//		System.out.println("Data = " + input);
//	}
	static {
		System.loadLibrary("leon3_xcvsx50t");
	}
}
