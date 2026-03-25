import SwiftUI
import Security

@main
struct keyboxiApp: App {
    @StateObject private var appState = AppState()
    @Environment(\.scenePhase) private var scenePhase

    init() {
        // Generate a 32-byte key and 8-byte IV using the system CSPRNG
        let key = Self.secureRandomData(count: 32)
        let iv = Self.secureRandomData(count: 8)

        // Configure the generator once
        _ = ORandomGenerator.shared().configure(withKey: key, iv: iv)

        // Optionally, also ensure the algorithm type (if your CRandomGenerator supports multiple)
        _ = ORandomGenerator.shared().configure(withType: UInt32(kSalsa20Type))
    }

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(appState)
                .onAppear {
                    appState.loadKeyboxFile()
                }
        }
        .onChange(of: scenePhase) { oldPhase, newPhase in
            switch newPhase {
            case .background:
                // App is moving to background - save the file
                print("App moving to background - saving file")
                if appState.isUnlocked {
                    appState.saveFile()
                }
            case .inactive:
                // App is becoming inactive (e.g., phone call, control center)
                print("App becoming inactive - saving file")
                if appState.isUnlocked {
                    appState.saveFile()
                }
            case .active:
                print("App is active")
            @unknown default:
                break
            }
        }
    }

    private static func secureRandomData(count: Int) -> Data {
        var data = Data(count: count)
        let result = data.withUnsafeMutableBytes { ptr in
            SecRandomCopyBytes(kSecRandomDefault, count, ptr.baseAddress!)
        }
        precondition(result == errSecSuccess, "Failed to generate secure random bytes")
        return data
    }
}

class AppState: ObservableObject {
    @Published var kbFile: OCKBFile?
    @Published var isFileLoaded: Bool = false
    @Published var loadError: String?
    @Published var unlockError: String?
    @Published var entries = PwdEntries()
    @Published var groups = PwdGroups()
    @Published var isUnlocked: Bool = false
    @Published var loadedFileData: Data?

    // UI state for creating a new file
    @Published var needsMasterPassword: Bool = false
    @Published var needsUnlockPassword: Bool = false
    @Published var pendingNewFile: Bool = false

    private let keyboxFileName = "keybox.kbx"

    func loadKeyboxFile() {
        guard let documentsPath = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first else {
            loadError = "Unable to access documents directory"
            return
        }

        let keyboxFilePath = documentsPath.appendingPathComponent(keyboxFileName)

        // Check if the file exists
        if FileManager.default.fileExists(atPath: keyboxFilePath.path) {
            // Load existing file
            do {
                // Read file data
                let fileData = try Data(contentsOf: keyboxFilePath)
                self.loadedFileData = fileData

                // Create OCKBFile instance
                let ockbFile = OCKBFile()

                // Load header first (Swift automatically converts NSError** to throws)
                try ockbFile.loadHeader(fromBuffer: fileData)

                // Store the file instance
                self.kbFile = ockbFile
                self.isFileLoaded = true

                print("Successfully loaded keybox.kbx from: \(keyboxFilePath.path)")

            } catch {
                loadError = "Failed to load keybox.kbx: \(error.localizedDescription)"
            }
        } else {
            // File doesn't exist, create new empty KBFile like MainWindow::newFile
            print("keybox.kbx not found, creating new empty KBFile")
            createNewKBFile()
        }
    }

    func createNewKBFile() {
        // Create new OCKBFile instance
        let ockbFile = OCKBFile()

        // Generate random 32-byte salt for key derivation (like g_RG.GetNextBytes(32, randomv32))
        var saltBytes = [UInt8](repeating: 0, count: 32)
        let result = SecRandomCopyBytes(kSecRandomDefault, saltBytes.count, &saltBytes)

        guard result == errSecSuccess else {
            loadError = "Failed to generate random salt"
            return
        }

        let salt = Data(saltBytes)

        // Set derivative parameters with the random salt and default rounds (60000)
        do {
            try ockbFile.setDerivativeParameters(salt, rounds: 60000)
        } catch {
            loadError = "Failed to set derivative parameters: \(error.localizedDescription)"
            return
        }

        // Keep the file instance and trigger password prompt
        self.kbFile = ockbFile
        self.isFileLoaded = true
        self.pendingNewFile = true
        self.needsMasterPassword = true

        print("Created new empty KBFile; awaiting master password")
    }

