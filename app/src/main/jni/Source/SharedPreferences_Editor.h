//
// Created by Hai Duong on 3/13/2024.
//

#ifndef ANDROID_MOD_MENU_3_2_SHAREDPREFERENCES_EDITOR_H
#define ANDROID_MOD_MENU_3_2_SHAREDPREFERENCES_EDITOR_H


#include <jni.h>
#include <android/log.h>
#include <string>

///Example reading values
///SharedPreferences sharedPref(env,context,"pref_telemetry");
///T_Protocol=sharedPref.getInt(IDT::T_Protocol);
///Example writing values
///SharedPreferences_Editor editor=sharedPref.edit();
///editor.putString("MY_KEY","HELLO");
///editor.commit();

class SharedPreferences_Editor{
public:
    SharedPreferences_Editor(JNIEnv* env,const jobject joSharedPreferences_Edit):env(env),joSharedPreferences_Edit(joSharedPreferences_Edit){
        //find the methods for putting values into Shared preferences via the editor
        jclass jcSharedPreferences_Editor = env->GetObjectClass(joSharedPreferences_Edit);
        jmPutBoolean=env->GetMethodID(jcSharedPreferences_Editor,"putBoolean","(Ljava/lang/String;Z)Landroid/content/SharedPreferences$Editor;");
        jmPutInt=env->GetMethodID(jcSharedPreferences_Editor,"putInt","(Ljava/lang/String;I)Landroid/content/SharedPreferences$Editor;");
        jmPutString=env->GetMethodID(jcSharedPreferences_Editor,"putString","(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;");
        jmPutFloat=env->GetMethodID(jcSharedPreferences_Editor,"putFloat","(Ljava/lang/String;F)Landroid/content/SharedPreferences$Editor;");
        jmCommit=env->GetMethodID(jcSharedPreferences_Editor,"commit","()Z");
    }
    //return itself for method chaining
    const SharedPreferences_Editor& putBoolean(const char* key,const bool value)const{
        env->CallObjectMethod(joSharedPreferences_Edit,jmPutBoolean,env->NewStringUTF(key),(jboolean)value);
        return *this;
    }
    const SharedPreferences_Editor& putInt(const char* key,const int value)const{
        env->CallObjectMethod(joSharedPreferences_Edit,jmPutInt,env->NewStringUTF(key),(jint)value);
        return *this;
    }
    const SharedPreferences_Editor& putString(const char* key,const char* value)const{
        env->CallObjectMethod(joSharedPreferences_Edit,jmPutString,env->NewStringUTF(key),env->NewStringUTF(value));
        return *this;
    }

    const SharedPreferences_Editor& putFloat(const char* key,const float value)const{
        env->CallObjectMethod(joSharedPreferences_Edit,jmPutFloat,env->NewStringUTF(key),(jfloat)value);
        return *this;
    }
    bool commit()const{
        return (bool)env->CallBooleanMethod(joSharedPreferences_Edit,jmCommit);
    }
private:
    JNIEnv* env;
    jobject joSharedPreferences_Edit;
    jmethodID jmPutBoolean;
    jmethodID jmPutInt;
    jmethodID jmPutString;
    jmethodID jmPutFloat;
    jmethodID jmCommit;
};


#endif //ANDROID_MOD_MENU_3_2_SHAREDPREFERENCES_EDITOR_H
