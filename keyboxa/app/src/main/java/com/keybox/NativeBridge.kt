package com.keybox

class NativeBridge {
    companion object {
        init {
            System.loadLibrary("keyboxa")
        }
    }

    external fun createCKBFile(): Long
    external fun destroyCKBFile(handle: Long)
    external fun setCurlCaBundlePath(path: String): Int
    external fun deserialize(handle: Long, data: ByteArray): Int
    external fun loadHeader(handle: Long, data: ByteArray): Int
    external fun loadPayload(handle: Long, data: ByteArray): Int
    external fun serialize(handle: Long): ByteArray?
    external fun lock(handle: Long): ByteArray?
    external fun getEntries(handle: Long): Array<Entry>?
    external fun getEntryAttachment(handle: Long, entryId: String): ByteArray?
    external fun addEntry(handle: Long, entry: Entry): Int
    external fun updateEntry(handle: Long, entry: Entry): Int
    external fun removeEntry(handle: Long, entryId: String): Int
    external fun setMasterPassword(handle: Long, password: String): Int
    external fun setDerivativeParameters(handle: Long, salt: ByteArray, rounds: Int): Int
    external fun getGroups(handle: Long): Array<Group>?
    external fun removeGroup(handle: Long, groupId: String): Int
    external fun updateGroup(handle: Long, groupId: String, name: String): Int
    external fun retrieveFromRemote(handle: Long): NativeOperationResult
    external fun pushToRemote(handle: Long): NativeOperationResult
    external fun register(handle: Long): NativeOperationResult
    external fun setupNewClient(handle: Long): SetupNewClientResult
    external fun changePassword(handle: Long, password: String): Int
    external fun getSyncUrl(handle: Long): String?
    external fun setSyncUrl(handle: Long, url: String): Int
    external fun getEmail(handle: Long): String?
    external fun setEmail(handle: Long, email: String): Int
    external fun generatePassword(
        length: Int,
        lower: Boolean,
        upper: Boolean,
        digit: Boolean,
        minus: Boolean,
        add: Boolean,
        shift18: Boolean,
        brace: Boolean,
        space: Boolean,
        question: Boolean,
        slash: Boolean,
        greaterLess: Boolean
    ): String?
}
