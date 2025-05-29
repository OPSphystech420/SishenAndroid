package ark.use.sishen;

import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.content.Context;
import android.graphics.PixelFormat;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

public class GLES3JNIView extends GLSurfaceView implements GLSurfaceView.Renderer {
    public static byte fontData[];
    private GiveKey giveKey; // add GiveKey instance

    public GLES3JNIView(Context context) {
        super(context);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setEGLContextClientVersion(3);
        setRenderer(this);

        // GiveKey instance
        giveKey = new GiveKey(context);

        giveKey.genAndSendKeyToCpp();
        giveKey.fetchUrlFromCpp();
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        step();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES30.glViewport(0, 0, width, height); // am adugat aceasta linie
        resize(width, height);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        init(/*getHolder().getSurface()*/);
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        imgui_Shutdown();
    }

    public static native void init(/*Surface surface*/);
    public static native void resize(int width, int height);
    public static native void step();
    public static native void imgui_Shutdown();
    public static native void MotionEventClick(boolean down,float PosX,float PosY);
    public static native String getWindowRect();
}
