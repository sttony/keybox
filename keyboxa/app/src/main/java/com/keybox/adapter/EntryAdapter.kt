package com.keybox.adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.keybox.Entry
import com.keybox.R

class EntryAdapter(
    private var entries: List<Entry> = emptyList(),
    private val onEntryClick: (Entry) -> Unit = {},
    private val onEntryLongClick: (Entry) -> Unit = {}
) :
    RecyclerView.Adapter<EntryAdapter.ViewHolder>() {

    class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val titleText: TextView = view.findViewById(R.id.text_title)
        val urlText: TextView = view.findViewById(R.id.text_url)
        val usernameText: TextView = view.findViewById(R.id.text_username)
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
