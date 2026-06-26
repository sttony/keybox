package com.keybox.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.recyclerview.widget.LinearLayoutManager
import com.keybox.Entry
import com.keybox.MainActivity
import com.keybox.R
import com.keybox.adapter.EntryAdapter
import com.keybox.databinding.FragmentEntryListBinding
import com.keybox.viewmodel.MainViewModel

class EntryListFragment : Fragment() {

    private var _binding: FragmentEntryListBinding? = null
    private val binding get() = _binding!!
    private val viewModel: MainViewModel by activityViewModels()
    private lateinit var adapter: EntryAdapter
    private var allEntries: List<Entry> = emptyList()
    private var selectedGroupId: String? = null

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentEntryListBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (requireActivity() as AppCompatActivity).supportActionBar?.title =
            getString(R.string.passwords)

        adapter = EntryAdapter(
            onEntryClick = ::openEntryEditor,
            onEntryLongClick = ::confirmDeleteEntry
        )
        binding.recyclerView.layoutManager = LinearLayoutManager(context)
        binding.recyclerView.adapter = adapter

        viewModel.entries.observe(viewLifecycleOwner) { entries ->
            allEntries = entries
            renderEntries()
        }
        viewModel.selectedGroup.observe(viewLifecycleOwner) { group ->
            selectedGroupId = group?.id
            (requireActivity() as AppCompatActivity).supportActionBar?.title =
                group?.name ?: getString(R.string.passwords)
            renderEntries()
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private fun confirmDeleteEntry(entry: Entry) {
        AlertDialog.Builder(requireContext())
            .setTitle(R.string.delete_entry)
            .setMessage(getString(R.string.delete_entry_confirmation, entry.title.ifEmpty { getString(R.string.untitled_entry) }))
            .setNegativeButton(android.R.string.cancel, null)
            .setPositiveButton(R.string.delete) { _, _ -> deleteEntry(entry) }
            .show()
    }

    private fun openEntryEditor(entry: Entry) {
        parentFragmentManager.beginTransaction()
            .replace(R.id.nav_host_fragment, EntryEditorFragment.newInstance(entry))
            .addToBackStack(null)
            .commit()
    }

    private fun deleteEntry(entry: Entry) {
        val result = (requireActivity() as MainActivity).removeEntryAndRefresh(entry.id)
        if (result != 0) {
            AlertDialog.Builder(requireContext())
                .setTitle(R.string.delete_entry_failed_title)
                .setMessage(getString(R.string.delete_entry_failed, result))
                .setPositiveButton(android.R.string.ok, null)
                .show()
        }
    }

    private fun renderEntries() {
        if (!::adapter.isInitialized) return
        val entries = selectedGroupId?.let { groupId ->
            allEntries.filter { it.groupUuid.equals(groupId, ignoreCase = true) }
        } ?: allEntries

        adapter.updateEntries(entries)
        if (entries.isEmpty()) {
            binding.textEmpty.visibility = View.VISIBLE
            binding.recyclerView.visibility = View.GONE
        } else {
            binding.textEmpty.visibility = View.GONE
            binding.recyclerView.visibility = View.VISIBLE
        }
    }
}
