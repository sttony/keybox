import SwiftUI

struct ContentView: View {
    @EnvironmentObject var appState: AppState
    @State private var isMenuOpen: Bool = false
    @State private var masterPassword: String = ""
    @State private var newMasterPassword: String = ""
    @State private var confirmMasterPassword: String = ""
    @State private var passwordError: String?

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
            .disabled(isMenuOpen) // Disable interaction with main content when menu is open

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
                    SideMenuView(isMenuOpen: $isMenuOpen)
                        .transition(.move(edge: .leading))
                        .zIndex(1)
                }
                Spacer()
            }
            .ignoresSafeArea()
        }
    }
}

// A simple implementation of the Side Menu for now
struct SideMenuView: View {
    @Binding var isMenuOpen: Bool

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
            c
            Button("Sync Now") {
                isMenuOpen = false
            }
            .padding(.leading)

            Spacer()

            
            .padding(.leading)
            .padding(.bottom)
        }
        .frame(width: 280)
        .background(Color(UIColor.systemBackground))
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
