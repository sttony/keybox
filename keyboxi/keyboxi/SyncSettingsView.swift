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
    @State private var isRegistering: Bool = false
    @State private var isSettingUp: Bool = false
    @State private var isDeleting: Bool = false
    @State private var showingDeleteConfirmation: Bool = false
    @State private var showingFinalDeleteConfirmation: Bool = false
    @State private var showingSetupSheet: Bool = false
    @State private var encryptedUrlData: Data?

    private var operationInProgress: Bool {
        isRegistering || isSettingUp || isDeleting
    }

    private var settingsAreValid: Bool {
        !email.trimmingCharacters(in: .whitespacesAndNewlines).isEmpty &&
        !syncUrl.trimmingCharacters(in: .whitespacesAndNewlines).isEmpty
    }

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
                    Button(action: registerEmail) {
                        if isRegistering {
                            HStack {
                                ProgressView()
                                Text("Registering...")
                            }
                        } else {
                            Text("Register New Email")
                        }
                    }
                    .disabled(!settingsAreValid || operationInProgress)

                    Button(action: setupNewClient) {
                        if isSettingUp {
                            HStack {
                                ProgressView()
                                    .progressViewStyle(CircularProgressViewStyle())
                                Text("Setting Up...")
                            }
                        } else {
                            Text("Setup New Client")
                        }
                    }
                    .disabled(!settingsAreValid || operationInProgress)

                    Button(role: .destructive) {
                        showingDeleteConfirmation = true
                    } label: {
                        if isDeleting {
                            HStack {
                                ProgressView()
                                Text("Deleting Account...")
                            }
                        } else {
                            Text("Delete Account")
                        }
                    }
                    .disabled(!settingsAreValid || operationInProgress)
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
            .alert("Delete Account", isPresented: $showingDeleteConfirmation) {
                Button("Cancel", role: .cancel) { }
                Button("Delete", role: .destructive) {
                    showingFinalDeleteConfirmation = true
                }
            } message: {
                Text("Delete the cloud account for \(email.trimmingCharacters(in: .whitespacesAndNewlines))?\n\nThis removes the account record and encrypted remote keybox data. Local keybox files on this device are not deleted.")
            }
            .alert("Confirm Account Deletion", isPresented: $showingFinalDeleteConfirmation) {
                Button("Cancel", role: .cancel) { }
                Button("Delete", role: .destructive, action: deleteAccount)
            } message: {
                Text("This action cannot be undone. Delete \(email.trimmingCharacters(in: .whitespacesAndNewlines))?")
            }
        }
        .sheet(isPresented: $showingSetupSheet) {
            if let data = encryptedUrlData {
                NewClientSetupView(isPresented: $showingSetupSheet, encryptedUrlData: data)
                    .environmentObject(appState)
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

        isRegistering = true
        DispatchQueue.global(qos: .userInitiated).async {
            do {
                try kbFile.setSyncUrl(syncUrl)
                try kbFile.setEmail(email)

                var message: NSString?
                try kbFile.register(withMessage: &message)
                let successMsg = message as String? ?? "Register success, please check your email to activate"

                DispatchQueue.main.async {
                    isRegistering = false
                    appState.saveFile()
                    showAlert(title: "Success", message: successMsg)
                }
            } catch {
                DispatchQueue.main.async {
                    isRegistering = false
                    showAlert(title: "Error", message: "Failed to register: \(error.localizedDescription)")
                }
            }
        }
    }

    private func setupNewClient() {
        guard let kbFile = appState.kbFile else {
            showAlert(title: "Error", message: "No keybox file loaded")
            return
        }

        do {
            try kbFile.setSyncUrl(syncUrl)
            try kbFile.setEmail(email)
        } catch {
            showAlert(title: "Error", message: "Failed to save settings: \(error.localizedDescription)")
            return
        }

        isSettingUp = true

        DispatchQueue.global(qos: .userInitiated).async {
            do {
                var url: NSData?
                var message: NSString?
                try kbFile.setupNewClient(withUrl: &url, message: &message)
                
                DispatchQueue.main.async {
                    isSettingUp = false
                    appState.saveFile()
                    
                    if let urlData = url as Data? {
                        self.encryptedUrlData = urlData
                        self.showingSetupSheet = true
                    } else {
                        let msg = message as String? ?? "New client setup successfully, but no URL received."
                        showAlert(title: "Success", message: msg)
                    }
                }
            } catch {
                DispatchQueue.main.async {
                    isSettingUp = false
                    showAlert(title: "Error", message: "Failed to setup new client: \(error.localizedDescription)")
                }
            }
        }
    }

    private func showAlert(title: String, message: String) {
        alertTitle = title
        alertMessage = message
        showingAlert = true
    }

    private func deleteAccount() {
        guard let kbFile = appState.kbFile else {
            showAlert(title: "Error", message: "No keybox file loaded")
            return
        }

        let trimmedEmail = email.trimmingCharacters(in: .whitespacesAndNewlines)
        let trimmedUrl = syncUrl.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !trimmedEmail.isEmpty else {
            showAlert(title: "Error", message: "Email is required.")
            return
        }
        guard !trimmedUrl.isEmpty else {
            showAlert(title: "Error", message: "Sync server URL is required.")
            return
        }

        isDeleting = true
        DispatchQueue.global(qos: .userInitiated).async {
            do {
                try kbFile.setSyncUrl(trimmedUrl)
                try kbFile.setEmail(trimmedEmail)

                var message: NSString?
                try kbFile.deleteRemoteAccount(withMessage: &message)

                DispatchQueue.main.async {
                    isDeleting = false
                    showAlert(
                        title: "Success",
                        message: message as String? ?? "Account deleted."
                    )
                }
            } catch {
                DispatchQueue.main.async {
                    isDeleting = false
                    showAlert(
                        title: "Error",
                        message: "Account deletion failed: \(error.localizedDescription)"
                    )
                }
            }
        }
    }
}

#Preview {
    let appState = AppState()
    appState.isFileLoaded = true
    appState.isUnlocked = true

    return SyncSettingsView()
        .environmentObject(appState)
}
