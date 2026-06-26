package com.keybox.ui

import android.os.Bundle
import android.util.Base64
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.widget.doAfterTextChanged
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.fragment.app.setFragmentResultListener
import com.keybox.Entry
import com.keybox.Group
import com.keybox.MainActivity
import com.keybox.R
import com.keybox.databinding.FragmentEntryEditorBinding
import com.keybox.viewmodel.MainViewModel

class EntryEditorFragment : Fragment() {

    private var _binding: FragmentEntryEditorBinding? = null
    private val binding get() = _binding!!
    private val viewModel: MainViewModel by activityViewModels()
    private var groups: List<Group> = emptyList()
    private var editingEntry: Entry? = null
    private var attachment: ByteArray = ByteArray(0)

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentEntryEditorBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        editingEntry = entryFromArguments()
        (requireActivity() as AppCompatActivity).supportActionBar?.title =
            getString(if (editingEntry == null) R.string.new_entry else R.string.edit_entry)

        binding.editTitle.doAfterTextChanged { updateSaveState() }
        binding.buttonGeneratePassword.setOnClickListener { openPasswordGenerator() }
        binding.buttonReadAttachment.setOnClickListener { showAttachment() }
        binding.buttonCancel.setOnClickListener { parentFragmentManager.popBackStack() }
        binding.buttonSave.setOnClickListener { saveEntry() }
        setFragmentResultListener(PasswordGeneratorFragment.REQUEST_KEY) { _, result ->
            binding.editPassword.setText(result.getString(PasswordGeneratorFragment.RESULT_PASSWORD).orEmpty())
        }
        populateFields()
        loadAttachment()
        viewModel.groups.observe(viewLifecycleOwner) { newGroups ->
            groups = newGroups
            renderGroupPicker()
        }
        updateSaveState()
    }

    private fun updateSaveState() {
        binding.buttonSave.isEnabled = binding.editTitle.text?.toString()?.trim().orEmpty().isNotEmpty()
        binding.layoutTitle.error = null
        binding.textError.visibility = View.GONE
    }

    private fun openPasswordGenerator() {
        parentFragmentManager.beginTransaction()
            .replace(R.id.nav_host_fragment, PasswordGeneratorFragment.newPicker())
            .addToBackStack(null)
            .commit()
    }

    private fun saveEntry() {
        val title = binding.editTitle.text?.toString()?.trim().orEmpty()
        if (title.isEmpty()) {
            binding.layoutTitle.error = getString(R.string.title_required)
            return
        }

        val entry = Entry(
            id = editingEntry?.id.orEmpty(),
            title = title,
            username = binding.editUsername.text?.toString().orEmpty(),
            url = binding.editUrl.text?.toString().orEmpty(),
            password = binding.editPassword.text?.toString().orEmpty(),
            note = binding.editNote.text?.toString().orEmpty(),
            groupUuid = selectedGroupId()
        )

        val activity = requireActivity() as MainActivity
        val result = if (editingEntry == null) {
            activity.addEntryAndRefresh(entry)
        } else {
            activity.updateEntryAndRefresh(entry)
        }
        if (result == 0) {
            parentFragmentManager.popBackStack()
        } else {
            binding.textError.text = getString(
                if (editingEntry == null) R.string.save_entry_failed else R.string.update_entry_failed,
                result
            )
            binding.textError.visibility = View.VISIBLE
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private fun renderGroupPicker() {
        if (_binding == null) return
        binding.spinnerGroup.adapter = ArrayAdapter(
            requireContext(),
            android.R.layout.simple_spinner_dropdown_item,
            groups.map { it.name }
        )

        val selectedGroupId = editingEntry?.groupUuid ?: viewModel.selectedGroup.value?.id
        val selectedIndex = groups.indexOfFirst { it.id.equals(selectedGroupId, ignoreCase = true) }
            .takeIf { it >= 0 }
            ?: 0
        if (groups.isNotEmpty()) {
            binding.spinnerGroup.setSelection(selectedIndex)
        }
        binding.groupPickerContainer.visibility = if (groups.isEmpty()) View.GONE else View.VISIBLE
    }

    private fun selectedGroupId(): String {
        val index = binding.spinnerGroup.selectedItemPosition
        return groups.getOrNull(index)?.id.orEmpty()
    }

    private fun populateFields() {
        val entry = editingEntry ?: return
        binding.editTitle.setText(entry.title)
        binding.editUrl.setText(entry.url)
        binding.editUsername.setText(entry.username)
        binding.editPassword.setText(entry.password)
        binding.editNote.setText(entry.note)
    }

    private fun loadAttachment() {
        val entry = editingEntry
        if (entry == null) {
            binding.attachmentContainer.visibility = View.GONE
            return
        }
        attachment = (requireActivity() as MainActivity).getEntryAttachment(entry.id) ?: ByteArray(0)
        binding.attachmentContainer.visibility = View.VISIBLE
        binding.textAttachmentSummary.text = if (attachment.isEmpty()) {
            getString(R.string.no_attachment)
        } else {
            getString(R.string.attachment_size, attachment.size)
        }
        binding.buttonReadAttachment.visibility = if (attachment.isEmpty()) View.GONE else View.VISIBLE
    }

    private fun showAttachment() {
        val text = attachment.toString(Charsets.UTF_8).takeIf { decoded ->
            decoded.all { it == '\n' || it == '\r' || it == '\t' || !it.isISOControl() }
        } ?: getString(R.string.binary_attachment_base64, Base64.encodeToString(attachment, Base64.NO_WRAP))

        AlertDialog.Builder(requireContext())
            .setTitle(R.string.attachment)
            .setMessage(text)
            .setPositiveButton(android.R.string.ok, null)
            .show()
    }

    private fun entryFromArguments(): Entry? {
        val args = arguments ?: return null
        val id = args.getString(ARG_ID).orEmpty()
        if (id.isEmpty()) return null
        return Entry(
            id = id,
            title = args.getString(ARG_TITLE).orEmpty(),
            username = args.getString(ARG_USERNAME).orEmpty(),
            url = args.getString(ARG_URL).orEmpty(),
            password = args.getString(ARG_PASSWORD).orEmpty(),
            note = args.getString(ARG_NOTE).orEmpty(),
            groupUuid = args.getString(ARG_GROUP_UUID).orEmpty()
        )
    }

    companion object {
        private const val ARG_ID = "id"
        private const val ARG_TITLE = "title"
        private const val ARG_USERNAME = "username"
        private const val ARG_URL = "url"
        private const val ARG_PASSWORD = "password"
        private const val ARG_NOTE = "note"
        private const val ARG_GROUP_UUID = "group_uuid"

        fun newInstance(entry: Entry? = null): EntryEditorFragment {
            return EntryEditorFragment().apply {
                if (entry != null) {
                    arguments = Bundle().apply {
                        putString(ARG_ID, entry.id)
                        putString(ARG_TITLE, entry.title)
                        putString(ARG_USERNAME, entry.username)
                        putString(ARG_URL, entry.url)
                        putString(ARG_PASSWORD, entry.password)
                        putString(ARG_NOTE, entry.note)
                        putString(ARG_GROUP_UUID, entry.groupUuid)
                    }
                }
            }
        }
    }
}
