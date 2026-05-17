
import SwiftUI

struct NewClientSetupView: View {
    @EnvironmentObject var appState: AppState
    @Binding var isPresented: Bool
    let encryptedUrlData: Data
    
    @State private var clientCode: String = ""
    @State private var isProcessing: Bool = false
    @State private var errorMessage: String?
    @State private var showingPasswordPrompt: Bool = false
    @State private var masterPassword: String = ""
    @State private var downloadedData: Data?
    
    var body: some View {
        NavigationView {
            VStack(spacing: 20) {
                Text("Enter the 96-character client setup code from your other device.")
                    .font(.subheadline)
                    .foregroundColor(.secondary)
                    .multilineTextAlignment(.center)
                    .padding()
                
                TextField("Setup Code", text: $clientCode, axis: .vertical)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                    .lineLimit(3...5)
                    .autocapitalization(.none)
                    .disableAutocorrection(true)
                    .padding(.horizontal)
                
                if let error = errorMessage {
                    Text(error)
                        .foregroundColor(.red)
                        .font(.caption)
                        .padding(.horizontal)
                }
                
                if isProcessing {
                    ProgressView("Processing...")
                } else {
                    Button("Finalize Setup") {
                        processSetupCode()
                    }
                    .buttonStyle(.borderedProminent)
                    .disabled(clientCode.count < 96)
                }
                
                Spacer()
            }
            .navigationTitle("Setup New Client")
            .navigationBarItems(trailing: Button("Cancel") {
                isPresented = false
            })
            .sheet(isPresented: $showingPasswordPrompt) {
                VStack(spacing: 20) {
                    Text("Enter Master Password")
                        .font(.headline)
                    
                    Text("Enter the master password for the downloaded keybox file.")
                        .font(.subheadline)
                        .foregroundColor(.secondary)
                        .multilineTextAlignment(.center)
                    
                    SecureField("Master Password", text: $masterPassword)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .padding(.horizontal)
                    
                    Button("Unlock & Load") {
                        unlockAndLoad()
                    }
                    .buttonStyle(.borderedProminent)
                    .disabled(masterPassword.isEmpty)
                    
                    Button("Cancel") {
                        showingPasswordPrompt = false
                    }
                    .foregroundColor(.red)
                }
                .padding()
            }
        }
    }
    
    private func processSetupCode() {
        // Normalize code
        var raw = clientCode.replacingOccurrences(of: " ", with: "")
        raw = raw.replacingOccurrences(of: "-", with: "")
        raw = raw.replacingOccurrences(of: "\n", with: "")
        
        let cipher = OCipherEngine()
        guard let codeData = cipher.dataFromHexString(raw), codeData.count == 48 else {
            errorMessage = "Invalid code. Must be 96 hex characters."
            return
        }
        
        let key = codeData.subdata(in: 0..<32)
        let iv = codeData.subdata(in: 32..<48)
        
        isProcessing = true
        errorMessage = nil
        
        do {
            let decryptedUrlData = try cipher.decryptData(encryptedUrlData, key: key, iv: iv)
            guard let urlString = String(data: decryptedUrlData, encoding: .utf8) else {
                errorMessage = "Failed to decode URL from decrypted data."
                isProcessing = false
                return
            }
            
            guard let url = URL(string: urlString.trimmingCharacters(in: .controlCharacters)) else {
                errorMessage = "Invalid download URL."
                isProcessing = false
                return
            }
            
            // Download the file
            URLSession.shared.dataTask(with: url) { data, response, error in
                DispatchQueue.main.async {
                    if let error = error {
                        self.errorMessage = "Download failed: \(error.localizedDescription)"
                        self.isProcessing = false
                        return
                    }
                    
                    guard let httpResponse = response as? HTTPURLResponse, httpResponse.statusCode == 200, let data = data else {
                        self.errorMessage = "Download failed (invalid response)."
                        self.isProcessing = false
                        return
                    }
                    
                    self.downloadedData = data
                    self.isProcessing = false
                    self.showingPasswordPrompt = true
                }
            }.resume()
            
        } catch {
            errorMessage = "Decryption failed: \(error.localizedDescription)"
            isProcessing = false
        }
    }
    
    private func unlockAndLoad() {
        guard let data = downloadedData else { return }
        
        // Use AppState to load the file
        appState.loadDownloadedKeybox(data: data, password: masterPassword) { success in
            if success {
                isPresented = false
            } else {
                errorMessage = "Failed to unlock keybox. Incorrect password?"
                showingPasswordPrompt = false
            }
        }
    }
}
