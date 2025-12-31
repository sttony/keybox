import SwiftUI
import Security

@main
struct keyboxiApp: App {
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
