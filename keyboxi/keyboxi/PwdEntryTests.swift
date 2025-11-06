// Swift file here (place in keyboxiTests target, e.g., PwdEntryTests.swift)

//import XCTest
//@testable import keyboxi
//
//final class PwdEntryTests: XCTestCase {
//
//    func test_PwdEntry_roundtrip_fields() throws {
//        // Arrange: build a backing OPwdEntry and wrap it
//        let backing = OPwdEntry()
//        let entry = PwdEntry(backing: backing)
//
//        // Test data
//        let expectedTitle = "Example Title"
//        let expectedUrl = "https://example.com"
//        let expectedUsername = "alice"
//        let expectedPassword = "s3cr3t!"
//        let expectedNote = "Some note text"
//        let groupUUID = UUID().uuidString
//
//        // Act: set fields through the Swift wrapper
//        entry.title = expectedTitle
//        entry.url = expectedUrl
//        entry.userName = expectedUsername
//        entry.password = expectedPassword
//        entry.note = expectedNote
//        entry.groupId = groupUUID
//
//        // Assert: getters return what we set
//        XCTAssertEqual(entry.title, expectedTitle)
//        XCTAssertEqual(entry.url, expectedUrl)
//        XCTAssertEqual(entry.userName, expectedUsername)
//        XCTAssertEqual(entry.password, expectedPassword)
//        XCTAssertEqual(entry.note, expectedNote)
//        XCTAssertEqual(entry.groupId, groupUUID)
//    }
//
//    func test_PwdEntry_default_init_is_empty() throws {
//        let entry = PwdEntry() // uses OPwdEntry()
//        XCTAssertEqual(entry.title, "")
//        XCTAssertEqual(entry.url, "")
//        XCTAssertEqual(entry.userName, "")
//        XCTAssertEqual(entry.password, "")
//        XCTAssertEqual(entry.note, "")
//        // groupId may be empty if not set
//        XCTAssertTrue(entry.groupId.isEmpty || UUID(uuidString: entry.groupId) != nil)
//    }
//}
