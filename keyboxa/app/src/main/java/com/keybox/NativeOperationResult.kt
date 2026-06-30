package com.keybox

data class NativeOperationResult(
    val resultCode: Int,
    val message: String
) {
    val isSuccess: Boolean
        get() = resultCode == 0
}
