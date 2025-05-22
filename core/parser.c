#include <stdio.h>
#include <string.h>
#include "lexer.h" // declare tokens[], tokenCount from lexer
#include <stdlib.h>

extern Token tokens[];
extern int tokenCount;

int current = 0;

Token peek() {
    return tokens[current];
}

Token advance() {
    return tokens[current++];
}

int match(const char *type, const char *value) {
    if (current >= tokenCount) return 0;
    Token tok = tokens[current];
    if ((type == NULL || strcmp(tok.type, type) == 0) &&
        (value == NULL || strcmp(tok.value, value) == 0)) {
        current++;
        return 1;
    }
    return 0;
}

void error(const char *message) {
    printf("Syntax Error at line %d: %s\n", tokens[current].line, message);
    exit(1);
}

// Recursive Descent Parser Functions (Core Parser):

void parseProgram();
void parseFunctionList();
void parseFunction();
void parseParameterList();
void parseStatementList();
void parseStatement();
void parseDeclaration();
void parseAssignment();
void parseReturnStmt();
void parseIfStmt();
void parseWhileStmt();
void parseExpression();
void expect(const char *type, const char *value);

void expect(const char *type, const char *value) {
    if (!match(type, value)) {
        char msg[100];
        sprintf(msg, "Expected %s '%s'", type, value);
        error(msg);
    }
}

void parseProgram() {
    // Skip preprocessor directives
    while (current < tokenCount && strcmp(peek().type, "PREPROCESSOR") == 0) {
        advance();  // Skip it
    }

    parseFunctionList();

    if (current < tokenCount) {
        error("Unexpected tokens after program end");
    }

    printf("Parsing successful. Program is syntactically correct.\n");
}

void parseFunctionList() {
    while (current < tokenCount) {
        parseFunction();
    }
}

void parseFunction() {
    if (!match("KEYWORD", NULL)) error("Expected return type (int, void, etc)");
    if (!match("IDENTIFIER", NULL)) error("Expected function name");
    expect("SYMBOL", "(");
    parseParameterList();
    expect("SYMBOL", ")");
    expect("SYMBOL", "{");
    parseStatementList();
    expect("SYMBOL", "}");
}

void parseParameterList() {
    if (match("KEYWORD", NULL)) {
        if (!match("IDENTIFIER", NULL)) error("Expected parameter name");
        while (match("SYMBOL", ",")) {
            if (!match("KEYWORD", NULL)) error("Expected parameter type after comma");
            if (!match("IDENTIFIER", NULL)) error("Expected parameter name");
        }
    }
    // else epsilon
}

void parseStatementList() {
    while (current < tokenCount && strcmp(peek().value, "}") != 0) {
        parseStatement();
    }
}

void parseStatement() {
    if (strcmp(peek().type, "KEYWORD") == 0) {
        if (strcmp(peek().value, "int") == 0 || strcmp(peek().value, "float") == 0 ||
            strcmp(peek().value, "char") == 0 || strcmp(peek().value, "double") == 0) {
            parseDeclaration();
        } else if (strcmp(peek().value, "return") == 0) {
            parseReturnStmt();
        } else if (strcmp(peek().value, "if") == 0) {
            parseIfStmt();
        } else if (strcmp(peek().value, "while") == 0) {
            parseWhileStmt();
        } else {
            error("Unknown keyword in statement");
        }
    } else if (strcmp(peek().type, "IDENTIFIER") == 0) {
        parseAssignment();
    } else {
        error("Invalid start of statement");
    }
}

void parseDeclaration() {
    advance(); // type
    if (!match("IDENTIFIER", NULL)) error("Expected variable name");
    expect("SYMBOL", ";");
}

void parseAssignment() {
    advance(); // variable
    expect("OPERATOR", "=");
    parseExpression();
    expect("SYMBOL", ";");
}

void parseReturnStmt() {
    advance(); // return
    parseExpression();
    expect("SYMBOL", ";");
}

void parseIfStmt() {
    advance(); // if
    expect("SYMBOL", "(");
    parseExpression();
    expect("SYMBOL", ")");
    expect("SYMBOL", "{");
    parseStatementList();
    expect("SYMBOL", "}");
    if (match("KEYWORD", "else")) {
        expect("SYMBOL", "{");
        parseStatementList();
        expect("SYMBOL", "}");
    }
}

void parseWhileStmt() {
    advance(); // while
    expect("SYMBOL", "(");
    parseExpression();
    expect("SYMBOL", ")");
    expect("SYMBOL", "{");
    parseStatementList();
    expect("SYMBOL", "}");
}

void parseExpression() {
    if (match("IDENTIFIER", NULL) || match("NUMBER", NULL)) {
        while (match("OPERATOR", NULL)) {
            if (!(match("IDENTIFIER", NULL) || match("NUMBER", NULL)))
                error("Expected identifier or number after operator");
        }
    } else {
        error("Expected identifier or number in expression");
    }
}

// Main Fn

int main() {
    char filename[300];

    printf("Enter full path to source code file (use quotes if it contains spaces):\n> ");
    fgets(filename, sizeof(filename), stdin);

    filename[strcspn(filename, "\n")] = '\0';

    int len = strlen(filename);
    if (filename[0] == '"' && filename[len - 1] == '"') {
        memmove(filename, filename + 1, len - 2);
        filename[len - 2] = '\0';
    }

    printf("Opening file: %s\n", filename);

    if (!analyze(filename)) {
        printf("Error: Could not open file %s\n", filename);
        return 1;
    }

    printTokens("tokens.txt");

    printf("\nTokens saved in tokens.txt\n");

    parseProgram();
    printf("Parsing Complete");

    return 0;
}
// TO RUN : gcc parser.c lexer.c -o parser (Together)