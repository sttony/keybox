package com.keybox

class KBFile(private val bridge: NativeBridge = NativeBridge()) {
    private var handle: Long = bridge.createCKBFile()

    fun destroy() {
        if (handle != 0L) {
            bridge.destroyCKBFile(handle)
            handle = 0L
        }
    }

    fun deserialize(data: ByteArray): Int = bridge.deserialize(handle, data)
    fun setCurlCaBundlePath(path: String): Int = bridge.setCurlCaBundlePath(path)
    fun loadHeader(data: ByteArray): Int = bridge.loadHeader(handle, data)
    fun loadPayload(data: ByteArray): Int = bridge.loadPayload(handle, data)
    fun serialize(): ByteArray? = bridge.serialize(handle)
    fun lock(): ByteArray? = bridge.lock(handle)

    fun getEntries(): List<Entry> = bridge.getEntries(handle)?.toList() ?: emptyList()
    fun getEntryAttachment(entryId: String): ByteArray? = bridge.getEntryAttachment(handle, entryId)
    fun addEntry(entry: Entry): Int = bridge.addEntry(handle, entry)
    fun updateEntry(entry: Entry): Int = bridge.updateEntry(handle, entry)
    fun removeEntry(entryId: String): Int = bridge.removeEntry(handle, entryId)

    fun setMasterPassword(password: String): Int = bridge.setMasterPassword(handle, password)
    fun setDerivativeParameters(salt: ByteArray, rounds: Int): Int = 
        bridge.setDerivativeParameters(handle, salt, rounds)

    fun getGroups(): List<Group> = bridge.getGroups(handle)?.toList() ?: emptyList()
    fun removeGroup(groupId: String): Int = bridge.removeGroup(handle, groupId)
    fun updateGroup(groupId: String, name: String): Int = bridge.updateGroup(handle, groupId, name)

    fun retrieveFromRemote(): NativeOperationResult = bridge.retrieveFromRemote(handle)
    fun pushToRemote(): NativeOperationResult = bridge.pushToRemote(handle)
    fun register(): NativeOperationResult = bridge.register(handle)
    fun setupNewClient(): SetupNewClientResult = bridge.setupNewClient(handle)
    fun changePassword(password: String): Int = bridge.changePassword(handle, password)

    fun getSyncUrl(): String? = bridge.getSyncUrl(handle)
    fun setSyncUrl(url: String): Int = bridge.setSyncUrl(handle, url)
    fun getEmail(): String? = bridge.getEmail(handle)
    fun setEmail(email: String): Int = bridge.setEmail(handle, email)

    protected fun finalize() {
        destroy()
    }
}
