

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
    @State private var showingAddEntry = false
    @State private var showingSyncSettings = false

    var body: some View {
        NavigationStack {
            if let entry = selectedEntry {
                // Navigate to the existing entry (for editing)
                NavigationLink(
                    value: entry,
                    label: {
                        EmptyView()
                    }
                )
            }

            if entries.entries.isEmpty {
                VStack(spacing: 16) {
                    Image(systemName: "key.fill")
                        .font(.system(size: 60))
                        .foregroundColor(.gray)
                    Text("you don't have any password,  can click + to add one")
                        .font(.subheadline)
                        .foregroundColor(.secondary)
                        .multilineTextAlignment(.center)
                        .padding(.horizontal)
                }
            } else {
                List {
                    ForEach(entries.entries) { entry in
                        NavigationLink(destination: PwdEntryView(groups: groups, entry: entry, onSave: { updated in
                            entries.persistAll()
                            selectedEntry = nil
                        })) {
                            PwdRow(entry: entry, store: entries)
                        }
                    }
                }
            }
        }
        .navigationTitle("Passwords")
        .toolbar {
            ToolbarItem(placement: .navigationBarLeading) {
                Button(action: {
                    showingSyncSettings = true
                }) {
                    Image(systemName: "gear")
                }
            }

            ToolbarItem(placement: .navigationBarTrailing) {
                Button(action: {
                    let newEntry = entries.addNew()
                    selectedEntry = newEntry
                }) {
                    Image(systemName: "plus")
                }
            }
        }
        .sheet(isPresented: $showingSyncSettings) {
            SyncSettingsView()
                .environmentObject(appState)
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
