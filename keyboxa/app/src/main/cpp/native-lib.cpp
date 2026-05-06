#include <jni.h>
#include <string>
#include <vector>
#include "CKBFile.h"
#include "CPwdGroup.h"
#include "InitGlobalRG.h"
#include <android/log.h>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#define LOG_TAG "KeyboxNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"
JNIEXPORT jlong JNICALL
Java_com_keybox_NativeBridge_createCKBFile(JNIEnv *env, jobject thiz) {
    InitGlobalRG();
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
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_loadHeader(JNIEnv *env, jobject thiz, jlong handle, jbyteArray data) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    jbyte *bufferPtr = env->GetByteArrayElements(data, nullptr);
    jsize length = env->GetArrayLength(data);
    uint32_t realSize = 0;
    uint32_t result = file->LoadHeader(reinterpret_cast<const unsigned char *>(bufferPtr), length, realSize);
    env->ReleaseByteArrayElements(data, bufferPtr, JNI_ABORT);
    return static_cast<jint>(result);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_loadPayload(JNIEnv *env, jobject thiz, jlong handle, jbyteArray data) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    jbyte *bufferPtr = env->GetByteArrayElements(data, nullptr);
    jsize length = env->GetArrayLength(data);
    uint32_t realSize = 0;
    uint32_t result = file->LoadPayload(reinterpret_cast<const unsigned char *>(bufferPtr), length, realSize);
    env->ReleaseByteArrayElements(data, bufferPtr, JNI_ABORT);
    return static_cast<jint>(result);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_keybox_NativeBridge_serialize(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    uint32_t realSize = 0;
    file->Serialize(nullptr, 0, realSize);
    if (realSize == 0) return nullptr;

    std::vector<unsigned char> buffer(realSize);
    uint32_t result = file->Serialize(buffer.data(), buffer.size(), realSize);
    if (result != 0) return nullptr;

    jbyteArray array = env->NewByteArray(realSize);
    env->SetByteArrayRegion(array, 0, realSize, reinterpret_cast<const jbyte *>(buffer.data()));
    return array;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_keybox_NativeBridge_lock(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    uint32_t realSize = 0;
    file->Lock(nullptr, 0, realSize);
    if (realSize == 0) return nullptr;

    std::vector<unsigned char> buffer(realSize);
    uint32_t result = file->Lock(buffer.data(), buffer.size(), realSize);
    if (result != 0) return nullptr;

    jbyteArray array = env->NewByteArray(realSize);
    env->SetByteArrayRegion(array, 0, realSize, reinterpret_cast<const jbyte *>(buffer.data()));
    return array;
}

jobject createJavaEntry(JNIEnv *env, const CPwdEntry &entry) {
    jclass entryClass = env->FindClass("com/keybox/Entry");
    jmethodID entryConstructor = env->GetMethodID(entryClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    jstring id = env->NewStringUTF(boost::uuids::to_string(entry.GetID()).c_str());
    jstring title = env->NewStringUTF(entry.GetTitle().c_str());
    jstring username = env->NewStringUTF(entry.GetUserName().c_str());
    jstring url = env->NewStringUTF(entry.GetUrl().c_str());
    // Note: In a real app, we might want to handle passwords more securely than just JNI strings
    jstring password = env->NewStringUTF(const_cast<CPwdEntry&>(entry).GetPassword().c_str());
    jstring note = env->NewStringUTF(const_cast<CPwdEntry&>(entry).GetNote().c_str());
    jstring groupUuid = env->NewStringUTF(boost::uuids::to_string(entry.GetGroup()).c_str());

    jobject entryObj = env->NewObject(entryClass, entryConstructor, id, title, username, url, password, note, groupUuid);

    env->DeleteLocalRef(id);
    env->DeleteLocalRef(title);
    env->DeleteLocalRef(username);
    env->DeleteLocalRef(url);
    env->DeleteLocalRef(password);
    env->DeleteLocalRef(note);
    env->DeleteLocalRef(groupUuid);

    return entryObj;
}

void fillCppEntry(JNIEnv *env, jobject entryObj, CPwdEntry &entry) {
    jclass entryClass = env->GetObjectClass(entryObj);

    auto getStringField = [&](const char* fieldName) -> std::string {
        jfieldID fieldId = env->GetFieldID(entryClass, fieldName, "Ljava/lang/String;");
        jstring jstr = (jstring)env->GetObjectField(entryObj, fieldId);
        if (!jstr) return "";
        const char *chars = env->GetStringUTFChars(jstr, nullptr);
        std::string res(chars);
        env->ReleaseStringUTFChars(jstr, chars);
        return res;
    };

    entry.SetTitle(getStringField("title"));
    entry.SetUserName(getStringField("username"));
    entry.SetUrl(getStringField("url"));
    
    std::string password = getStringField("password");
    entry.SetPassword(password, g_RG.GetNextBytes(32));

    std::string note = getStringField("note");
    entry.SetNote(note, g_RG.GetNextBytes(32));

    std::string groupUuidStr = getStringField("groupUuid");
    if (!groupUuidStr.empty()) {
        try {
            entry.SetGroup(boost::lexical_cast<boost::uuids::uuid>(groupUuidStr));
        } catch (...) {}
    }
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_keybox_NativeBridge_getEntries(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    const auto &entries = file->GetEntries();

    jclass entryClass = env->FindClass("com/keybox/Entry");
    if (entryClass == nullptr) return nullptr;

    jobjectArray array = env->NewObjectArray(entries.size(), entryClass, nullptr);

    for (size_t i = 0; i < entries.size(); ++i) {
        jobject entryObj = createJavaEntry(env, entries[i]);
        env->SetObjectArrayElement(array, i, entryObj);
        env->DeleteLocalRef(entryObj);
    }

    return array;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_addEntry(JNIEnv *env, jobject thiz, jlong handle, jobject entryObj) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    CPwdEntry entry;
    fillCppEntry(env, entryObj, entry);
    return file->AddEntry(entry);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_updateEntry(JNIEnv *env, jobject thiz, jlong handle, jobject entryObj) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    
    jclass entryClass = env->GetObjectClass(entryObj);
    jfieldID idField = env->GetFieldID(entryClass, "id", "Ljava/lang/String;");
    jstring jid = (jstring)env->GetObjectField(entryObj, idField);
    if (!jid) return 1; // Error

    const char *idChars = env->GetStringUTFChars(jid, nullptr);
    boost::uuids::uuid uuid = boost::lexical_cast<boost::uuids::uuid>(idChars);
    env->ReleaseStringUTFChars(jid, idChars);

    CPwdEntry entry(uuid);
    fillCppEntry(env, entryObj, entry);
    return file->SetEntry(entry);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_removeEntry(JNIEnv *env, jobject thiz, jlong handle, jstring entryId) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    const char *idChars = env->GetStringUTFChars(entryId, nullptr);
    boost::uuids::uuid uuid = boost::lexical_cast<boost::uuids::uuid>(idChars);
    env->ReleaseStringUTFChars(entryId, idChars);
    return file->RemoveEntry(uuid);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_setMasterPassword(JNIEnv *env, jobject thiz, jlong handle, jstring password) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    const char *pwdChars = env->GetStringUTFChars(password, nullptr);
    std::string pwd(pwdChars);
    env->ReleaseStringUTFChars(password, pwdChars);

    CMaskedBlob masterKey;
    masterKey.Set(pwd, g_RG.GetNextBytes(32));
    file->SetMasterKey(masterKey);
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_setDerivativeParameters(JNIEnv *env, jobject thiz, jlong handle, jbyteArray salt, jint rounds) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    jbyte *saltPtr = env->GetByteArrayElements(salt, nullptr);
    jsize length = env->GetArrayLength(salt);
    std::vector<unsigned char> vSalt(reinterpret_cast<unsigned char *>(saltPtr), reinterpret_cast<unsigned char *>(saltPtr) + length);
    env->ReleaseByteArrayElements(salt, saltPtr, JNI_ABORT);
    return file->SetDerivativeParameters(vSalt, rounds);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_keybox_NativeBridge_getGroups(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    const auto &groups = file->GetGroups();

    jclass groupClass = env->FindClass("com/keybox/Group");
    jmethodID groupConstructor = env->GetMethodID(groupClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");

    jobjectArray array = env->NewObjectArray(groups.size(), groupClass, nullptr);

    for (size_t i = 0; i < groups.size(); ++i) {
        jstring id = env->NewStringUTF(groups[i].GetID().c_str());
        jstring name = env->NewStringUTF(groups[i].GetName().c_str());
        jobject groupObj = env->NewObject(groupClass, groupConstructor, id, name);
        env->SetObjectArrayElement(array, i, groupObj);
        env->DeleteLocalRef(id);
        env->DeleteLocalRef(name);
        env->DeleteLocalRef(groupObj);
    }

    return array;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_removeGroup(JNIEnv *env, jobject thiz, jlong handle, jstring groupId) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    const char *idChars = env->GetStringUTFChars(groupId, nullptr);
    std::string id(idChars);
    env->ReleaseStringUTFChars(groupId, idChars);
    return file->RemoveGroup(id);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_updateGroup(JNIEnv *env, jobject thiz, jlong handle, jstring groupId, jstring name) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    const char *idChars = env->GetStringUTFChars(groupId, nullptr);
    const char *nameChars = env->GetStringUTFChars(name, nullptr);
    std::string id(idChars);
    std::string n(nameChars);
    env->ReleaseStringUTFChars(groupId, idChars);
    env->ReleaseStringUTFChars(name, nameChars);
    return file->UpdateGroup(id, n);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_retrieveFromRemote(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    std::string message;
    file->RetrieveFromRemote(message);
    return env->NewStringUTF(message.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_pushToRemote(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    std::string message;
    file->PushToRemote(message);
    return env->NewStringUTF(message.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_register(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    std::string message;
    file->Register(message);
    return env->NewStringUTF(message.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_getSyncUrl(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    return env->NewStringUTF(file->GetHeader().GetSyncUrl().c_str());
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_setSyncUrl(JNIEnv *env, jobject thiz, jlong handle, jstring url) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    const char *urlChars = env->GetStringUTFChars(url, nullptr);
    std::string u(urlChars);
    env->ReleaseStringUTFChars(url, urlChars);
    return file->GetHeader().SetSyncUrl(u);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_keybox_NativeBridge_getEmail(JNIEnv *env, jobject thiz, jlong handle) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    return env->NewStringUTF(file->GetHeader().GetSyncEmail().c_str());
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_keybox_NativeBridge_setEmail(JNIEnv *env, jobject thiz, jlong handle, jstring email) {
    auto *file = reinterpret_cast<CKBFile *>(handle);
    const char *emailChars = env->GetStringUTFChars(email, nullptr);
    std::string e(emailChars);
    env->ReleaseStringUTFChars(email, emailChars);
    return file->GetHeader().SetSyncEmail(e);
}
