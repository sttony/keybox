#include <jni.h>
#include <string>
#include <vector>
#include "CKBFile.h"
#include <android/log.h>

#define LOG_TAG "KeyboxNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"
JNIEXPORT jlong JNICALL
Java_com_keybox_NativeBridge_createCKBFile(JNIEnv *env, jobject thiz) {
    return reinterpret_cast<jlong>(new CKBFile());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_keybox_NativeBridge_destroyCKBFile(JNIEnv *env, jobject thiz, jlong handle) {
    delete reinterpret_cast<CKBFile *>(handle);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_deserialize(JNIEnv *env, jobject thiz, jlong handle, jbyteArray data) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    jbyte *bufferPtr = env->GetByteArrayElements(data, nullptr);
    jsize length = env->GetArrayLength(data);

    uint32_t realSize = 0;
    uint32_t result = file->Deserialize(reinterpret_cast<const unsigned char *>(bufferPtr), length, realSize);

    env->ReleaseByteArrayElements(data, bufferPtr, JNI_ABORT);
    return static_cast<jint>(result);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_keybox_NativeBridge_getEntries(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    const auto &entries = file->GetEntries();

    jclass entryClass = env->FindClass("com/keybox/Entry");
    if (entryClass == nullptr) return nullptr;

    jmethodID entryConstructor = env->GetMethodID(entryClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    jobjectArray array = env->NewObjectArray(entries.size(), entryClass, nullptr);

    for (size_t i = 0; i < entries.size(); ++i) {
        jstring title = env->NewStringUTF(entries[i].GetTitle().c_str());
        jstring username = env->NewStringUTF(entries[i].GetUserName().c_str());
        jstring password = env->NewStringUTF(entries[i].GetPassword().c_str());

        jobject entryObj = env->NewObject(entryClass, entryConstructor, title, username, password);
        env->SetObjectArrayElement(array, i, entryObj);

        env->DeleteLocalRef(title);
        env->DeleteLocalRef(username);
        env->DeleteLocalRef(password);
        env->DeleteLocalRef(entryObj);
    }

    return array;
}
