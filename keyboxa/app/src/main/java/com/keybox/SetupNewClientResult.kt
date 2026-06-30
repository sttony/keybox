package com.keybox

data class SetupNewClientResult(
    val resultCode: Int,
    val message: String,
    val encryptedUrl: ByteArray?
) {
    val isSuccess: Boolean
        get() = resultCode == 0
}
