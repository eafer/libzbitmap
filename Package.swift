// swift-tools-version:5.0
// SPDX-License-Identifier: MIT

import PackageDescription

let package = Package(
    name: "libzbitmap",
    products: [
        .library(name: "libzbitmap", targets: ["libzbitmap"])
    ],
    targets: [
        .target(
            name: "libzbitmap",
            path: ".",
            exclude: ["test", "LICENSE", "Makefile", "README.rst"],
            sources: ["libzbitmap.c"],
            publicHeadersPath: "include"
        )
    ],
    cLanguageStandard: .gnu99
)
