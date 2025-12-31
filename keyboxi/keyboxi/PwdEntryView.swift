import SwiftUI
import Foundation


struct PwdEntryView: View {
    // Input data
    var groups: PwdGroups
    var initialEntry: PwdEntry
    var onSave: (PwdEntry) -> Void
    var onCancel: () -> Void

    // Local editable state
    @State private var title: String
    @State private var url: String
    @State private var userName: String
    @State private var password: String
    @State private var note: String
    @State private var selectedGroupId: String

    init(groups: PwdGroups, entry: PwdEntry? = nil, onSave: @escaping (PwdEntry) -> Void = { _ in }, onCancel: @escaping () -> Void = {}) {
        self.groups = groups
        let entryValue = entry ?? PwdEntry()
        self.initialEntry = entryValue
        self._title = State(initialValue: entryValue.title)
        self._url = State(initialValue: entryValue.url)
        self._userName = State(initialValue: entryValue.userName)
        self._password = State(initialValue: entryValue.password)
        self._note = State(initialValue: entryValue.note)
        // default to first group if not set
        let gid = entryValue.groupId
        let defaultGroupId = gid.isEmpty ? (groups.first?.id ?? "") : gid
        self._selectedGroupId = State(initialValue: defaultGroupId)
        self.onSave = onSave
        self.onCancel = onCancel
    }

    var body: some View {
        VStack(spacing: 0) {
            Form {
                Section(header: Text("General")) {
                    HStack {
                        Text("Title")
                        TextField("Title", text: $title)
                            .textInputAutocapitalization(.sentences)
                    }
                    HStack {
                        Text("Url")
                        TextField("https://", text: $url)
                            .keyboardType(.URL)
                            .textInputAutocapitalization(.never)
                            .autocorrectionDisabled()
                    }
                    HStack {
                        Text("Username")
                        TextField("username", text: $userName)
                            .textInputAutocapitalization(.never)
                            .autocorrectionDisabled()
                    }
                }

                Section(header: Text("Security")) {
                    HStack {
                        Text("Password")
                        SecureField("Password", text: $password)
                    }
                }

                Section(header: Text("Note")) {
                    TextEditor(text: $note)
                        .frame(minHeight: 120)
                }

                Section(header: Text("Group")) {
                    Picker("Group", selection: $selectedGroupId) {
                        ForEach(groups) { g in
                            Text(g.name).tag(g.id)
                        }
                    }
                }
            }

            Divider()

            HStack {
                Button(role: .cancel) {
                    onCancel()
                } label: {
                    Text("Cancel")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.bordered)

                Button(action: { saveTapped() }) {
                    Text("Save")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                .disabled(!canSave)
            }
            .padding()
        }
        .navigationTitle(initialEntry.title.isEmpty ? "New Entry" : "Edit Entry")
    }

    private var canSave: Bool {
        !title.trimmingCharacters(in: .whitespacesAndNewlines).isEmpty
    }

    private func saveTapped() {
        let entry = initialEntry
        entry.title = title
        entry.url = url
        entry.userName = userName
        entry.password = password
        entry.note = note
        entry.groupId = selectedGroupId
        onSave(entry)
    }
}

#Preview {
    NavigationView {
        let groups = PwdGroups()
        
        // Create fake test groups
        let generalGroup = groups.addNewGroup()
        generalGroup.name = "General"
        
        let workGroup = groups.addNewGroup()
        workGroup.name = "Work"
        
        let personalGroup = groups.addNewGroup()
        personalGroup.name = "Personal"
        
       
        
        return PwdEntryView(groups: groups)
    }
}
