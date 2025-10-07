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
                            .foregroundStyle(.accent)
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
                    NavigationLink(destination: PwdEntryView(groups: [group], entry: entry, onSave: { updated in
                        onUpdate(updated)
                    }, onCancel: {})) {
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
                PwdEntryView(groups: [group], entry: nil, onSave: { newEntry in
                    // Ensure the new entry carries the group id
                    newEntry.groupId = group.id
                    onCreate(newEntry)
                    showingNew = false
                }, onCancel: {
                    showingNew = false
                })
            }
        }
    }
}

#Preview {
    // Sample groups
    let g1 = PwdGroup(id: UUID().uuidString, name: "General")
    let g2 = PwdGroup(id: UUID().uuidString, name: "Work")
    let g3 = PwdGroup(id: UUID().uuidString, name: "Personal")
    let groups = [g1, g2, g3]

    // Sample entries leveraging OPwdEntry wrapper
    func makeEntry(title: String, user: String, url: String, groupId: String) -> PwdEntry {
        let o = OPwdEntry()
        o.setTitle(title)
        o.setUsername(user)
        o.setUrl(url)
        if let gid = NSUUID(uuidString: groupId) { o.setGroupUUID(gid) }
        // minimal pad for demo
        let pwd = "secret-\(Int.random(in: 100...999))"
        o.setPassword(pwd, onePad: Data(count: pwd.lengthOfBytes(using: .utf8)))
        return PwdEntry(backing: o)
    }

    let e1 = makeEntry(title: "Email", user: "alice", url: "mail.example.com", groupId: g1.id)
    let e2 = makeEntry(title: "Forum", user: "bob", url: "forum.example.com", groupId: g1.id)
    let e3 = makeEntry(title: "Corp VPN", user: "carol", url: "vpn.company.com", groupId: g2.id)

    let map: [String: [PwdEntry]] = [
        g1.id: [e1, e2],
        g2.id: [e3],
        g3.id: []
    ]

    return GroupToEntryView(groups: groups, entriesByGroup: map)
}