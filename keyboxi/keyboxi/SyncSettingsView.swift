//
//  SyncSettingsView.swift
//  keyboxi
//

import SwiftUI

struct SyncSettingsView: View {
    @EnvironmentObject var appState: AppState
    @Environment(\.dismiss) var dismiss

    @State private var email: String = ""
    @State private var syncUrl: String = ""
    @State private var errorMessage: String?
    @State private var showingAlert = false
    @State private var alertTitle = ""
    @State private var alertMessage = ""

    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Sync Configuration")) {
                    TextField("Email", text: $email)
                        .autocapitalization(.none)
                        .keyboardType(.emailAddress)

                    TextField("Sync Server URL", text: $syncUrl)
                        .autocapitalization(.none)
                        .keyboardType(.URL)
                }

                if let error = errorMessage {
                    Section {
                        Text(error)
                            .foregroundColor(.red)
                            .font(.caption)
                    }
                }

                Section {
                    Button("Register New Email") {
                        registerEmail()
                    }
                    .disabled(email.isEmpty || syncUrl.isEmpty)

                    Button("Setup New Client") {
                        setupNewClient()
                    }
                    .disabled(email.isEmpty || syncUrl.isEmpty)
                }
                
                // Security section moved out to the main SlideDrawer menu
            }
            .navigationTitle("Sync Settings")
            .navigationBarItems(
                leading: Button("Cancel") {
                    dismiss()
                },
                trailing: Button("Save") {
                    saveSettings()
                }
                .disabled(email.isEmpty && syncUrl.isEmpty)
            )
            .alert(alertTitle, isPresented: $showingAlert) {
                Button("OK", role: .cancel) { }
            } message: {
                Text(alertMessage)
            }
        }
        .onAppear {
            loadCurrentSettings()
        }
    }

    private func loadCurrentSettings() {
        guard let kbFile = appState.kbFile else { return }

        email = kbFile.getEmail() ?? ""
        syncUrl = kbFile.getSyncUrl() ?? ""
    }

    private func saveSettings() {
        guard let kbFile = appState.kbFile else {
            errorMessage = "No keybox file loaded"
            return
        }

        do {
            if !syncUrl.isEmpty {
                try kbFile.setSyncUrl(syncUrl)
            }
            if !email.isEmpty {
                try kbFile.setEmail(email)
            }

            // Save the file
            appState.saveFile()

            errorMessage = nil
            dismiss()
        } catch {
            errorMessage = "Failed to save settings: \(error.localizedDescription)"
        }
    }

    private func registerEmail() {
        guard let kbFile = appState.kbFile else {
            showAlert(title: "Error", message: "No keybox file loaded")
            return
        }

        do {
            try kbFile.setSyncUrl(syncUrl)
            try kbFile.setEmail(email)

            var message: NSString?
            try kbFile.register(withMessage: &message)

            let successMsg = message as String? ?? "Register success, please check your email to activate"
            showAlert(title: "Success", message: successMsg)
            appState.saveFile()
            dismiss()
        } catch {
            showAlert(title: "Error", message: "Failed to register: \(error.localizedDescription)")
        }
    }

    private func setupNewClient() {
        showAlert(title: "Not Implemented", message: "Setup new client feature is not yet implemented in iOS version")
    }

    private func showAlert(title: String, message: String) {
        alertTitle = title
        alertMessage = message
        showingAlert = true
    }
}

#Preview {
    let appState = AppState()
    appState.isFileLoaded = true
    appState.isUnlocked = true

    return SyncSettingsView()
        .environmentObject(appState)
}
