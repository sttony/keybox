package com.keybox.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.widget.doAfterTextChanged
import androidx.fragment.app.Fragment
import com.keybox.MainActivity
import com.keybox.R
import com.keybox.databinding.FragmentChangePasswordBinding

class ChangePasswordFragment : Fragment() {

    private var _binding: FragmentChangePasswordBinding? = null
    private val binding get() = _binding!!
    private var isChanging = false

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentChangePasswordBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (requireActivity() as AppCompatActivity).supportActionBar?.title = getString(R.string.change_password)

        binding.editNewPassword.doAfterTextChanged { updateButtonState() }
        binding.editConfirmPassword.doAfterTextChanged { updateButtonState() }
        binding.buttonCancel.setOnClickListener { parentFragmentManager.popBackStack() }
        binding.buttonChangePassword.setOnClickListener { changePassword() }
        updateButtonState()
    }

    private fun updateButtonState() {
        val password = binding.editNewPassword.text?.toString().orEmpty()
        val confirm = binding.editConfirmPassword.text?.toString().orEmpty()
        binding.buttonChangePassword.isEnabled = password.isNotEmpty() && confirm.isNotEmpty() && !isChanging
        binding.layoutNewPassword.error = null
        binding.layoutConfirmPassword.error = null
        binding.textError.visibility = View.GONE
    }

    private fun changePassword() {
        val password = binding.editNewPassword.text?.toString().orEmpty()
        val confirm = binding.editConfirmPassword.text?.toString().orEmpty()
        if (password.isEmpty()) {
            binding.layoutNewPassword.error = getString(R.string.password_required)
            return
        }
        if (password != confirm) {
            binding.layoutConfirmPassword.error = getString(R.string.passwords_do_not_match)
            return
        }

        setChanging(true)
        Thread {
            val activity = requireActivity() as MainActivity
            val result = activity.kbFile.changePassword(password)
            activity.runOnUiThread {
                setChanging(false)
                if (result == 0) {
                    activity.saveKeyboxFile()
                    AlertDialog.Builder(requireContext())
                        .setTitle(R.string.success)
                        .setMessage(R.string.change_password_success)
                        .setPositiveButton(android.R.string.ok) { _, _ -> parentFragmentManager.popBackStack() }
                        .show()
                } else {
                    binding.textError.text = getString(R.string.change_password_failed, result)
                    binding.textError.visibility = View.VISIBLE
                }
            }
        }.start()
    }

    private fun setChanging(changing: Boolean) {
        isChanging = changing
        binding.progressChange.visibility = if (changing) View.VISIBLE else View.GONE
        binding.buttonChangePassword.text = getString(
            if (changing) R.string.changing_password else R.string.change_password
        )
        updateButtonState()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
