package com.keybox.adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.keybox.Entry
import com.keybox.R

class EntryAdapter(
    private var entries: List<Entry> = emptyList(),
    private val onEntryClick: (Entry) -> Unit = {},
    private val onEntryLongClick: (Entry) -> Unit = {},
    private val onDeleteEntry: (Entry) -> Unit = {},
    private val onCopyPassword: (Entry) -> Unit = {}
) :
    RecyclerView.Adapter<EntryAdapter.ViewHolder>() {

    class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val titleText: TextView = view.findViewById(R.id.text_title)
        val urlText: TextView = view.findViewById(R.id.text_url)
        val usernameText: TextView = view.findViewById(R.id.text_username)
        val copyPasswordButton: Button = view.findViewById(R.id.button_copy_password)
        val deleteEntryButton: Button = view.findViewById(R.id.button_delete_entry)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_pwd_entry, parent, false)
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val entry = entries[position]
        holder.titleText.text = if (entry.title.isEmpty()) "(no title)" else entry.title
        holder.urlText.text = entry.url
        holder.usernameText.text = entry.username
        
        // Ensure visibility of fields even if empty (for better layout stability)
        holder.urlText.visibility = if (entry.url.isEmpty()) View.GONE else View.VISIBLE
        holder.usernameText.visibility = if (entry.username.isEmpty()) View.GONE else View.VISIBLE
        holder.copyPasswordButton.isEnabled = entry.password.isNotEmpty()
        holder.copyPasswordButton.setOnClickListener { onCopyPassword(entry) }
        holder.deleteEntryButton.setOnClickListener { onDeleteEntry(entry) }
        holder.itemView.setOnLongClickListener {
            onEntryLongClick(entry)
            true
        }
        holder.itemView.setOnClickListener {
            onEntryClick(entry)
        }
    }

    override fun getItemCount() = entries.size

    fun updateEntries(newEntries: List<Entry>) {
        entries = newEntries
        notifyDataSetChanged()
    }
}
