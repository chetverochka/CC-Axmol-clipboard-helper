#pragma once
#include "cocos2d.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#include <Windows.h>
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif

class ClipboardHelper {
public:
    static ClipboardHelper& getInstance() {
        static ClipboardHelper instance;
        return instance;
    }

    std::string getText() {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
        if (!OpenClipboard(nullptr)) return "";
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData == nullptr) {
            CloseClipboard();
            return "";
        }
        char* pszText = static_cast<char*>(GlobalLock(hData));
        if (pszText == nullptr) {
            CloseClipboard();
            return "";
        }
        std::string text(pszText);
        GlobalUnlock(hData);
        CloseClipboard();
        return text;
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
        JniMethodInfo methodInfo;
        if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
                                                    "org/axmol/app/ClipboardHelper",
                                                    "getText",
                                                    "(Landroid/content/Context;)Ljava/lang/String;")) {

            jobject context = cocos2d::JniHelper::getActivity();
            jstring result = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, context);
            const char* str = methodInfo.env->GetStringUTFChars(result, nullptr);
            std::string ret(str);
            methodInfo.env->ReleaseStringUTFChars(result, str);
            methodInfo.env->DeleteLocalRef(result);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            return ret;
        }
        return "";
#endif
        return "";
    }

    void setText(std::string text) {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
        if (!OpenClipboard(nullptr)) return;
        EmptyClipboard();
        HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (!hGlob) {
            CloseClipboard();
            return;
        }
        memcpy(GlobalLock(hGlob), text.c_str(), text.size() + 1);
        GlobalUnlock(hGlob);
        SetClipboardData(CF_TEXT, hGlob);
        CloseClipboard();
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
        JniMethodInfo methodInfo;
        if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
                                                    "org/axmol/app/ClipboardHelper",
                                                    "setText",
                                                    "(Landroid/content/Context;Ljava/lang/String;)V")) {

            jobject context = cocos2d::JniHelper::getActivity();
            jstring jtext = methodInfo.env->NewStringUTF(text.c_str());
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, context, jtext);
            methodInfo.env->DeleteLocalRef(jtext);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
#endif
    }
};
