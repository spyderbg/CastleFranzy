
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;



public class GLXPMiddlet extends MIDlet
{
	public static Game s_game;
	public static GLXPMiddlet s_midlet;
	
	public GLXPMiddlet()
	{
		s_midlet = this;
	}
	public void startApp()
	{
		if (null == s_game)
		{
			s_game = new Game();
		}
	String name=getAppProperty("url");
	if(name!=null)
	System.out.println(name);
		Display.getDisplay(this).setCurrent(s_game);
	}
	
	public void destroyApp(boolean unconditional)
	{
		notifyDestroyed();
	}
	
	public void pauseApp()
	{
		notifyPaused();
	}
}
