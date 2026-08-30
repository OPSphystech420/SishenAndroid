#include "Source/Includes.h"
#include "Source/SharedPreferences_Editor.h"
#include "Source/SharedPreferences.h"
#include "Source/Menu.h"
#include "obfuscate.h"
#include "Source/Icon.h"


void SetDataFont(JNIEnv* env, const char *fontName)
{
    jstring jstr = env->NewStringUTF(fontName);
    jclass cls = env->FindClass("ark/use/sishen/SishenTool");  // try to find the class
    if(cls != nullptr) {
        jmethodID mid = env->GetStaticMethodID(cls, "SetFontData",
                                               "(Ljava/lang/String;)V");  // find method
        if(mid != nullptr)
        {
            env->CallStaticVoidMethod(cls, mid, jstr);
        }
    }
}

void GLES3JNIView_init(JNIEnv *env, jclass clazz /*, jobject surface*/) {

    //SetUpImGuiContext
    if (g_Initialized)
        return;

    //g_NativeWindow = ANativeWindow_fromSurface(env, surface);

    ImGui::CreateContext();
    ImGuiStyle *style = &ImGui::GetStyle();
    style->WindowTitleAlign = ImVec2(0, 0.50);
    style->FrameBorderSize = 1.5;
    style->WindowRounding = 1.0f;
    style->ScrollbarRounding = 0.5;
    style->FramePadding = ImVec2(7, 5);
    style->ScaleAllSizes(3.43f);
    style->ScrollbarSize /= 1;
    style->WindowMinSize = ImVec2(400, 180);

    ImGuiIO *io = &ImGui::GetIO();
    ImGui_ImplAndroid_Init(g_NativeWindow);
    ImGui_ImplOpenGL3_Init("#version 100");

    ImFontConfig font_cfg;

    SetDataFont(env, "tahomabd.ttf");
    jfieldID Id = env->GetStaticFieldID(clazz,"fontData", "[B");
    jbyteArray byteArray = (jbyteArray)env->GetStaticObjectField(clazz,Id);
    jbyte* fontData = env->GetByteArrayElements(byteArray,NULL);
    int fontDataSize= env->GetArrayLength(byteArray);
    //io->Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, 25.0f, &font_cfg, io->Fonts->GetGlyphRangesJapanese());

    /*io->Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, 38.0f, &font_cfg, io->Fonts->GetGlyphRangesChineseFull());*/

    io->Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, 38.0f, &font_cfg, io->Fonts->GetGlyphRangesThai());

    //io->Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, 25.0f, &font_cfg, io->Fonts->GetGlyphRangesGreek());

    //io->Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, 25.0f, &font_cfg, io->Fonts->GetGlyphRangesCyrillic());
    //IM_ASSERT(font != NULL);

    settings.Load();


    g_Initialized = true;
}

void GLES3JNIView_resize(JNIEnv *env, jclass clazz, jint width, jint height) {
    ScreenWidth = (int) width;
    ScreenHeight = (int) height;
    glViewport(0, 0, width, height);
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    //io.IniFilename = NULL;
    ImGui::GetIO().DisplaySize = ImVec2((float)width, (float)height);
}

void Toast(const char *text, int length)
{
    if (global_env!= nullptr)
    {
        jstring jstr = global_env->NewStringUTF(text);
        jclass cls = global_env->FindClass(OBFUSCATE("ark/use/sishen/SishenTool"));  // try to find the class
        if(cls != nullptr) {
            jmethodID mid = global_env->GetStaticMethodID(cls, OBFUSCATE("Toast"),
                                                          OBFUSCATE("(Ljava/lang/String;I)V"));  // find method
            if(mid != nullptr)
            {
                global_env->CallStaticVoidMethod(cls, mid, jstr, length);
            }
        }
    }
}

void CloseMenuImgui(JNIEnv *env)
{
    if (!CanCloseMenu)
        return;

    if (!ShowMenu && env != nullptr)
    {
        jclass cls = env->FindClass("ark/use/sishen/SishenTool");  // try to find the class
        if(cls != nullptr) {
            jmethodID mid = env->GetStaticMethodID(cls, OBFUSCATE("ClosedMenuImgui"),
                                                   OBFUSCATE("()V"));  // find method
            if(mid != nullptr)
            {
                env->CallStaticVoidMethod(cls, mid);
                settings.Save();
                CanCloseMenu = false;
            }
        }
    }
}




