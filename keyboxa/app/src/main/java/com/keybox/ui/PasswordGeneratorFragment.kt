package com.keybox.ui

import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.widget.doAfterTextChanged
import androidx.fragment.app.Fragment
import androidx.fragment.app.setFragmentResult
import com.keybox.NativeBridge
import com.keybox.R
import com.keybox.databinding.FragmentPasswordGeneratorBinding

class PasswordGeneratorFragment : Fragment() {

    private var _binding: FragmentPasswordGeneratorBinding? = null
    private val binding get() = _binding!!
    private val bridge = NativeBridge()
    private val pickerMode: Boolean get() = arguments?.getBoolean(ARG_PICKER_MODE) == true

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentPasswordGeneratorBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (requireActivity() as AppCompatActivity).supportActionBar?.title =
            getString(R.string.password_generator)

        binding.editLength.setText(DEFAULT_LENGTH.toString())
        binding.switchLowercase.isChecked = true
        binding.switchDigits.isChecked = true

        val regenerateListener = View.OnClickListener { regeneratePassword() }
        listOf(
            binding.switchLowercase,
            binding.switchUppercase,
            binding.switchDigits,
            binding.switchMinus,
            binding.switchAdd,
            binding.switchSymbols,
            binding.switchBraces,
            binding.switchSpace,
            binding.switchQuestion,
            binding.switchSlash,
            binding.switchGreaterLess
        ).forEach { it.setOnClickListener(regenerateListener) }

        binding.editLength.doAfterTextChanged { regeneratePassword() }
        binding.buttonRegenerate.setOnClickListener { regeneratePassword() }
        binding.buttonCopy.setOnClickListener { copyPassword() }
        binding.buttonUsePassword.visibility = if (pickerMode) View.VISIBLE else View.GONE
        binding.buttonUsePassword.setOnClickListener { usePassword() }

        regeneratePassword()
    }

    private fun regeneratePassword() {
        val length = binding.editLength.text?.toString()?.toIntOrNull()
            ?.coerceIn(MIN_LENGTH, MAX_LENGTH)
            ?: DEFAULT_LENGTH

        val password = bridge.generatePassword(
            length = length,
            lower = binding.switchLowercase.isChecked,
            upper = binding.switchUppercase.isChecked,
            digit = binding.switchDigits.isChecked,
            minus = binding.switchMinus.isChecked,
            add = binding.switchAdd.isChecked,
            shift18 = binding.switchSymbols.isChecked,
            brace = binding.switchBraces.isChecked,
            space = binding.switchSpace.isChecked,
            question = binding.switchQuestion.isChecked,
            slash = binding.switchSlash.isChecked,
            greaterLess = binding.switchGreaterLess.isChecked
        ).orEmpty()

        binding.textPassword.setText(password)
        binding.buttonCopy.isEnabled = password.isNotEmpty()
        binding.textError.visibility = if (password.isEmpty()) View.VISIBLE else View.GONE
    }

    private fun copyPassword() {
        val password = binding.textPassword.text?.toString().orEmpty()
        if (password.isEmpty()) return

        val clipboard = requireContext().getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
        clipboard.setPrimaryClip(ClipData.newPlainText(getString(R.string.generated_password), password))
        Toast.makeText(requireContext(), R.string.password_copied, Toast.LENGTH_SHORT).show()
    }

    private fun usePassword() {
        val password = binding.textPassword.text?.toString().orEmpty()
        if (password.isEmpty()) return
        setFragmentResult(
            REQUEST_KEY,
            Bundle().apply { putString(RESULT_PASSWORD, password) }
        )
        parentFragmentManager.popBackStack()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    companion object {
        const val REQUEST_KEY = "password_generator_result"
        const val RESULT_PASSWORD = "password"
        private const val ARG_PICKER_MODE = "picker_mode"
        const val MIN_LENGTH = 4
        const val MAX_LENGTH = 128
        const val DEFAULT_LENGTH = 8

        fun newPicker(): PasswordGeneratorFragment {
            return PasswordGeneratorFragment().apply {
                arguments = Bundle().apply { putBoolean(ARG_PICKER_MODE, true) }
            }
        }
    }
}
