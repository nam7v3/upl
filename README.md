# Flex Lexer Project

A lexical analyzer built using for UPL (UET Programming Language

## Dependency
Ensure you have Flex installed. On most Linux distributions, you can install it via:
```sh
sudo apt-get install flex   # Debian-based
sudo dnf install flex       # Fedora
brew install flex           # macOS (Homebrew)
```

## Building
To compile the lexer:
```sh
make
./upl-lexer
```

## ▶️ Running the Lexer
Pass an input file to the lexer:
```sh
./upl-lexer test.upl  # output to stdout
./upl-lexer -o test.out test.upl  # output to test.out
./upl-lexer -b flex test.upl  # use the flex backend
./upl-lexer -b handmade test.upl  # use the handmade backend
```
Type input and press `Ctrl+D` (Linux/macOS) or `Ctrl+Z` (Windows) to end input.
