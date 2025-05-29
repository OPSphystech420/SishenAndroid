//Please don't replace listeners with lambda!

package ark.use.sishen;

import android.annotation.SuppressLint;
import android.app.Activity;

import android.content.Context;

import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;

import android.os.Build;
import android.os.Bundle;
import android.os.Handler;

import android.provider.Settings;
import android.util.Base64;

import android.util.TypedValue;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;

import android.widget.FrameLayout;
import android.widget.ImageView;

import android.widget.RelativeLayout;
import android.widget.Toast;


import static android.view.ViewGroup.LayoutParams.WRAP_CONTENT;

import java.io.InputStream;

public class SishenTool extends Activity
{

    static {
        System.loadLibrary("Sishen");
    }

    static int POS_X = 0;
    static int POS_Y = 100;

    static int ICON_SIZE = 50; //Change both width and height of image
    static float ICON_ALPHA = 0.7f; //Transparent

    static RelativeLayout mCollapsed;
    static RelativeLayout mRootContainer;
    static WindowManager mWindowManager;
    static WindowManager.LayoutParams vmParams;
    static ImageView startimage;
    static FrameLayout rootFrame;
    static Context getContext;

    static long timeHideIcon = 1000;
    static long timeKillIcon = 5000;

    static boolean ShowingMenu = false;

    static native String Icon();

    public static native void ShowMenuImgui();

    public static native void SaveConfig();

    private GiveKey giveKey;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        GiveKey giveKey = new GiveKey(this);
        giveKey.genAndSendKeyToCpp();
        giveKey.fetchUrlFromCpp();
        if (!Settings.canDrawOverlays(this)) {
            //startActivityForResult(new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName())), 0);
        }
        //Start(this);
        StartWithIcon(this);
    }
    public GiveKey getGiveKey() {
        return giveKey;
    }

    public static void StartWithIcon(Context context) {
        if (context instanceof Activity) {
            //Check if context is an Activity.
            Menu(context);
            SetWindowManagerActivity();
        }
    }
    @SuppressLint("SuspiciousIndentation")
    public static void ClosedMenuImgui()
    {
        if (!ShowingMenu) return;
        ((Activity)getContext).runOnUiThread(new Runnable()
        {
            public void run()
            {
                //Log.e("GOD:","Close");
                ShowingMenu = false;
                mCollapsed.setVisibility(View.VISIBLE);
                vParams.width = 0;
                vParams.height = 0;
                manager.updateViewLayout(vTouch, vParams);
            }
        });
    }


    //Here we write the code for our Menu
    // Reference: https://www.androidhive.info/2016/11/android-floating-widget-like-facebook-chat-head/
    @SuppressLint("ClickableViewAccessibility")
    public static void Menu(Context context) {
        getContext = context;
        rootFrame = new FrameLayout(context); // Global markup
        rootFrame.setOnTouchListener(onTouchListener());
        mRootContainer = new RelativeLayout(context); // Markup on which two markups of the icon and the menu itself will be placed
        mCollapsed = new RelativeLayout(context); // Markup of the icon (when the menu is minimized)
        mCollapsed.setVisibility(View.VISIBLE);
        mCollapsed.setAlpha(ICON_ALPHA);

        //********** The icon to open mod menu **********
        startimage = new ImageView(context);
        startimage.setLayoutParams(new RelativeLayout.LayoutParams(WRAP_CONTENT, WRAP_CONTENT));
        int applyDimension = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, ICON_SIZE, context.getResources().getDisplayMetrics()); //Icon size
        startimage.getLayoutParams().height = applyDimension;
        startimage.getLayoutParams().width = applyDimension;
        //startimage.requestLayout();
        startimage.setScaleType(ImageView.ScaleType.FIT_XY);
        byte[] decode = Base64.decode(Icon(), 0);
        startimage.setImageBitmap(BitmapFactory.decodeByteArray(decode, 0, decode.length));
        ((ViewGroup.MarginLayoutParams) startimage.getLayoutParams()).topMargin = convertDipToPixels(10);
        //Initialize event handlers for buttons, etc.
        startimage.setOnTouchListener(onTouchListener());

        //********** Adding view components **********
        mRootContainer.addView(mCollapsed);
        mCollapsed.addView(startimage);
        rootFrame.addView(mRootContainer);
    }


     @SuppressLint("WrongConstant")
    public static void SetWindowManagerActivity() {
        vmParams = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT,
                POS_X,//initialX
                POS_Y,//initialy
                WindowManager.LayoutParams.TYPE_APPLICATION,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_OVERSCAN |
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN |
                        WindowManager.LayoutParams.FLAG_SPLIT_TOUCH,
                PixelFormat.TRANSPARENT
        );
        vmParams.gravity = 51;
        vmParams.x = POS_X;
        vmParams.y = POS_Y;

         mWindowManager = ((Activity) getContext).getWindowManager();
         mWindowManager.addView(rootFrame, vmParams);
