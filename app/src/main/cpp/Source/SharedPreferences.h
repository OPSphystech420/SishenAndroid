//
// Created by Hai Duong on 3/13/2024.
//

#ifndef ANDROID_MOD_MENU_3_2_SHAREDPREFERENCES_H
#define ANDROID_MOD_MENU_3_2_SHAREDPREFERENCES_H
#include <jni.h>
#include <android/log.h>
#include <string>

class SharedPreferences {
public:
    SharedPreferences(SharedPreferences const &) = delete;
    void operator=(SharedPreferences const &)= delete;
public:
    //Note: Per default, this doesn't keep the reference to the sharedPreferences java object alive
    //longer than the lifetime of the JNIEnv.
    //With keepReference=true the joSharedPreferences is kept 'alive' and you can still use the class after the original JNIEnv* has become invalid -
    //but make sure to refresh the JNIEnv* object with a new valid reference via replaceJNI()
    SharedPreferences(JNIEnv *env, jobject androidContext,const char* name,const bool keepReference=false){
        this->env=env;
        //Find the 2 java classes we need to make calls with
        jclass jcContext = env->FindClass("android/content/Context");
        jclass jcSharedPreferences = env->FindClass("android/content/SharedPreferences");
        //jclass jcSharedPreferences_Editor=env->FindClass("android/content/SharedPreferences$Editor");
        if(jcContext==nullptr || jcSharedPreferences== nullptr){
            __android_log_print(ANDROID_LOG_DEBUG, "SharedPreferences","Cannot find classes");
        }
        //find the 3 functions we need to get values from an SharedPreferences instance
        jmGetBoolean=env->GetMethodID(jcSharedPreferences,"getBoolean","(Ljava/lang/String;Z)Z");
        jmGetInt=env->GetMethodID(jcSharedPreferences,"getInt","(Ljava/lang/String;I)I");
        jmGetFloat=env->GetMethodID(jcSharedPreferences,"getFloat","(Ljava/lang/String;F)F");
        jmGetString=env->GetMethodID(jcSharedPreferences,"getString","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        //find the 1 function we need to create the SharedPreferences.Editor object
        jmEdit=env->GetMethodID(jcSharedPreferences,"edit", "()Landroid/content/SharedPreferences$Editor;");
        //create a instance of SharedPreferences and store it in @joSharedPreferences
        jmethodID jmGetSharedPreferences=env->GetMethodID(jcContext,"getSharedPreferences","(Ljava/lang/String;I)Landroid/content/SharedPreferences;");
        joSharedPreferences=env->CallObjectMethod(androidContext,jmGetSharedPreferences,env->NewStringUTF(name),MODE_PRIVATE);
        //jmEdit_commit=env->GetMethodID(jcSharedPreferences_Editor,"putString","(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;");
        if(keepReference){
            joSharedPreferences=env->NewWeakGlobalRef(joSharedPreferences);
        }
    }
    void replaceJNI(JNIEnv* newEnv){
        env=newEnv;
    }
private:
    JNIEnv* env;
    jobject joSharedPreferences;
    jmethodID jmGetBoolean;
    jmethodID jmGetInt;
    jmethodID jmGetFloat;
    jmethodID jmGetString;
    jmethodID jmEdit;
public:
    bool getBoolean(const char* id,bool defaultValue=false)const{
        return (bool)(env->CallBooleanMethod(joSharedPreferences,jmGetBoolean,env->NewStringUTF(id),(jboolean)defaultValue));
    }
    int getInt(const char* id,int defaultValue=0)const{
        return (int)(env->CallIntMethod(joSharedPreferences,jmGetInt,env->NewStringUTF(id),(jint)defaultValue));
    }
    float getFloat(const char* id,float defaultValue=0.0f)const{
        return (float)(env->CallFloatMethod(joSharedPreferences,jmGetFloat,env->NewStringUTF(id),(jfloat)defaultValue));
    }
    std::string getString(const char* id,const char* defaultValue="")const{
        auto value=(jstring)(env->CallObjectMethod(joSharedPreferences,jmGetString,env->NewStringUTF(id),env->NewStringUTF(defaultValue)));
        const char* valueP = env->GetStringUTFChars(value, nullptr);
        const std::string ret=std::string(valueP);
        env->ReleaseStringUTFChars(value,valueP);
        //__android_log_print(ANDROID_LOG_DEBUG, "SharedPreferences","%s",ret.c_str());
        return ret;
    }
    SharedPreferences_Editor edit()const{
        //create a instance of SharedPreferences.Editor and store it in @joSharedPreferences_Edit
        jobject joSharedPreferences_Edit=env->CallObjectMethod(joSharedPreferences,jmEdit);
        SharedPreferences_Editor editor(env,joSharedPreferences_Edit);
        return editor;
    }
private:
    static constexpr const int  MODE_PRIVATE = 0; //taken directly from java, assuming this value stays constant in java
};


#endif //ANDROID_MOD_MENU_3_2_SHAREDPREFERENCES_H
