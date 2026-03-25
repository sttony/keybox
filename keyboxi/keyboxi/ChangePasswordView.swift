//
//  ChangePasswordView.swift
//  keyboxi
//

import SwiftUI

struct ChangePasswordView: View {
    @EnvironmentObject var appState: AppState
    @Environment(\.dismiss) var dismiss

    @State private var newPassword: String = ""
    @State private var confirmPassword: String = ""
    @State private var errorMessage: String?
    @State private var isChanging: Bool = false
    @State private var showingAlert = false
    @State private var alertTitle = ""
    @State private var alertMessage = ""

    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("New Master Password")) {
                    SecureField("New Password", text: $newPassword)
                    SecureField("Confirm Password", text: $confirmPassword)
                }

                if let error = errorMessage {
                    Section {
                        Text(error)
                            .foregroundColor(.red)
                            .font(.caption)
                    }
                }

                Section {
                    Button(action: changePassword) {
                        if isChanging {
                            HStack {
                                ProgressView()
                                    .progressViewStyle(CircularProgressViewStyle())
                                Text("Changing Password...")
                            }
                        } else {
                            Text("Change Password")
                        }
                    }
                    .disabled(newPassword.isEmpty || confirmPassword.isEmpty || isChanging)
                }

                Section {
                    Text("Note: This will retrieve from remote, change the password, and push to remote server.")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
            }
            .navigationTitle("Change Password")
            .navigationBarItems(
                leading: Button("Cancel") {
                    dismiss()
                }
                .disabled(isChanging)
            )
            .alert(alertTitle, isPresented: $showingAlert) {
                Button("OK", role: .cancel) {
                    if alertTitle == "Success" {
                        dismiss()
                    }
                }
            } message: {
                Text(alertMessage)
            }
        }
    }

    private func changePassword() {
        guard !newPassword.isEmpty else {
            errorMessage = "Password cannot be empty"
            return
        }

        guard newPassword == confirmPassword else {
            errorMessage = "Passwords do not match"
            return
        }

        guard let kbFile = appState.kbFile else {
            errorMessage = "No keybox file loaded"
            return
        }

        isChanging = true
        errorMessage = nil

        // Run in background to avoid blocking UI
        DispatchQueue.global(qos: .userInitiated).async {
            do {
                try kbFile.changePassword(newPassword)

                DispatchQueue.main.async {
                    isChanging = false
                    appState.saveFile()
                    showAlert(title: "Success", message: "Password changed successfully")
                }
            } catch {
                DispatchQueue.main.async {
                    isChanging = false
                    errorMessage = "Failed to change password: \(error.localizedDescription)"
                }
            }
        }
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

    return ChangePasswordView()
        .environmentObject(appState)
}
