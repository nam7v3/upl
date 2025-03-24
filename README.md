# UPL

A compiler built using for UPL (UET Programming Language

# UPL's CFG
```
Program    -> "begin" StmtList "end"
StmtList   -> Stmt ";" StmtList | e
Stmt       -> DeclStmt 
           | AssignStmt
           | IfStmt
           | WhileStmt
           | PrintStmt
DeclStmt   -> "int" ID OptInit 
           | "bool" ID OptInit
OptInit    -> "=" Expr | e
AssignStmt -> ID "=" Expr
IfStmt     -> "if" Expr "then" Block OptElse
OptElse    -> "else" Block | e
WhileStmt  -> "do" Block "while" "(" Expr ")"
PrintStmt  -> "print" "(" Expr ")"
Block      -> "{" StmtList "}"
Expr       -> RelExpr
RelExpr    -> AddExpr RelOp AddExpr | AddExpr
RelOp      -> ">" | ">=" | "=="
AddExpr    -> MulExpr "+" AddExpr | MulExpr
MulExpr    -> Factor "*" MulExpr | Factor
Factor     -> "(" Expr ")" 
           | ID
           | NUMBER
ID         -> [a-z]+[0-9]*
NUMBER     -> [0-9]+
```
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

## Running the Lexer
Pass an input file to the lexer:
```sh
./upl-lexer test.upl  # output to stdout
./upl-lexer -o test.out test.upl  # output to test.out
./upl-lexer -b flex test.upl  # use the flex backend
./upl-lexer -b handmade test.upl  # use the handmade backend
```
Type input and press `Ctrl+D` (Linux/macOS) or `Ctrl+Z` (Windows) to end input.
