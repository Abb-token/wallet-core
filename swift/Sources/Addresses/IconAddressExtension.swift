// Copyright © 2017-2019 Trust.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import Foundation

/// Icon address.
extension IconAddress: Address, Hashable {
    public static func isValid(data: Data) -> Bool {
        return data.count == 20
    }

    public var data: Data {
        return keyHash
    }

    public func hash(into hasher: inout Hasher) {
        hasher.combine(keyHash)
    }
}