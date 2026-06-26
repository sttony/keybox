package com.keybox.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import com.keybox.MainActivity
import com.keybox.R
import com.keybox.databinding.FragmentNewClientSetupBinding
import java.net.HttpURLConnection
import java.net.URL
import javax.crypto.Cipher
import javax.crypto.spec.IvParameterSpec
import javax.crypto.spec.SecretKeySpec

class NewClientSetupFragment : Fragment() {

    private var _binding: FragmentNewClientSetupBinding? = null
    private val binding get() = _binding!!
    private var encryptedUrlData: ByteArray = ByteArray(0)
    private var downloadedKeybox: ByteArray? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        encryptedUrlData = requireArguments().getByteArray(ARG_ENCRYPTED_URL) ?: ByteArray(0)
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentNewClientSetupBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (requireActivity() as AppCompatActivity).supportActionBar?.title = getString(R.string.setup_new_client)

        binding.buttonFinalizeSetup.setOnClickListener { processSetupCode() }
        binding.buttonUnlockLoad.setOnClickListener { unlockAndLoad() }
        binding.buttonCancel.setOnClickListener { parentFragmentManager.popBackStack() }
        showPasswordPrompt(false)
    }

    private fun processSetupCode() {
        val rawCode = binding.editSetupCode.text?.toString().orEmpty()
            .replace(" ", "")
            .replace("-", "")
            .replace("\n", "")
            .replace("\t", "")

        val codeData = hexToBytes(rawCode)
        if (codeData == null || codeData.size != SETUP_CODE_BYTES) {
            showError(getString(R.string.invalid_setup_code))
            return
        }

        setProcessing(true)
        showError(null)
        Thread {
            runCatching {
                val key = codeData.copyOfRange(0, AES_KEY_BYTES)
                val iv = codeData.copyOfRange(AES_KEY_BYTES, SETUP_CODE_BYTES)
                val decryptedUrl = decryptUrl(encryptedUrlData, key, iv)
                downloadKeybox(decryptedUrl)
            }.onSuccess { data ->
                activity?.runOnUiThread {
                    downloadedKeybox = data
                    setProcessing(false)
                    showPasswordPrompt(true)
                }
            }.onFailure { error ->
                activity?.runOnUiThread {
                    setProcessing(false)
                    showError(getString(R.string.setup_new_client_failed, error.localizedMessage ?: "unknown error"))
                }
            }
        }.start()
    }

    private fun unlockAndLoad() {
        val data = downloadedKeybox ?: return
        val password = binding.editMasterPassword.text?.toString().orEmpty()
        if (password.isEmpty()) {
            binding.layoutMasterPassword.error = getString(R.string.password_required)
            return
        }

        val success = (requireActivity() as MainActivity).loadDownloadedKeybox(data, password)
        if (success) {
            parentFragmentManager.popBackStack()
        } else {
            showError(getString(R.string.downloaded_keybox_unlock_failed))
            showPasswordPrompt(false)
        }
    }

    private fun decryptUrl(encryptedData: ByteArray, key: ByteArray, iv: ByteArray): String {
        val cipher = Cipher.getInstance("AES/CBC/PKCS5Padding")
        cipher.init(Cipher.DECRYPT_MODE, SecretKeySpec(key, "AES"), IvParameterSpec(iv))
        val decrypted = cipher.doFinal(encryptedData)
        return decrypted.toString(Charsets.UTF_8).trim { it <= ' ' }
    }

    private fun downloadKeybox(urlString: String): ByteArray {
        val connection = (URL(urlString).openConnection() as HttpURLConnection).apply {
            requestMethod = "GET"
            connectTimeout = NETWORK_TIMEOUT_MS
            readTimeout = NETWORK_TIMEOUT_MS
        }
        return try {
            if (connection.responseCode != HttpURLConnection.HTTP_OK) {
                error("HTTP ${connection.responseCode}")
            }
            connection.inputStream.use { it.readBytes() }
        } finally {
            connection.disconnect()
        }
    }

    private fun setProcessing(processing: Boolean) {
        binding.progressSetup.visibility = if (processing) View.VISIBLE else View.GONE
        binding.buttonFinalizeSetup.isEnabled = !processing
        binding.editSetupCode.isEnabled = !processing
    }

    private fun showPasswordPrompt(show: Boolean) {
        binding.groupPassword.visibility = if (show) View.VISIBLE else View.GONE
        binding.buttonFinalizeSetup.visibility = if (show) View.GONE else View.VISIBLE
        binding.editSetupCode.isEnabled = !show
    }

    private fun showError(message: String?) {
        binding.textError.text = message.orEmpty()
        binding.textError.visibility = if (message.isNullOrBlank()) View.GONE else View.VISIBLE
        binding.layoutMasterPassword.error = null
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private fun hexToBytes(hex: String): ByteArray? {
        if (hex.length % 2 != 0 || !hex.all { it in '0'..'9' || it in 'a'..'f' || it in 'A'..'F' }) {
            return null
        }
        return ByteArray(hex.length / 2) { index ->
            hex.substring(index * 2, index * 2 + 2).toInt(16).toByte()
        }
    }

    companion object {
        private const val ARG_ENCRYPTED_URL = "encrypted_url"
        private const val AES_KEY_BYTES = 32
        private const val SETUP_CODE_BYTES = 48
        private const val NETWORK_TIMEOUT_MS = 30000

        fun newInstance(encryptedUrl: ByteArray): NewClientSetupFragment {
            return NewClientSetupFragment().apply {
                arguments = Bundle().apply { putByteArray(ARG_ENCRYPTED_URL, encryptedUrl) }
            }
        }
    }
}
