import SwiftUI
import Security

@main
struct keyboxiApp: App {
    @StateObject private var appState = AppState()

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
    @Published var entries = PwdEntries()
    @Published var groups = PwdGroups()
    @Published var isUnlocked: Bool = false

    // UI state for creating a new file
    @Published var needsMasterPassword: Bool = false
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

        // Convert password to UTF-8 bytes and generate a 32-byte one-time pad (for derived key size)
        let passwordData = Data(password.utf8)
        var onePad = Data(count: 32)
        let genResult = onePad.withUnsafeMutableBytes { ptr in
            SecRandomCopyBytes(kSecRandomDefault, 32, ptr.baseAddress!)
        }
        guard genResult == errSecSuccess else {
            loadError = "Failed to generate one-time pad"
            return
        }

        // Set the master key (now handles derivation internally)
        ockbFile.setMasterKey(passwordData, onePad: onePad)

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
            // Now we have a saved file; remain locked until user unlocks explicitly in UI flow
            self.needsMasterPassword = false
            self.pendingNewFile = false
        } catch {
            loadError = "Failed to save new keybox.kbx: \(error.localizedDescription)"
        }
    }

    func loadPayloadAfterUnlock() {
        guard let ockbFile = kbFile else {
            loadError = "No keybox file loaded"
            return
        }

        // Load all groups
        if let groupsArray = ockbFile.getAllGroups() {
            self.groups = PwdGroups(wrapping: groupsArray)
        }

        // Load all entries from OCKBFile
        if let entriesArray = ockbFile.getAllEntries() {
            // Convert OPwdEntry objects to PwdEntry and populate entries
            self.entries.entries = entriesArray.map { oPwdEntry in
                PwdEntry(backing: oPwdEntry)
            }
        }

        self.isUnlocked = true
        print("Loaded \(self.entries.entries.count) entries and \(self.groups.groups.count) groups")
    }

    func getDocumentsPath() -> URL? {
        return FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first
    }
}
