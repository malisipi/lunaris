public class Lunaris {
    /* General */
    public native long newWindow();
    public native void windowLoop(long win);
    public native void windowSetDecoration(long win, boolean state);
    public native void windowResize(long win, int width, int height);
    public native void windowSetFullscreen(long win, boolean state);
    public native void windowSetTitle(long win, String title);
    public native int getWindowWidth(long win);
    public native int getWindowHeight(long win);

    @FunctionalInterface
    public interface DrawHandler {
        void draw(long win, int[] buffer);
    }

    public native void windowRegisterDrawHandler(long win, DrawHandler drawHandler);

    /* Graphics */
    public native void windowGraphicsLine(long win, int x1, int y1, int x2, int y2, long color);
    public native void windowGraphicsRect(long win, int x, int y, int w, int h, long color);

    /* Loader */
    static {
        System.loadLibrary("lunaris4java");
    }
}
