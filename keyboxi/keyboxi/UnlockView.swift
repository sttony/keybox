

import SwiftUI

struct UnlockView: View {
    @EnvironmentObject var appState: AppState
    @State private var masterPassword: String = ""
    @State private var newMasterPassword: String = ""
    @State private var confirmMasterPassword: String = ""
    @State private var passwordError: String?

    var body: some View {
        VStack(spacing: 20) {
            Image(systemName: "lock.fill")
                .font(.system(size: 50))
                .foregroundColor(.blue)

            Text("Keybox Loaded")
                .font(.headline)

            if !appState.pendingNewFile {
                Button("unlock") {
                    appState.unlockError = nil
                    appState.needsUnlockPassword = true
                }
                .buttonStyle(.borderedProminent)
            }
        }
        .sheet(isPresented: $appState.needsUnlockPassword) {
            unlockPasswordSheet()
        }
        .sheet(isPresented: $appState.needsMasterPassword) {
            createPasswordSheet()
        }
    }

    private func unlockPasswordSheet() -> some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Unlock Keybox")
                .font(.headline)
            SecureField("Enter master password", text: $masterPassword)
            if let err = appState.unlockError {
                Text(err)
                    .foregroundColor(.red)
                    .font(.caption)
            } else if let err = passwordError {
                Text(err)
                    .foregroundColor(.red)
                    .font(.caption)
            }
            HStack {
                Spacer()
                Button("Unlock") {
                    guard !masterPassword.isEmpty else {
                        passwordError = "Password cannot be empty"
                        appState.unlockError = nil
                        return
                    }
                    passwordError = nil
                    appState.unlock(withMasterPassword: masterPassword)
                    masterPassword = ""
                }
                .buttonStyle(.borderedProminent)
            }
        }
        .padding()
        .presentationDetents([.medium])
    }

    private func createPasswordSheet() -> some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Create Master Password")
                .font(.headline)
            SecureField("Enter master password", text: $newMasterPassword)
            SecureField("Confirm master password", text: $confirmMasterPassword)
            if let err = passwordError {
                Text(err)
                    .foregroundColor(.red)
                    .font(.caption)
            }
            HStack {
                Spacer()
                Button("Create") {
                    guard !newMasterPassword.isEmpty else {
                        passwordError = "Password cannot be empty"
                        return
                    }
                    guard newMasterPassword == confirmMasterPassword else {
                        passwordError = "Passwords do not match"
                        return
                    }
                    passwordError = nil
                    appState.finalizeNewFile(withMasterPassword: newMasterPassword)
                    newMasterPassword = ""
                    confirmMasterPassword = ""
                }
                .buttonStyle(.borderedProminent)
            }
        }
        .padding()
        .presentationDetents([.medium])
    }
}

#Preview {
    let appState = AppState()
    appState.isFileLoaded = true
    appState.isUnlocked = false

    return UnlockView()
        .environmentObject(appState)
}
