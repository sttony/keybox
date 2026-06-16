#include <jni.h>
#include <string>

namespace {
std::string g_sync_url;
std::string g_email;

jstring to_jstring(JNIEnv *env, const std::string &value) {
    return env->NewStringUTF(value.c_str());
}
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_keybox_NativeBridge_createCKBFile(JNIEnv *, jobject) {
    return 1;
}

extern "C" JNIEXPORT void JNICALL
Java_com_keybox_NativeBridge_destroyCKBFile(JNIEnv *, jobject, jlong) {}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_deserialize(JNIEnv *, jobject, jlong, jbyteArray) {
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_loadHeader(JNIEnv *, jobject, jlong, jbyteArray) {
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_loadPayload(JNIEnv *, jobject, jlong, jbyteArray) {
    return 0;
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_keybox_NativeBridge_serialize(JNIEnv *env, jobject, jlong) {
    return env->NewByteArray(0);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_keybox_NativeBridge_lock(JNIEnv *env, jobject, jlong) {
    return env->NewByteArray(0);
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_keybox_NativeBridge_getEntries(JNIEnv *, jobject, jlong) {
    return nullptr;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_addEntry(JNIEnv *, jobject, jlong, jobject) {
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_updateEntry(JNIEnv *, jobject, jlong, jobject) {
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_removeEntry(JNIEnv *, jobject, jlong, jstring) {
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_setMasterPassword(JNIEnv *, jobject, jlong, jstring) {
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_setDerivativeParameters(JNIEnv *, jobject, jlong, jbyteArray, jint) {
    return 0;
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_keybox_NativeBridge_getGroups(JNIEnv *, jobject, jlong) {
    return nullptr;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_removeGroup(JNIEnv *, jobject, jlong, jstring) {
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_updateGroup(JNIEnv *, jobject, jlong, jstring, jstring) {
    return 0;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_retrieveFromRemote(JNIEnv *env, jobject, jlong) {
    return to_jstring(env, "Sync backend is not configured in this Android build.");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_pushToRemote(JNIEnv *env, jobject, jlong) {
    return to_jstring(env, "Sync backend is not configured in this Android build.");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_register(JNIEnv *env, jobject, jlong) {
    return to_jstring(env, "Sync backend is not configured in this Android build.");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_getSyncUrl(JNIEnv *env, jobject, jlong) {
    return to_jstring(env, g_sync_url);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_setSyncUrl(JNIEnv *env, jobject, jlong, jstring url) {
    const char *chars = env->GetStringUTFChars(url, nullptr);
    g_sync_url = chars == nullptr ? "" : chars;
    if (chars != nullptr) {
        env->ReleaseStringUTFChars(url, chars);
    }
    return 0;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_getEmail(JNIEnv *env, jobject, jlong) {
    return to_jstring(env, g_email);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_setEmail(JNIEnv *env, jobject, jlong, jstring email) {
    const char *chars = env->GetStringUTFChars(email, nullptr);
    g_email = chars == nullptr ? "" : chars;
    if (chars != nullptr) {
        env->ReleaseStringUTFChars(email, chars);
    }
    return 0;
}
