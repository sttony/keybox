package com.keybox.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.TextView
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import com.google.android.material.button.MaterialButton
import com.keybox.Group
import com.keybox.MainActivity
import com.keybox.R
import com.keybox.databinding.FragmentGroupsBinding
import com.keybox.viewmodel.MainViewModel
import java.util.UUID

class GroupsFragment : Fragment() {

    private var _binding: FragmentGroupsBinding? = null
    private val binding get() = _binding!!
    private val viewModel: MainViewModel by activityViewModels()
    private var groups: List<Group> = emptyList()

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentGroupsBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (requireActivity() as AppCompatActivity).supportActionBar?.title = getString(R.string.manage_groups)
        binding.buttonAddGroup.setOnClickListener { showGroupNameDialog(null) }
        viewModel.groups.observe(viewLifecycleOwner) { newGroups ->
            groups = newGroups
            renderGroups()
        }
    }

    private fun renderGroups() {
        binding.groupList.removeAllViews()
        groups.forEach { group ->
            val row = LinearLayout(requireContext()).apply {
                orientation = LinearLayout.HORIZONTAL
                setPadding(0, 8, 0, 8)
            }
            val name = TextView(requireContext()).apply {
                text = group.name
                textSize = 16f
                layoutParams = LinearLayout.LayoutParams(0, ViewGroup.LayoutParams.WRAP_CONTENT, 1f)
            }
            val rename = MaterialButton(requireContext()).apply {
                text = getString(R.string.rename)
                setOnClickListener { showGroupNameDialog(group) }
            }
            val delete = MaterialButton(requireContext()).apply {
                text = getString(R.string.delete)
                isEnabled = group.id != ROOT_GROUP_ID
                setOnClickListener { confirmDelete(group) }
            }
            row.addView(name)
            row.addView(rename)
            row.addView(delete)
            binding.groupList.addView(row)
        }
    }

    private fun showGroupNameDialog(group: Group?) {
        val input = EditText(requireContext()).apply {
            setText(group?.name.orEmpty())
            hint = getString(R.string.group_name)
            selectAll()
        }
        AlertDialog.Builder(requireContext())
            .setTitle(if (group == null) R.string.add_group else R.string.rename_group)
            .setView(input)
            .setNegativeButton(android.R.string.cancel, null)
            .setPositiveButton(R.string.save) { _, _ ->
                val name = input.text?.toString()?.trim().orEmpty()
                if (name.isNotEmpty()) saveGroup(group, name)
            }
            .show()
    }

    private fun saveGroup(group: Group?, name: String) {
        val id = group?.id ?: UUID.randomUUID().toString()
        val result = (requireActivity() as MainActivity).updateGroupAndRefresh(id, name)
        if (result != 0) {
            showError(getString(R.string.save_group_failed, result))
        }
    }

    private fun confirmDelete(group: Group) {
        AlertDialog.Builder(requireContext())
            .setTitle(R.string.delete_group)
            .setMessage(getString(R.string.delete_group_confirmation, group.name))
            .setNegativeButton(android.R.string.cancel, null)
            .setPositiveButton(R.string.delete) { _, _ -> deleteGroup(group) }
            .show()
    }

    private fun deleteGroup(group: Group) {
        val result = (requireActivity() as MainActivity).removeGroupAndRefresh(group.id)
        if (result != 0) {
            showError(getString(R.string.delete_group_failed, result))
        }
    }

    private fun showError(message: String) {
        AlertDialog.Builder(requireContext())
            .setTitle(R.string.error)
            .setMessage(message)
            .setPositiveButton(android.R.string.ok, null)
            .show()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private companion object {
        const val ROOT_GROUP_ID = "60dd3126-6e7a-4e50-961b-fa77a91d7e38"
    }
}
