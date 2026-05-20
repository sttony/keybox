

//
//  PwdEntryListView.swift
//  keyboxi
//

import SwiftUI


final class PwdEntries: ObservableObject {
    @Published var entries: [PwdEntry] = []
    weak var kbFile: OCKBFile?

    func addNew() -> PwdEntry {
        let newBacking = OPwdEntry()
        let newEntry  = PwdEntry(backing: newBacking, kbFile: kbFile)
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
    @EnvironmentObject var appState: AppState

    @State private var selectedEntry: PwdEntry?
    @State private var showingSyncSettings = false

    var filteredEntries: [PwdEntry] {
        if let selectedGroup = appState.selectedGroup {
            return entries.entries.filter { $0.groupId == selectedGroup.id }
        } else {
            return entries.entries
        }
    }

    var body: some View {
        Group {
            if filteredEntries.isEmpty {
                VStack(spacing: 16) {
                    Image(systemName: appState.selectedGroup == nil ? "key.fill" : "folder.badge.questionmark")
                        .font(.system(size: 60))
                        .foregroundColor(.gray)
                    Text(appState.selectedGroup == nil ? "You don't have any passwords.\nClick + to add one." : "No passwords in this group.")
                        .font(.subheadline)
                        .foregroundColor(.secondary)
                        .multilineTextAlignment(.center)
                        .padding(.horizontal)
                }
            } else {
                List {
                    ForEach(filteredEntries) { entry in
                        NavigationLink(destination: PwdEntryView(groups: groups, entry: entry, onSave: { _ in
                            entries.persistAll()
                        })) {
                            PwdRow(entry: entry, store: entries)
                        }
                    }
                }
            }
        }
        .navigationTitle(appState.selectedGroup?.name ?? "Passwords")
        .toolbar {
            ToolbarItem(placement: .navigationBarTrailing) {
                Button(action: {
                    // Create a new entry and trigger the sheet
                    let newEntry = entries.addNew()
                    if let group = appState.selectedGroup {
                        newEntry.groupId = group.id
                    }
                    selectedEntry = newEntry
                }) {
                    Image(systemName: "plus")
                }
            }
        }
        // MODALS ATTACHED TO THE OUTERMOST CONTAINER
        .sheet(item: $selectedEntry) { entry in
            NavigationStack {
                PwdEntryView(groups: groups, entry: entry, onSave: { _ in
                    entries.persistAll()
                    selectedEntry = nil
                })
                .navigationTitle("New Entry")
                .toolbar {
                    ToolbarItem(placement: .cancellationAction) {
                        Button("Cancel") { selectedEntry = nil }
                    }
                }
            }
        }
    }
}

#Preview {
    let appState = AppState()
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

    appState.isFileLoaded = true
    appState.isUnlocked = true

    return PwdEntryListView(entries: entries, groups: groups)
        .environmentObject(appState)
}