//==================================
         //SetFontData(getContext,"OpenSans-Bold.ttf");
         manager = ((Activity) getContext).getWindowManager();
         vParams = getAttributes(false);
         WindowManager.LayoutParams wParams = getAttributes(true);
         GLES3JNIView display = new GLES3JNIView(getContext);
         vTouch = new View(getContext);
         manager.addView(vTouch, vParams);
         manager.addView(display, wParams);

         vTouch.setOnTouchListener(new View.OnTouchListener() {
             @Override
             public boolean onTouch(View v, MotionEvent event) {
                 int action = event.getAction();
                 switch (action) {
                     case MotionEvent.ACTION_MOVE:
                     case MotionEvent.ACTION_DOWN:
                     case MotionEvent.ACTION_UP:
                         //if (ShowingMenu)
                         GLES3JNIView.MotionEventClick(action != MotionEvent.ACTION_UP, event.getRawX(), event.getRawY());
                         break;
                     default:
                         break;
                 }
                 return false;
             }
         });


         updateTouch();
    }

    static void updateTouch()
    {
        final Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                if (ShowingMenu) {
                    try {
                        String rect[] = GLES3JNIView.getWindowRect().split("\\|");
                        vParams.x = Integer.parseInt(rect[0]);
                        vParams.y = Integer.parseInt(rect[1]);
                        vParams.width = Integer.parseInt(rect[2]);
                        vParams.height = Integer.parseInt(rect[3]);
                        //vParams.width = MATCH_PARENT;
                        //vParams.height = MATCH_PARENT;
                        manager.updateViewLayout(vTouch, vParams);
                    } catch (Exception e) {
                    }
                }
                else
                {
                    vParams.width = 0;
                    vParams.height = 0;
                    manager.updateViewLayout(vTouch, vParams);
                }
                handler.postDelayed(this, 20);
            }
        }, 20);
    }

    private static View.OnTouchListener onTouchListener() {
        return new View.OnTouchListener() {

            private long StepTimeDown = 0;
            private long downTime = 0;
            private float initialTouchX, initialTouchY;
            private int initialX, initialY;

            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if (!ShowingMenu) {
                            initialX = vmParams.x;
                            initialY = vmParams.y;
                            initialTouchX = motionEvent.getRawX();
                            initialTouchY = motionEvent.getRawY();

                            downTime = System.currentTimeMillis();
                        }
                        return true;
                    case MotionEvent.ACTION_UP:
                        if (!ShowingMenu) {
                            int rawX = (int) (motionEvent.getRawX() - initialTouchX);
                            int rawY = (int) (motionEvent.getRawY() - initialTouchY);
                            mCollapsed.setAlpha(1f);
                            //The check for Xdiff <10 && YDiff< 10 because sometime elements moves a little while clicking.
                            //So that is click event.
                            if (rawX < 10 && rawY < 10) {
                                //When user clicks on the image view of the collapsed layout,
                                //visibility of the collapsed layout will be changed to "View.GONE"
                                //and expanded view will become visible.
                                StepTimeDown = System.currentTimeMillis() - downTime;
                                if (StepTimeDown < timeHideIcon) {
                                    // user short pressed
                                    try {
                                        mCollapsed.setVisibility(View.GONE);
                                        ShowingMenu = true;
                                        //Log.e("GOD:", "Open");
                                        ShowMenuImgui();
                                        updateTouch();
                                    } catch (NullPointerException e) {

                                    }
                                }
                                else if (StepTimeDown < timeKillIcon) {
                                    // user Hide Icon after Longpress 3s
                                    mCollapsed.setAlpha(0);
                                    Toast.makeText(getContext, "Icon hidden. Remember the hidden icon position", Toast.LENGTH_SHORT).show();
                                }
                                else {
                                    // user Kill Menu after Longpress 10s
                                    mCollapsed.setVisibility(View.GONE);
                                    rootFrame.removeView(mRootContainer);
                                    mWindowManager.removeView(rootFrame);
                                    Toast.makeText(getContext, "Menu killed", Toast.LENGTH_SHORT).show();
                                }
                            }
                        }
                        return true;
                    case MotionEvent.ACTION_MOVE:
                        if (!ShowingMenu) {
                            //mExpanded.setAlpha(0.5f);
                            mCollapsed.setAlpha(0.5f);
                            //Calculate the X and Y coordinates of the view.
                            vmParams.x = initialX + ((int) (motionEvent.getRawX() - initialTouchX));
                            vmParams.y = initialY + ((int) (motionEvent.getRawY() - initialTouchY));
                            //Update the layout with new X & Y coordinate
                            mWindowManager.updateViewLayout(rootFrame, vmParams);
                        }
                        return true;
                    default:
                        return false;
                }
            }
        };
    }

    //For our image a little converter
    private static int convertDipToPixels(int i) {
        return (int) ((((float) i) * getContext.getResources().getDisplayMetrics().density) + 0.5f);
    }


    public void setVisibility(int view) {
        if (rootFrame != null) {
            rootFrame.setVisibility(view);
        }
    }

    public void onDestroy() {
        super.onDestroy();
        if (rootFrame != null) {
            mWindowManager.removeView(rootFrame);
        }
    }

    public static WindowManager manager;
    public static  WindowManager.LayoutParams vParams;

    public static  View vTouch;

    public static WindowManager.LayoutParams getAttributes(boolean isWindow) {
        WindowManager.LayoutParams params = new WindowManager.LayoutParams();
        params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT,
                0,
                100,
                WindowManager.LayoutParams.TYPE_APPLICATION,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_OVERSCAN |
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN |
                        WindowManager.LayoutParams.FLAG_SPLIT_TOUCH |
                        WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE |
                        WindowManager.LayoutParams.FLAG_FULLSCREEN | WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS | WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSPARENT);

        params.flags = WindowManager.LayoutParams.FLAG_FULLSCREEN | WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS | WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;

        if (isWindow) {
            params.flags |= WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE;
        }
        params.format = PixelFormat.RGBA_8888;            // Set the image format to make the background transparent
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            params.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }
        params.gravity = Gravity.LEFT | Gravity.TOP;        // Adjust the docking position of the floating window to the top left
        params.x = params.y = 0;
        params.width = params.height = isWindow ? WindowManager.LayoutParams.MATCH_PARENT : 0;
        return params;
    }

    public static void Toast(final String msg, final int lengh)
    {
        ((Activity)getContext).runOnUiThread(new Runnable()
        {
            public void run()
            {
                Toast.makeText(getContext, msg, lengh).show();
            }
        });
    }

    public void onPause(){
        super.onPause();
        SaveConfig();

    }

    static void SetFontData(String fontname)
    {
        try {
            //InputStream in =getAssets().open("OpenSans-Bold.ttf");
            InputStream in =getContext.getAssets().open(fontname);
            GLES3JNIView.fontData = new byte[in.available()];
            in.read(GLES3JNIView.fontData);
        } catch (Exception e) {}
    }
}
