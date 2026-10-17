class DEF
{
	//Debug switches
	final static boolean bEmu   = false; // emulator
	final static boolean bErr    = false; // catch errors (exceptions)
	final static boolean bDbgS   = false; // debug out for cSprite.Load()
	final static boolean bDbgO   = true; // generic debug out (System.out.println)
	final static boolean bDbgI   = false; // debug out for images (cached sprites)
	
	final static int FIXED_PRECISION = 8;  // fixed point precision
	
	//screen settings, device == MotoTriplets
	final static int SCREEN_WIDTH = 240;
	final static int SCREEN_HEIGHT = 320;
	public static final int GAMEKEY_QUEUE_SIZE = 10;
	
	static public final int	KEY_RELEASED		= 0x80000000;
	static public final int	KEY_NONE			= 0x000000;
	static public final int	KEY_LEFT			= 0x000001;
	static public final int	KEY_RIGHT			= 0x000002;
	static public final int	KEY_UP				= 0x000004;
	static public final int	KEY_DOWN			= 0x000008;
	static public final int	KEY_A				= 0x000010;
	static public final int	KEY_B				= 0x000020;
	static public final int	KEY_C				= 0x000040;
	static public final int	KEY_0				= 0x000080;
	static public final int	KEY_1				= 0x000100;
	static public final int	KEY_2				= 0x000200;
	static public final int	KEY_3				= 0x000400;
	static public final int	KEY_4				= 0x000800;
	static public final int	KEY_6				= 0x001000;
	static public final int	KEY_7				= 0x002000;
	static public final int	KEY_8				= 0x004000;
	static public final int	KEY_9				= 0x008000;
	static public final int	KEY_SOFT_LEFT		= 0x010000;
	static public final int	KEY_SOFT_RIGHT		= 0x020000;

}
