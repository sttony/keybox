package com.keybox.ui

import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.view.LayoutInflater
import android.view.Menu
import android.view.MenuInflater
import android.view.MenuItem
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.MenuProvider
import androidx.fragment.app.Fragment
import androidx.lifecycle.Lifecycle
import com.keybox.MainActivity
import com.keybox.R
import com.keybox.databinding.FragmentSyncSettingsBinding

class SyncSettingsFragment : Fragment(), MenuProvider {

    private var _binding: FragmentSyncSettingsBinding? = null
    private val binding get() = _binding!!
    private var operationInProgress = false

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentSyncSettingsBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        requireActivity().addMenuProvider(this, viewLifecycleOwner, Lifecycle.State.RESUMED)
        (requireActivity() as AppCompatActivity).supportActionBar?.title =
            getString(R.string.sync_settings)

        loadCurrentSettings()

        val textWatcher = object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable?) { updateButtonStates() }
        }
        binding.editEmail.addTextChangedListener(textWatcher)
        binding.editSyncUrl.addTextChangedListener(textWatcher)

        binding.btnRegisterEmail.setOnClickListener { registerEmail() }
        binding.btnSetupNewClient.setOnClickListener { setupNewClient() }
        binding.btnDeleteAccount.setOnClickListener { confirmDeleteAccount() }

        updateButtonStates()
    }

    override fun onCreateMenu(menu: Menu, menuInflater: MenuInflater) {
        menuInflater.inflate(R.menu.menu_sync_settings, menu)
    }

    override fun onMenuItemSelected(menuItem: MenuItem): Boolean {
        return when (menuItem.itemId) {
            R.id.action_save -> { saveSettings(); true }
            else -> false
        }
    }

    private fun loadCurrentSettings() {
        val kbFile = (requireActivity() as MainActivity).kbFile
        binding.editEmail.setText(kbFile.getEmail() ?: "")
        binding.editSyncUrl.setText(kbFile.getSyncUrl() ?: "")
    }

    private fun saveSettings() {
        val kbFile = (requireActivity() as MainActivity).kbFile
        val email = binding.editEmail.text?.toString().orEmpty()
        val url = binding.editSyncUrl.text?.toString().orEmpty()

        if (url.isNotEmpty()) kbFile.setSyncUrl(url)
        if (email.isNotEmpty()) kbFile.setEmail(email)
        (requireActivity() as MainActivity).saveKeyboxFile()

        binding.textError.visibility = View.GONE
        parentFragmentManager.popBackStack()
    }

    private fun registerEmail() {
        val activity = requireActivity() as MainActivity
        val kbFile = activity.kbFile
        val email = binding.editEmail.text?.toString().orEmpty()
        val url = binding.editSyncUrl.text?.toString().orEmpty()

        kbFile.setSyncUrl(url)
        kbFile.setEmail(email)

        setRegisterInProgress(true)
        Thread {
            val registerResult = kbFile.register()
            activity.runOnUiThread {
                if (!isAdded || _binding == null) return@runOnUiThread
                setRegisterInProgress(false)
                if (!registerResult.isSuccess) {
                    val detail = registerResult.message.ifBlank {
                        getString(R.string.error_code, registerResult.resultCode)
                    }
                    showAlert(
                        getString(R.string.error),
                        getString(R.string.register_failed_with_detail, detail)
                    )
                    return@runOnUiThread
                }

                if (!activity.saveKeyboxFile()) {
                    showAlert(getString(R.string.error), getString(R.string.save_keybox_failed))
                    return@runOnUiThread
                }

                val message = registerResult.message.ifBlank { getString(R.string.register_success_default) }
                showAlert(getString(R.string.success), message)
                parentFragmentManager.popBackStack()
            }
        }.start()
    }

    private fun setupNewClient() {
        val activity = requireActivity() as MainActivity
        val kbFile = activity.kbFile
        val email = binding.editEmail.text?.toString().orEmpty()
        val url = binding.editSyncUrl.text?.toString().orEmpty()

        kbFile.setSyncUrl(url)
        kbFile.setEmail(email)
        if (!activity.saveKeyboxFile()) {
            showAlert(getString(R.string.error), getString(R.string.save_keybox_failed))
            return
        }

        setSetupInProgress(true)
        Thread {
            val setupResult = kbFile.setupNewClient()
            activity.runOnUiThread {
                if (!isAdded || _binding == null) return@runOnUiThread
                setSetupInProgress(false)
                val encryptedUrl = setupResult.encryptedUrl
                if (!setupResult.isSuccess || encryptedUrl == null) {
                    val detail = setupResult.message.ifBlank {
                        getString(R.string.error_code, setupResult.resultCode)
                    }
                    showAlert(
                        getString(R.string.error),
                        getString(R.string.setup_new_client_request_failed_with_detail, detail)
                    )
                } else {
                    parentFragmentManager.beginTransaction()
                        .replace(R.id.nav_host_fragment, NewClientSetupFragment.newInstance(encryptedUrl))
                        .addToBackStack(null)
                        .commit()
                }
            }
        }.start()
    }

    private fun confirmDeleteAccount() {
        val email = binding.editEmail.text?.toString()?.trim().orEmpty()
        val url = binding.editSyncUrl.text?.toString().orEmpty()
        if (email.isEmpty()) {
            showAlert(getString(R.string.error), getString(R.string.email_required))
            return
        }
        if (url.isEmpty()) {
            showAlert(getString(R.string.error), getString(R.string.sync_url_required))
            return
        }

        AlertDialog.Builder(requireContext())
            .setTitle(R.string.delete_account)
            .setMessage(getString(R.string.delete_account_confirmation, email))
            .setNegativeButton(R.string.cancel, null)
            .setPositiveButton(R.string.delete) { _, _ -> confirmDeleteAccountFinal(email, url) }
            .show()
    }

    private fun confirmDeleteAccountFinal(email: String, url: String) {
        AlertDialog.Builder(requireContext())
            .setTitle(R.string.confirm_account_deletion)
            .setMessage(getString(R.string.delete_account_final_confirmation, email))
            .setNegativeButton(R.string.cancel, null)
            .setPositiveButton(R.string.delete) { _, _ -> deleteAccount(email, url) }
            .show()
    }

    private fun deleteAccount(email: String, url: String) {
        val activity = requireActivity() as MainActivity
        val kbFile = activity.kbFile

        kbFile.setSyncUrl(url)
        kbFile.setEmail(email)

        setDeleteInProgress(true)
        Thread {
            val deleteResult = kbFile.deleteRemoteAccount()
            activity.runOnUiThread {
                if (!isAdded || _binding == null) return@runOnUiThread
                setDeleteInProgress(false)
                if (!deleteResult.isSuccess) {
                    val detail = deleteResult.message.ifBlank {
                        getString(R.string.error_code, deleteResult.resultCode)
                    }
                    showAlert(
                        getString(R.string.error),
                        getString(R.string.delete_account_failed_with_detail, detail)
                    )
                    return@runOnUiThread
                }

                showAlert(getString(R.string.success), getString(R.string.delete_account_success))
                parentFragmentManager.popBackStack()
            }
        }.start()
    }

    private fun setSetupInProgress(inProgress: Boolean) {
        operationInProgress = inProgress
        binding.btnSetupNewClient.text = getString(
            if (inProgress) R.string.setting_up else R.string.setup_new_client
        )
        updateButtonStates()
    }

    private fun setRegisterInProgress(inProgress: Boolean) {
        operationInProgress = inProgress
        binding.btnRegisterEmail.text = getString(
            if (inProgress) R.string.registering else R.string.register_new_email
        )
        updateButtonStates()
    }

    private fun setDeleteInProgress(inProgress: Boolean) {
        operationInProgress = inProgress
        binding.btnDeleteAccount.text = getString(
            if (inProgress) R.string.deleting_account else R.string.delete_account
        )
        updateButtonStates()
    }

    private fun showAlert(title: String, message: String) {
        AlertDialog.Builder(requireContext())
            .setTitle(title)
            .setMessage(message)
            .setPositiveButton(android.R.string.ok, null)
            .show()
    }

    private fun updateButtonStates() {
        val email = binding.editEmail.text?.toString().orEmpty()
        val url = binding.editSyncUrl.text?.toString().orEmpty()
        val bothFilled = email.isNotEmpty() && url.isNotEmpty()
        binding.btnRegisterEmail.isEnabled = bothFilled && !operationInProgress
        binding.btnSetupNewClient.isEnabled = bothFilled && !operationInProgress
        binding.btnDeleteAccount.isEnabled = bothFilled && !operationInProgress
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
