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

        binding.textError.visibility = View.GONE
        parentFragmentManager.popBackStack()
    }

    private fun registerEmail() {
        val kbFile = (requireActivity() as MainActivity).kbFile
        val email = binding.editEmail.text?.toString().orEmpty()
        val url = binding.editSyncUrl.text?.toString().orEmpty()

        kbFile.setSyncUrl(url)
        kbFile.setEmail(email)

        val message = kbFile.register() ?: getString(R.string.register_success_default)
        showAlert(getString(R.string.success), message)
        parentFragmentManager.popBackStack()
    }

    private fun setupNewClient() {
        showAlert(
            getString(R.string.not_implemented),
            getString(R.string.setup_new_client_not_implemented)
        )
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
        binding.btnRegisterEmail.isEnabled = bothFilled
        binding.btnSetupNewClient.isEnabled = bothFilled
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
