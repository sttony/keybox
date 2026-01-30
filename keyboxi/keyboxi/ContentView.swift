

import SwiftUI

struct ContentView: View {
    @EnvironmentObject var appState: AppState

    var body: some View {
        Group {
            if let error = appState.loadError {
                VStack(spacing: 20) {
                    Image(systemName: "exclamationmark.triangle")
                        .font(.system(size: 50))
                        .foregroundColor(.orange)

                    Text("Error Loading Keybox")
                        .font(.headline)

                    Text(error)
                        .font(.subheadline)
                        .foregroundColor(.secondary)
                        .multilineTextAlignment(.center)
                        .padding()
                }
            } else if !appState.isFileLoaded {
                VStack(spacing: 20) {
                    ProgressView()
                        .scaleEffect(1.5)

                    Text("Loading keybox.kbx...")
                        .font(.headline)
                }
            } else if !appState.isUnlocked {
                VStack(spacing: 20) {
                    Image(systemName: "lock.fill")
                        .font(.system(size: 50))
                        .foregroundColor(.blue)

                    Text("Keybox Loaded")
                        .font(.headline)

                    Button("Unlock with Test Password") {
                        // For testing: simulate unlock
                        appState.loadPayloadAfterUnlock()
                    }
                    .buttonStyle(.borderedProminent)
                }
            } else {
                PwdEntryListView(entries: appState.entries, groups: appState.groups)
            }
        }
    }
}

#Preview {
    let appState = AppState()

    // Add test data for preview
    let entry1 = appState.entries.addNew()
    entry1.title = "Gmail"
    entry1.url = "https://mail.google.com"
    entry1.userName = "user@gmail.com"

    let entry2 = appState.entries.addNew()
    entry2.title = "GitHub"
    entry2.url = "https://github.com"
    entry2.userName = "john_doe"

    appState.isFileLoaded = true
    appState.isUnlocked = true

    return ContentView()
        .environmentObject(appState)
}
