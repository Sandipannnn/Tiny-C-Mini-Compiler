# Tiny C Mini Compiler (C++)

A beginner-friendly educational mini compiler for a small subset of C. It performs lexical analysis, parsing, semantic checks, generates three-address intermediate code, and interprets the validated AST to produce program output.

## Requirements
- GCC with C++17 support (MinGW-w64 on Windows works)
- A terminal / command prompt

## Files
- `main.cpp` — compiler implementation
- `input.tc` — basic sample program
- `control.tc` — sample with a `while` loop and `if-else`

## Compile

### Windows (PowerShell / CMD with g++ installed)
```powershell
g++ -std=c++17 -Wall -Wextra main.cpp -o tinyc.exe
```

### Linux / macOS
```bash
g++ -std=c++17 -Wall -Wextra main.cpp -o tinyc
```

## Run

Windows:
```powershell
.\tinyc.exe input.tc
.\tinyc.exe control.tc
```

Linux / macOS:
```bash
./tinyc input.tc
./tinyc control.tc
```

## Tiny C subset supported
- Program wrapper: `int main() { ... }`
- Types: `int`, `float`
- Declarations: `int a, b;`
- Assignment: `a = 10 + 2;`
- Operators: `+ - * / %`
- Comparisons: `< <= > >= == !=`
- `print(expression);`
- `if (condition) statement else statement`
- `while (condition) statement`
- `return expression;`
- `//` single-line comments

## Example
Input:
```c
int main() {
    int a, b;
    a = 10;
    b = 20;
    print(a + b);
    return 0;
}
```
The program prints `30`.

## Compiler phases
1. **Lexical analysis**: reads characters and makes tokens.
2. **Syntax analysis**: checks grammar and builds an Abstract Syntax Tree (AST).
3. **Semantic analysis**: verifies variables are declared and checks basic types.
4. **Intermediate code generation**: emits three-address code (TAC).
5. **Execution backend**: interprets the AST and prints the result.

## Important project limitation
This is an educational compiler with an interpreter backend, not a native machine-code compiler. It executes programs through the `tinyc` executable. Generating a standalone native executable from each Tiny C source file would require an additional backend, such as C code generation followed by invoking a C/C++ compiler, or LLVM.

The implementation intentionally uses a single program-wide symbol table. Block-local scope, functions, arrays, strings, input statements, and full C type-conversion rules are not supported.
