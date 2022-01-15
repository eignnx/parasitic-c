# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project tries to adhere to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
## [0.0.2]
### Added
- Automatic Forward Declarations: Allow any ordering of struct/union/enum and function definitions! You no longer have to worry about ordering because everything will be declared automatically at the top of the generated C file!

## [0.0.1]
### Added
- Parasitic C files will note be compiled more than once even if they are `includ`ed more than once.
- Files can now just end in `.parac` since we have bootstrapped the compiler!
- Include guards are generated for all `.parac` files. This allows files to be `includ`ed multiple times from multiple files.
- Lexing and parsing error messages have been greatly improved. Filenames and line numbers are now mentioned so you can jump directly to the source of the error.

## [0.0.0] - 2022-01-11
### Added
- First bootstrapped version