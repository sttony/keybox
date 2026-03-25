
final class PwdEntry: Identifiable, Hashable, ObservableObject{

    @Published var title: String
    @Published var url: String
    @Published var userName: String
    @Published var password: String
    @Published var note: String
    @Published var groupId: String // Group ID as a string
    let backing: OPwdEntry
    weak var kbFile: OCKBFile?

    init(backing: OPwdEntry = OPwdEntry(), kbFile: OCKBFile? = nil) {
        self.backing = backing
        self.kbFile = kbFile

        self.title = backing.getTitle();
        self.url = backing.getUrl();
        self.userName = backing.getUsername();
        self.password = backing.getPassword();
        self.note = backing.getNote();
        self.groupId = backing.getGroupUUID().uuidString
    }

    static func == (lhs: PwdEntry, rhs: PwdEntry) -> Bool { lhs.backing.getID() == rhs.backing.getID() }
    func hash(into hasher: inout Hasher) { hasher.combine(id) }

    func save() {
        backing.setTitle(title)
        backing.setUrl(url)
        backing.setUsername(userName)
        backing.setPassword(password, onePad: Data()) // Resetting pad for simplicity
        backing.setNote(note, onePad: Data())
        if let uuid = UUID(uuidString: groupId) {
            backing.setGroupUUID((uuid as NSUUID) as UUID)
        } else {
            backing.setGroupUUID(NSUUID.init() as UUID) // Default group if invalid ID
        }

        // Add or update entry in CKBFile
        if let kbFile = kbFile {
            do {
                try kbFile.update(backing)
            } catch {
                print("Failed to update entry in CKBFile: \(error)")
            }
        }
    }

}

