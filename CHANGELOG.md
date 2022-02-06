# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project tries to adhere to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0]
### Added
- You no longer need to forward declare functions, in fact the `fndecl` keyword has been removed.
- You are no longer required to put a semicolon after a struct or enum definitoin. Doing so will give a compiler warning, but not a hard error.

### Changed
- Local and global variables are now declared `let/global ident: type = value;`
- Struct members and function parameters are now declared `ident: type,` instead of `type ident;`.
- Functions are now defined `fn name(params...) -> ret_type { stmts... }`, or `fn name(params...) { stmts... }` (for void functions).

## [0.0.3]
### Fixed
- Enums can't be forward declared in C, so they need to be treated differently in the generated C code. This lead to the compiler not being able to compile itself. This has been fixed now.

## [0.0.2]
### Added
- Automatic Forward Declarations: Allow any ordering of struct/union/enum and function definitions! You no longer have to worry about ordering because everything will be declared automatically at the top of the generated C file!
- Function pointers are now supported! A pointer to a function that takes an int and returns a char is declared like this: `fn(int) -> char my_fn_ptr`
    - Function pointers can be passed to functions, returned from functions, and stored in structs! This was difficult to do because the C syntax for function pointers is hideous.

## [0.0.1]
### Added
- Parasitic C files will note be compiled more than once even if they are `includ`ed more than once.
- Files can now just end in `.parac` since we have bootstrapped the compiler!
- Include guards are generated for all `.parac` files. This allows files to be `includ`ed multiple times from multiple files.
- Lexing and parsing error messages have been greatly improved. Filenames and line numbers are now mentioned so you can jump directly to the source of the error.

## [0.0.0] - 2022-01-11
### Added
- First bootstrapped version

## [Unreleased]