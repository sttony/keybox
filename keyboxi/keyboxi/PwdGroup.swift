//
//  PwdGroups.swift
//  keyboxi
//

import SwiftUI
import Foundation

extension OPwdGroup: Identifiable {
    public var id: String {
        getID()
    }
}

// A SwiftUI-friendly wrapper for OPwdGroup
final class PwdGroup: Identifiable, Hashable, ObservableObject {
    // Published properties for UI bindings
    @Published var name: String

    // Underlying Objective-C/C++ backed model
    let backing: OPwdGroup

    // Identifiable conformance
    var id: String { backing.getID() }

    // Designated initializer using an existing backing object
    init(backing: OPwdGroup) {
        self.backing = backing
        self.name = backing.getName()
    }

    // Convenience initializer to create a new group with a name
    convenience init(name: String = "") {
        let obj = OPwdGroup(name: name)
        self.init(backing: obj)
    }

    // Hashable & Equatable via stable backing ID
    static func == (lhs: PwdGroup, rhs: PwdGroup) -> Bool { lhs.backing.getID() == rhs.backing.getID() }
    func hash(into hasher: inout Hasher) { hasher.combine(id) }

    // Persist current state back into the backing object
    func save() {
        backing.setName(name)
    }
}

final class PwdGroups: ObservableObject, RandomAccessCollection {
    @Published var groups: [PwdGroup] = []

    init() {
        // Initialize with empty groups or load from storage
    }

    // Convenience init to wrap existing OPwdGroup objects if needed
    convenience init(wrapping backingGroups: [OPwdGroup]) {
        self.init()
        self.groups = backingGroups.map { PwdGroup(backing: $0) }
    }

    func addNewGroup() -> PwdGroup {
        let newGroup = PwdGroup()
        groups.append(newGroup)
        return newGroup
    }

    func deleteGroup(_ group: PwdGroup) {
        if let index = groups.firstIndex(where: { $0.id == group.id }) {
            groups.remove(at: index)
        }
    }

    var first: PwdGroup? {
        groups.first
    }

    // MARK: - RandomAccessCollection conformance
    typealias Index = Int
    typealias Element = PwdGroup

    var startIndex: Int {
        groups.startIndex
    }

    var endIndex: Int {
        groups.endIndex
    }

    subscript(position: Int) -> PwdGroup {
        groups[position]
    }

//    func persistAll() {
//        for g in groups { g.save() }
//    }
}
