//
//  PwdEntryListView.swift
//  keyboxi
//

import SwiftUI


final class PwdEntries: ObservableObject {
    @Published var entries: [PwdEntry] = []
    
    func addNew() -> PwdEntry {
        let newBacking = OPwdEntry()
        let newEntry  = PwdEntry(backing: newBacking)
        entries.append(newEntry)
        return newEntry
    }
    
    /// Remove an existing entry (by id or by reference)
    func delete(_ entry: PwdEntry) {
        if let index = entries.firstIndex(where: { $0.id == entry.id }) {
            entries.remove(at: index)
        }
    }
    
    /// Persist *all* entries – just a convenience wrapper
    func persistAll() {
        for e in entries { e.save() }      // Each PwdEntry knows how to write itself
    }
}

struct PwdRow: View {
    @ObservedObject var entry: PwdEntry          // ← each row listens to its own object
    @Environment(\.editMode) private var editMode
    let store: PwdEntries                    // needed for deletion if desired
    
    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text(entry.title.isEmpty ? "(no title)" : entry.title)
                .font(.headline)
            
            HStack {
                Image(systemName: "globe")
                Text(entry.url).foregroundColor(.secondary)
            }
            HStack {
                Image(systemName: "person.crop.circle")
                Text(entry.userName).foregroundColor(.secondary)
            }
        }
        .contextMenu {   // optional long‑press menu
            Button(role: .destructive) {
                store.delete(entry)
            } label: { Label("Delete", systemImage: "trash") }
        }
    }
}



struct PwdEntryListView: View {
    @ObservedObject var entries: PwdEntries
    @ObservedObject var groups: PwdGroups

    var body: some View {
        NavigationView {
            List {
                ForEach(entries.entries) { entry in
                    NavigationLink(destination: PwdEntryView(groups: groups, entry: entry)) {
                        PwdRow(entry: entry, store: entries)
                    }
                }
            }
            .navigationTitle("Passwords")
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button(action: {
                        _ = entries.addNew()
                    }) {
                        Image(systemName: "plus")
                    }
                }
            }
        }
    }
}

#Preview {
    let entries = PwdEntries()
    let groups = PwdGroups()
    
    // Add test groups
    let group1 = groups.addNewGroup()
    group1.name = "Work Accounts"
    
    let group2 = groups.addNewGroup()
    group2.name = "Personal Accounts"
    
    let group3 = groups.addNewGroup()
    group3.name = "Finance"
    
    // Add test entries
    let entry1 = entries.addNew()
    entry1.title = "Gmail"
    entry1.url = "https://mail.google.com"
    entry1.userName = "user@gmail.com"
    
    let entry2 = entries.addNew()
    entry2.title = "GitHub"
    entry2.url = "https://github.com"
    entry2.userName = "john_doe"
    
    let entry3 = entries.addNew()
    entry3.title = "Apple ID"
    entry3.url = "https://appleid.apple.com"
    entry3.userName = "user@icloud.com"
    
    return PwdEntryListView(entries: entries, groups: groups)
}
