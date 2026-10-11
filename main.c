#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
   Simple Tiny C interpreter in C.
   Supports: int main(){...}, int variables, assignments, print(expr),
   return, + - * /, and parentheses.
   This shorter teaching version focuses on basic compiler phases.
*/

#define MAX_TOKENS 1000
#define MAX_TEXT 64
#define MAX_VARS 100

typedef struct {
    char text[MAX_TEXT];
    int line;
} Token;

Token tokens[MAX_TOKENS];
int tokenCount = 0, pos = 0, lineNo = 1;

char names[MAX_VARS][MAX_TEXT];
int values[MAX_VARS], varCount = 0;

void error(const char *msg) {
    printf("Error at line %d: %s\n", lineNo, msg);
    exit(1);
}

void addToken(const char *text, int line) {
    if (tokenCount >= MAX_TOKENS) error("too many tokens");
    strcpy(tokens[tokenCount].text, text);
    tokens[tokenCount++].line = line;
}

void tokenize(const char *src) {
    int i = 0;
    while (src[i]) {
        if (isspace((unsigned char)src[i])) {
            if (src[i] == '\n') lineNo++;
            i++;
        } else if (src[i] == '/' && src[i+1] == '/') {
            while (src[i] && src[i] != '\n') i++;
        } else if (isalpha((unsigned char)src[i]) || src[i] == '_') {
            char word[MAX_TEXT]; int j = 0;
            while (isalnum((unsigned char)src[i]) || src[i] == '_') {
                if (j < MAX_TEXT-1) word[j++] = src[i];
                i++;
            }
            word[j] = '\0';
            addToken(word, lineNo);
        } else if (isdigit((unsigned char)src[i])) {
            char number[MAX_TEXT]; int j = 0;
            while (isdigit((unsigned char)src[i])) {
                if (j < MAX_TEXT-1) number[j++] = src[i];
                i++;
            }
            number[j] = '\0';
            addToken(number, lineNo);
        } else {
            char symbol[2] = {src[i++], '\0'};
            addToken(symbol, lineNo);
        }
    }
    addToken("EOF", lineNo);
}

int match(const char *s) {
    if (strcmp(tokens[pos].text, s) == 0) { pos++; return 1; }
    return 0;
}

void expect(const char *s) {
    if (!match(s)) {
        lineNo = tokens[pos].line;
        error("unexpected token");
    }
}

int findVar(const char *name) {
    int i;
    for (i = 0; i < varCount; i++)
        if (strcmp(names[i], name) == 0) return i;
    return -1;
}

/* Expression parser: parentheses, multiplication/division, addition/subtraction */
int expression(void);

int primary(void) {
    int value;
    if (match("(")) {
        value = expression();
        expect(")");
        return value;
    }

    if (isdigit((unsigned char)tokens[pos].text[0])) {
        value = atoi(tokens[pos++].text);
        return value;
    }

    if (isalpha((unsigned char)tokens[pos].text[0]) || tokens[pos].text[0] == '_') {
        int id = findVar(tokens[pos].text);
        if (id < 0) {
            lineNo = tokens[pos].line;
            error("variable used before declaration");
        }
        pos++;
        return values[id];
    }

    lineNo = tokens[pos].line;
    error("expected number, variable, or '('");
    return 0;
}

int term(void) {
    int value = primary();
    while (strcmp(tokens[pos].text, "*") == 0 ||
           strcmp(tokens[pos].text, "/") == 0) {
        char op = tokens[pos++].text[0];
        int rhs = primary();
        if (op == '*') value *= rhs;
        else {
            if (rhs == 0) error("division by zero");
            value /= rhs;
        }
    }
    return value;
}

int expression(void) {
    int value = term();
    while (strcmp(tokens[pos].text, "+") == 0 ||
           strcmp(tokens[pos].text, "-") == 0) {
        char op = tokens[pos++].text[0];
        int rhs = term();
        if (op == '+') value += rhs;
        else value -= rhs;
    }
    return value;
}

void declaration(void) {
    expect("int");
    do {
        char name[MAX_TEXT];
        if (strlen(tokens[pos].text) >= MAX_TEXT) error("name too long");
        strcpy(name, tokens[pos++].text);

        if (!isalpha((unsigned char)name[0]) && name[0] != '_')
            error("expected variable name");
        if (findVar(name) >= 0) error("variable declared twice");
        if (varCount >= MAX_VARS) error("too many variables");

        strcpy(names[varCount], name);
        values[varCount++] = 0;
    } while (match(","));
    expect(";");
}

void statement(void) {
    if (strcmp(tokens[pos].text, "int") == 0) {
        declaration();
    } else if (match("print")) {
        expect("(");
        int value = expression();
        expect(")");
        expect(";");
        printf("%d\n", value);
    } else if (match("return")) {
        (void)expression();
        expect(";");
        /* Return value is parsed; this simple version continues to the closing brace. */
    } else {
        char name[MAX_TEXT];
        int id;

        if (!isalpha((unsigned char)tokens[pos].text[0]) &&
            tokens[pos].text[0] != '_') {
            lineNo = tokens[pos].line;
            error("expected statement");
        }

        strcpy(name, tokens[pos++].text);
        id = findVar(name);
        if (id < 0) {
            lineNo = tokens[pos-1].line;
            error("assignment to undeclared variable");
        }

        expect("=");
        values[id] = expression();
        expect(";");
    }
}

int main(int argc, char *argv[]) {
    FILE *file;
    char source[20000];
    size_t n;

    if (argc != 2) {
        printf("Usage: tinyc input.tc\n");
        return 1;
    }

    file = fopen(argv[1], "r");
    if (!file) {
        printf("Cannot open file: %s\n", argv[1]);
        return 1;
    }
    n = fread(source, 1, sizeof(source)-1, file);
    source[n] = '\0';
    fclose(file);

    tokenize(source);
    printf("=== TINY C MINI COMPILER ===\n");
    printf("[1] Lexical analysis: %d tokens\n", tokenCount-1);

    expect("int");
    expect("main");
    expect("(");
    expect(")");
    expect("{");
    printf("[2] Syntax analysis started\n");
    printf("[3] Semantic checks run during parsing\n");
    printf("[4] Executing program:\n");

    while (strcmp(tokens[pos].text, "}") != 0 &&
           strcmp(tokens[pos].text, "EOF") != 0)
        statement();

    expect("}");
    expect("EOF");
    printf("Compilation and execution completed.\n");
    return 0;
}