    // Called by UI after user enters a master password to finalize and save the new file
    func finalizeNewFile(withMasterPassword password: String) {
        guard let ockbFile = kbFile else {
            loadError = "Internal error: KB file not initialized"
            return
        }

        // Set the master password (handles derivation and random padding internally)
        guard ockbFile.setMasterPassword(password) else {
            loadError = "Failed to set master password"
            return
        }

        // Serialize and write to Documents/keybox.kbx
        do {
            guard let documentsPath = getDocumentsPath() else {
                loadError = "Unable to access documents directory"
                return
            }
            let fileURL = documentsPath.appendingPathComponent(keyboxFileName)

            let data = try ockbFile.serialize()
            try data.write(to: fileURL, options: .atomic)
            print("New keybox.kbx created at: \(fileURL.path)")

            // Now we have a saved file; automatically unlock and proceed to entries
            self.needsMasterPassword = false
            self.pendingNewFile = false
            self.loadPayloadAfterUnlock()
        } catch {
            loadError = "Failed to save new keybox.kbx: \(error.localizedDescription)"
        }
    }

    func loadPayloadAfterUnlock() {
        guard let ockbFile = kbFile else {
            loadError = "No keybox file loaded"
            return
        }

        // Set kbFile reference for PwdEntries
        self.entries.kbFile = ockbFile

        // Load all groups
        if let groupsArray = ockbFile.getAllGroups() {
            self.groups = PwdGroups(wrapping: groupsArray)
        }

        // Load all entries from OCKBFile
        if let entriesArray = ockbFile.getAllEntries() {
            // Convert OPwdEntry objects to PwdEntry and populate entries
            self.entries.entries = entriesArray.map { oPwdEntry in
                PwdEntry(backing: oPwdEntry, kbFile: ockbFile)
            }
        }

        self.isUnlocked = true
        print("Loaded \(self.entries.entries.count) entries and \(self.groups.groups.count) groups")
    }

    func unlock(withMasterPassword password: String) {
        guard let documentsPath = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first else {
            loadError = "Unable to access documents directory"
            return
        }

        let keyboxFilePath = documentsPath.appendingPathComponent(keyboxFileName)

        // Reload the latest file data and header first, like MainWindow::OpenFile
        let fileData: Data
        do {
            fileData = try Data(contentsOf: keyboxFilePath)
            self.loadedFileData = fileData

            // Create or reuse OCKBFile instance
            let ockbFile = self.kbFile ?? OCKBFile()
            try ockbFile.loadHeader(fromBuffer: fileData)
            self.kbFile = ockbFile
            self.isFileLoaded = true
        } catch {
            loadError = "Failed to reload keybox file: \(error.localizedDescription)"
            return
        }

        guard let ockbFile = kbFile else {
            loadError = "Internal error: KB file not initialized"
            return
        }

        // Set the master password (handles derivation and random padding internally)
        guard ockbFile.setMasterPassword(password) else {
            loadError = "Failed to derive key from password"
            return
        }

        do {
            // Load payload from the same data buffer
            try ockbFile.loadPayload(fromBuffer: fileData)

            // Successfully loaded payload
            self.needsUnlockPassword = false
            self.unlockError = nil
            self.loadPayloadAfterUnlock()
        } catch {
            // Password might be wrong or file is corrupted
            self.unlockError = "password is incorrect"
            print("Unlock failed: \(error.localizedDescription)")
        }
    }

    func getDocumentsPath() -> URL? {
        return FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first
    }

    func saveFile() {
        guard let ockbFile = kbFile else {
            loadError = "No keybox file loaded"
            return
        }

        do {
            guard let documentsPath = getDocumentsPath() else {
                loadError = "Unable to access documents directory"
                return
            }
            let fileURL = documentsPath.appendingPathComponent(keyboxFileName)

            let data = try ockbFile.serialize()
            try data.write(to: fileURL, options: .atomic)
            print("Keybox file saved at: \(fileURL.path)")
        } catch {
            loadError = "Failed to save keybox.kbx: \(error.localizedDescription)"
        }
    }
}
