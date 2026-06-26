package com.keybox.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AppCompatActivity
import androidx.core.widget.doAfterTextChanged
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import com.keybox.MainActivity
import com.keybox.R
import com.keybox.databinding.FragmentUnlockBinding
import com.keybox.viewmodel.MainViewModel

class UnlockFragment : Fragment() {

    private var _binding: FragmentUnlockBinding? = null
    private val binding get() = _binding!!
    private val viewModel: MainViewModel by activityViewModels()

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentUnlockBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (requireActivity() as AppCompatActivity).supportActionBar?.title = getString(R.string.app_name)

        binding.editPassword.doAfterTextChanged { updateButtonState() }
        binding.editConfirmPassword.doAfterTextChanged { updateButtonState() }
        binding.buttonPrimary.setOnClickListener { submitPassword() }

        viewModel.pendingNewFile.observe(viewLifecycleOwner) { updateMode() }
        viewModel.loadError.observe(viewLifecycleOwner) { error ->
            binding.textError.text = error.orEmpty()
            binding.textError.visibility = if (error.isNullOrBlank()) View.GONE else View.VISIBLE
        }
    }

    private fun updateMode() {
        val pendingNewFile = viewModel.pendingNewFile.value == true
        binding.textState.text = getString(
            if (pendingNewFile) R.string.create_keybox_message else R.string.unlock_keybox_message
        )
        binding.layoutConfirmPassword.visibility = if (pendingNewFile) View.VISIBLE else View.GONE
        binding.buttonPrimary.text = getString(if (pendingNewFile) R.string.create else R.string.unlock)
        updateButtonState()
    }

    private fun updateButtonState() {
        val pendingNewFile = viewModel.pendingNewFile.value == true
        val password = binding.editPassword.text?.toString().orEmpty()
        val confirm = binding.editConfirmPassword.text?.toString().orEmpty()
        binding.buttonPrimary.isEnabled = password.isNotEmpty() && (!pendingNewFile || confirm.isNotEmpty())
        binding.layoutPassword.error = null
        binding.layoutConfirmPassword.error = null
    }

    private fun submitPassword() {
        val password = binding.editPassword.text?.toString().orEmpty()
        val confirm = binding.editConfirmPassword.text?.toString().orEmpty()
        val pendingNewFile = viewModel.pendingNewFile.value == true

        if (password.isEmpty()) {
            binding.layoutPassword.error = getString(R.string.password_required)
            return
        }
        if (pendingNewFile && password != confirm) {
            binding.layoutConfirmPassword.error = getString(R.string.passwords_do_not_match)
            return
        }

        val activity = requireActivity() as MainActivity
        val success = if (pendingNewFile) {
            activity.createNewKeyboxWithPassword(password)
        } else {
            activity.unlockExistingKeybox(password)
        }

        if (success) {
            binding.editPassword.text?.clear()
            binding.editConfirmPassword.text?.clear()
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
