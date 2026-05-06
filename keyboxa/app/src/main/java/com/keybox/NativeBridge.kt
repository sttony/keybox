package com.keybox

class NativeBridge {
    companion object {
        init {
            System.loadLibrary("keyboxa")
        }
    }

    external fun createCKBFile(): Long
    external fun destroyCKBFile(handle: Long)
    external fun deserialize(handle: Long, data: ByteArray): Int
    external fun getEntries(handle: Long): Array<Entry>?
}
