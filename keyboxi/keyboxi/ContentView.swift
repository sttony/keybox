import SwiftUI

struct ContentView: View {
    @EnvironmentObject var appState: AppState
    @State private var isMenuOpen: Bool = false
    @State private var showingSyncSettings: Bool = false
    @State private var showingChangePassword: Bool = false
    @State private var masterPassword: String = ""
    @State private var newMasterPassword: String = ""
    @State private var confirmMasterPassword: String = ""
    @State private var passwordError: String?
    @State private var showingSyncAlert: Bool = false
    @State private var syncAlertTitle: String = ""
    @State private var syncAlertMessage: String = ""

    var body: some View {
        ZStack {
            // Main Content
            NavigationStack {
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
                        UnlockView()
                    } else {
                        PwdEntryListView(entries: appState.entries, groups: appState.groups)
                    }
                }
                .toolbar {
                    ToolbarItem(placement: .navigationBarLeading) {
                        Button {
                            withAnimation(.spring()) {
                                isMenuOpen = true
                            }
                        } label: {
                            Image(systemName: "line.3.horizontal")
                        }
                    }
                }
            }
            .disabled(isMenuOpen)

            // Dimming Overlay
            if isMenuOpen {
                Color.black.opacity(0.3)
                    .ignoresSafeArea()
                    .onTapGesture {
                        withAnimation(.spring()) {
                            isMenuOpen = false
                        }
                    }
            }

            // Side Menu Overlay
            HStack {
                if isMenuOpen {
                    SideMenuView(
                        isMenuOpen: $isMenuOpen,
                        showingSyncSettings: $showingSyncSettings,
                        showingChangePassword: $showingChangePassword,
                        onSyncNow: performSync
                    )
                    .transition(.move(edge: .leading))
                    .zIndex(1)
                }
                Spacer()
            }
            .ignoresSafeArea()

            // Sync progress overlay
            if appState.isSyncing {
                Color.black.opacity(0.4)
                    .ignoresSafeArea()
                VStack(spacing: 16) {
                    ProgressView()
                        .scaleEffect(1.5)
                        .tint(.white)
                    Text("Syncing...")
                        .foregroundColor(.white)
                        .font(.headline)
                }
            }
        }
        .alert(syncAlertTitle, isPresented: $showingSyncAlert) {
            Button("OK", role: .cancel) { }
        } message: {
            Text(syncAlertMessage)
        }
        .sheet(isPresented: $showingSyncSettings) {
            SyncSettingsView()
                .environmentObject(appState)
        }
        .sheet(isPresented: $showingChangePassword) {
            ChangePasswordView()
                .environmentObject(appState)
        }
    }

    private func performSync() {
        appState.syncNow { result in
            switch result {
            case .success(let message):
                syncAlertTitle = "Sync Successful"
                syncAlertMessage = message
            case .failure(let error):
                syncAlertTitle = "Sync Failed"
                syncAlertMessage = error.localizedDescription
            }
            showingSyncAlert = true
        }
    }
}

struct SideMenuView: View {
    @EnvironmentObject var appState: AppState
    @Binding var isMenuOpen: Bool
    @Binding var showingSyncSettings: Bool
    @Binding var showingChangePassword: Bool
    var onSyncNow: () -> Void

    var body: some View {
        VStack(alignment: .leading, spacing: 20) {
            Text("Menu")
                .font(.largeTitle)
                .bold()
                .padding(.top, 60)
                .padding(.leading)

            Divider()
                .padding(.horizontal)

            Button("Settings") {
                isMenuOpen = false
            }
            .padding(.leading)

            Button {
                isMenuOpen = false
                onSyncNow()
            } label: {
                HStack(spacing: 8) {
                    if appState.isSyncing {
                        ProgressView()
                            .progressViewStyle(CircularProgressViewStyle())
                            .scaleEffect(0.8)
                    }
                    Text(appState.isSyncing ? "Syncing..." : "Sync Now")
                }
            }
            .padding(.leading)
            .disabled(!appState.isUnlocked || appState.isSyncing)
            .opacity(appState.isUnlocked && !appState.isSyncing ? 1.0 : 0.5)

            Button("Sync Settings") {
                isMenuOpen = false
                showingSyncSettings = true
            }
            .padding(.leading)
            .disabled(!appState.isUnlocked)
            .opacity(appState.isUnlocked ? 1.0 : 0.5)

            Button("Security") {
                isMenuOpen = false
                showingChangePassword = true
            }
            .padding(.leading)
            .disabled(!appState.isUnlocked)
            .opacity(appState.isUnlocked ? 1.0 : 0.5)

            Spacer()
        }
        .frame(width: 280)
        .background(Color(UIColor.systemBackground))
    }
}

#Preview {
    let appState = AppState()

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