void GLES3JNIView_step(JNIEnv *env, jclass clazz)
{
    if (!g_Initialized)
        return;

    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 0.8f;
    global_env = env;
    static bool show_MainMenu_window = true;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(ScreenWidth, ScreenHeight);
    ImGui::NewFrame();


    if (ShowMenu)
    {
        DrawMenu();
        CanCloseMenu = true;
    }
    else
    {
        CloseMenuImgui(env);
    }

    /*if (show_MainMenu_window) {
        // BeginDraw();
        ImGui::Begin("Another Window", &show_MainMenu_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        g_window = ImGui::GetCurrentWindow();
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_MainMenu_window = false;
        ImGui::End();
    }*/

    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void GLES3JNIView_imgui_1Shutdown(JNIEnv *env, jclass clazz)
{
    if (!g_Initialized)
        return;

    settings.Save();

    // shut down everything
    g_Initialized = false;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    //ANativeWindow_release(g_NativeWindow);
}

void GLES3JNIView_MotionEventClick(JNIEnv *env, jclass clazz, jboolean down, jfloat PosX, jfloat PosY)
{
    ImGuiIO & io = ImGui::GetIO();
    io.MouseDown[0] = down;
    io.MousePos = ImVec2(PosX,PosY);
}

jstring GLES3JNIView_getWindowRect(JNIEnv *env, jclass clazz)
{
    char result[256]="0|0|0|0";
    if(g_window){
        sprintf(result,"%d|%d|%d|%d",(int)g_window->Pos.x,(int)g_window->Pos.y,(int)g_window->Size.x,(int)g_window->Size.y);
    }
    return env->NewStringUTF(result);
}

void native_ShowMenuImgui(JNIEnv *env, jclass clazz)
{
    ShowMenu = true;
}
//======================================================================================================================
static jobject getGlobalContext()
{
    jclass activityThread = global_env->FindClass(OBFUSCATE("android/app/ActivityThread"));
    jmethodID currentActivityThread = global_env->GetStaticMethodID(activityThread, OBFUSCATE(
            "currentActivityThread"), OBFUSCATE("()Landroid/app/ActivityThread;"));
    jobject at = global_env->CallStaticObjectMethod(activityThread, currentActivityThread);

    jmethodID getApplication = global_env->GetMethodID(activityThread, OBFUSCATE("getApplication"),
                                                       OBFUSCATE("()Landroid/app/Application;"));
    jobject context = global_env->CallObjectMethod(at, getApplication);
    return context;
}

std::filesystem::path GetInternalStoragePath()
{
    jobject context = getGlobalContext();
    if (!context) return "";

    jclass contextClass = global_env->GetObjectClass(context);
    jmethodID getFilesDir = global_env->GetMethodID(contextClass, "getFilesDir", "()Ljava/io/File;");
    jobject fileObj = global_env->CallObjectMethod(context, getFilesDir);

    jclass fileClass = global_env->GetObjectClass(fileObj);
    jmethodID getPath = global_env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");
    jstring jPath = (jstring)global_env->CallObjectMethod(fileObj, getPath);

    const char* pathChars = global_env->GetStringUTFChars(jPath, nullptr);
    std::string result(pathChars);
    global_env->ReleaseStringUTFChars(jPath, pathChars);

    return std::filesystem::path(result); // This will be like "/data/data/com.your.app/files"
}



int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("ShowMenuImgui"), OBFUSCATE("()V"), reinterpret_cast<void *>(native_ShowMenuImgui)}
    };

    jclass clazz = env->FindClass(OBFUSCATE("ark/use/sishen/SishenTool"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterGLES3JNIView(JNIEnv *env) {
    JNINativeMethod methods[] = {
            //{OBFUSCATE("init"), OBFUSCATE("(Landroid/view/Surface;)V"), reinterpret_cast<void *>(GLES3JNIView_init)},
            {OBFUSCATE("init"), OBFUSCATE("()V"), reinterpret_cast<void *>(GLES3JNIView_init)},
            {OBFUSCATE("resize"), OBFUSCATE("(II)V"), reinterpret_cast<void *>(GLES3JNIView_resize)},
            {OBFUSCATE("step"), OBFUSCATE("()V"), reinterpret_cast<void *>(GLES3JNIView_step)},
            {OBFUSCATE("imgui_Shutdown"), OBFUSCATE("()V"), reinterpret_cast<void *>(GLES3JNIView_imgui_1Shutdown)},
            {OBFUSCATE("MotionEventClick"), OBFUSCATE("(ZFF)V"), reinterpret_cast<void *>(GLES3JNIView_MotionEventClick)},
            {OBFUSCATE("getWindowRect"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(GLES3JNIView_getWindowRect)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("ark/use/sishen/GLES3JNIView"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
    vm->GetEnv((void **) &global_env, JNI_VERSION_1_6);

    if (RegisterMenu(global_env) != 0)
        return JNI_ERR;

    if (RegisterGLES3JNIView(global_env) != 0)
        return JNI_ERR;

    Settings::FilePath = GetInternalStoragePath() / "Settings.txt";

    //setupConfigFile();
    return JNI_VERSION_1_6;
}