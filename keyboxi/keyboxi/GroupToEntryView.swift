import SwiftUI
import Foundation

// View that lets user navigate from Groups to the Password Entries table/list,
// and then into the existing PwdEntryView for viewing/editing/creating entries.
struct GroupToEntryView: View {
    // Input models
    var groups: [PwdGroup]
    // Mapping groupId -> entries
    @State var entriesByGroup: [String: [PwdEntry]]

    // Callbacks (optional) to bubble up persistence events
    var onCreate: ((String, PwdEntry) -> Void)? = nil // (groupId, entry)
    var onUpdate: ((String, PwdEntry) -> Void)? = nil // (groupId, entry)
    var onDelete: ((String, PwdEntry) -> Void)? = nil // (groupId, entry)

    init(groups: [PwdGroup],
         entriesByGroup: [String: [PwdEntry]] = [:],
         onCreate: ((String, PwdEntry) -> Void)? = nil,
         onUpdate: ((String, PwdEntry) -> Void)? = nil,
         onDelete: ((String, PwdEntry) -> Void)? = nil) {
        self.groups = groups
        self._entriesByGroup = State(initialValue: entriesByGroup)
        self.onCreate = onCreate
        self.onUpdate = onUpdate
        self.onDelete = onDelete
    }

    var body: some View {
        NavigationStack {
            List(groups) { group in
                NavigationLink(value: group) {
                    HStack {
                        Image(systemName: "folder")
                            .foregroundStyle(.blue)
                        Text(group.name)
                        Spacer()
                        let count = (entriesByGroup[group.id] ?? []).count
                        Text("\(count)")
                            .foregroundStyle(.secondary)
                    }
                }
            }
            .navigationTitle("Groups")
            .navigationDestination(for: PwdGroup.self) { group in
                EntriesListView(
                    group: group,
                    entries: entriesBinding(for: group.id),
                    onCreate: { entry in
                        // append and bubble up
                        entriesByGroup[group.id, default: []].append(entry)
                        onCreate?(group.id, entry)
                    },
                    onUpdate: { entry in
                        // replace by identity within array and bubble up
                        if var arr = entriesByGroup[group.id] {
                            if let idx = arr.firstIndex(of: entry) {
                                arr[idx] = entry
                                entriesByGroup[group.id] = arr
                            }
                        }
                        onUpdate?(group.id, entry)
                    },
                    onDelete: { entry in
                        if var arr = entriesByGroup[group.id] {
                            arr.removeAll { $0 == entry }
                            entriesByGroup[group.id] = arr
                        }
                        onDelete?(group.id, entry)
                    }
                )
            }
        }
    }

    // Helper to create a Binding<[PwdEntry]> for the selected group
    private func entriesBinding(for groupId: String) -> Binding<[PwdEntry]> {
        Binding<[PwdEntry]>(
            get: { entriesByGroup[groupId] ?? [] },
            set: { entriesByGroup[groupId] = $0 }
        )
    }
}

// MARK: - Entries List for a Single Group
private struct EntriesListView: View {
    var group: PwdGroup
    @Binding var entries: [PwdEntry]

    var onCreate: (PwdEntry) -> Void
    var onUpdate: (PwdEntry) -> Void
    var onDelete: (PwdEntry) -> Void

    @State private var showingNew = false

    var body: some View {
        List {
            if entries.isEmpty {
                ContentUnavailableView("No Entries", systemImage: "list.bullet", description: Text("Tap + to create a new entry."))
            } else {
                ForEach(entries) { entry in
                    NavigationLink(destination: entryViewFor(entry: entry, group: group, onUpdate: onUpdate)) {
                        VStack(alignment: .leading) {
                            Text(entry.title.isEmpty ? "(untitled)" : entry.title)
                                .font(.headline)
                            HStack(spacing: 12) {
                                if !entry.userName.isEmpty {
                                    Label(entry.userName, systemImage: "person")
                                }
                                if !entry.url.isEmpty {
                                    Label(entry.url, systemImage: "link")
                                }
                            }
                            .foregroundStyle(.secondary)
                            .font(.subheadline)
                        }
                    }
                    .swipeActions(edge: .trailing, allowsFullSwipe: true) {
                        Button(role: .destructive) {
                            onDelete(entry)
                        } label: {
                            Label("Delete", systemImage: "trash")
                        }
                    }
                }
            }
        }
        .navigationTitle(group.name)
        .toolbar {
            ToolbarItem(placement: .navigationBarTrailing) {
                Button(action: { showingNew = true }) {
                    Image(systemName: "plus")
                }
                .accessibilityLabel("Add Entry")
            }
        }
        .sheet(isPresented: $showingNew) {
            NavigationStack {
                newEntryView(group: group, onCreate: onCreate, onDismiss: { showingNew = false })
            }
        }
    }

    private func entryViewFor(entry: PwdEntry, group: PwdGroup, onUpdate: @escaping (PwdEntry) -> Void) -> some View {
        let pwdGroups = PwdGroups()
        pwdGroups.groups = [group]
        return PwdEntryView(groups: pwdGroups, entry: entry, onSave: { updated in
            onUpdate(updated)
        }, onCancel: {})
    }

    private func newEntryView(group: PwdGroup, onCreate: @escaping (PwdEntry) -> Void, onDismiss: @escaping () -> Void) -> some View {
        let pwdGroups = PwdGroups()
        pwdGroups.groups = [group]
        return PwdEntryView(groups: pwdGroups, entry: nil, onSave: { newEntry in
            newEntry.groupId = group.id
            onCreate(newEntry)
            onDismiss()
        }, onCancel: {
            onDismiss()
        })
    }
}

#Preview("Group to Entry View") {
    GroupToEntryView(groups: [], entriesByGroup: [:])
}