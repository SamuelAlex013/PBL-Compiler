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
void parseSwitchStmt();
void parseWhileStmt();
void parseForStmt();
void parsePrint();
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
    while (current < tokenCount && strcmp(peek().type, "PREPROCESSOR") == 0) {
        advance();
    }

    parseFunctionList();

    if (current < tokenCount) {
        error("Unexpected tokens after program end");
    }

    printf("Parsing successful. Program is syntactically correct.\n");
}

void parseFunctionList() {
    while (current < tokenCount) {
        int backup = current;

        if (match("KEYWORD", NULL)) {
            if (match("IDENTIFIER", NULL)) {
                if (match("SYMBOL", "(")) {
                    current = backup;
                    parseFunction();
                } else {
                    current = backup;
                    parseDeclaration();
                }
            } else {
                error("Expected identifier after keyword");
            }
        } else {
            error("Expected function or global declaration");
        }
    }
}

void parseFunction() {
    Token start = peek();
    if (!match("KEYWORD", NULL)) error("Expected return type (int, void, etc)");
    if (!match("IDENTIFIER", NULL)) error("Expected function name");
    expect("SYMBOL", "(");
    parseParameterList();
    expect("SYMBOL", ")");

    if (match("SYMBOL", ";")) return;

    expect("SYMBOL", "{");
    parseStatementList();
    expect("SYMBOL", "}");

    printf("Parsed function successfully at line %d\n", start.line);
}

void parseParameterList() {
    if (match("KEYWORD", NULL)) {
        if (!match("IDENTIFIER", NULL)) error("Expected parameter name");
        while (match("SYMBOL", ",")) {
            if (!match("KEYWORD", NULL)) error("Expected parameter type after comma");
            if (!match("IDENTIFIER", NULL)) error("Expected parameter name");
        }
    }
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
        } else if (strcmp(peek().value, "for") == 0) {
            parseForStmt();
        } else if (strcmp(peek().value, "printf") == 0) {
            parsePrint();
        } else if (strcmp(peek().value, "switch") == 0) {
            parseSwitchStmt();
        } else if (strcmp(peek().value, "break") == 0) {
            advance();
            expect("SYMBOL", ";");
            printf("Parsed break statement at line %d\n", peek().line);
        } else {
            error("Unknown keyword in statement");
        }
    } else if (strcmp(peek().type, "IDENTIFIER") == 0) {
        if (strcmp(peek().value, "printf") == 0) {
            parsePrint();
        } else {
            parseAssignment();
        }
    } else {
        error("Invalid start of statement");
    }
}

void parseDeclaration() {
    while (strcmp(peek().type, "KEYWORD") == 0 &&
           (strcmp(peek().value, "extern") == 0 ||
            strcmp(peek().value, "static") == 0 ||
            strcmp(peek().value, "const") == 0)) {
        advance();
    }

    if (!(match("KEYWORD", "int") || match("KEYWORD", "float") ||
          match("KEYWORD", "char") || match("KEYWORD", "double") ||
          match("KEYWORD", "void"))) {
        error("Expected type specifier like int, float, etc.");
    }

    if (!match("IDENTIFIER", NULL)) error("Expected variable name");

    if (match("SYMBOL", "[")) {
        if (!match("NUMBER", NULL)) error("Expected array size inside brackets");
        expect("SYMBOL", "]");
    }

    if (match("OPERATOR", "=")) {
        parseExpression();
    }

    expect("SYMBOL", ";");
}

void parseAssignment() {
    if (!match("IDENTIFIER", NULL)) error("Expected variable name");

    if (match("SYMBOL", "[")) {
        parseExpression();
        expect("SYMBOL", "]");
    }

    expect("OPERATOR", "=");
    parseExpression();
    expect("SYMBOL", ";");
}

void parseReturnStmt() {
    advance();
    parseExpression();
    expect("SYMBOL", ";");
}

void parseIfStmt() {
    Token start = peek();
    advance();
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

    printf("Parsed if-else statement successfully at line %d\n", start.line);
}

void parseSwitchStmt() {
    Token start = peek();
    advance();
    expect("SYMBOL", "(");
    parseExpression(); 
    expect("SYMBOL", ")");
    expect("SYMBOL", "{");

    while (!match("SYMBOL", "}")) {
        if (match("KEYWORD", "case")) {
            if (!match("NUMBER", NULL)) error("Expected number after case");
            expect("OPERATOR", ":"); 
        } else if (match("KEYWORD", "default")) {
            expect("OPERATOR", ":");
        } else {
            parseStatement();
        }
    }
    printf("Parsed switch statement successfully at line %d\n", start.line);
}

void parseWhileStmt() {
    Token start = peek();
    advance();
    expect("SYMBOL", "(");
    parseExpression();
    expect("SYMBOL", ")");
    expect("SYMBOL", "{");
    parseStatementList();
    expect("SYMBOL", "}");

    printf("Parsed while loop successfully at line %d\n", start.line);
}

void parseForStmt() {
    Token start = peek();
    advance(); 
    expect("SYMBOL", "(");

    if (strcmp(peek().type, "KEYWORD") == 0) {
        parseDeclaration();
    } else if (strcmp(peek().type, "IDENTIFIER") == 0) {
        parseAssignment();
    } else if (strcmp(peek().value, ";") == 0) {
        advance(); 
    } else {
        error("Expected declaration, assignment, or ';' in for-loop initialization");
    }

    if (strcmp(peek().value, ";") != 0) {
        parseExpression();
    }
    expect("SYMBOL", ";");

    if (strcmp(peek().value, ")") != 0) {
        if (strcmp(peek().type, "IDENTIFIER") == 0) {
            advance(); // consume variable
            if (match("OPERATOR", "++") || match("OPERATOR", "--")) {
            } else if (match("OPERATOR", "=")) {
                parseExpression();
            } else {
                error("Expected '++', '--' or '=' in for-loop update");
            }
        } else {
            error("Expected identifier in for-loop update");
        }
    }

    expect("SYMBOL", ")");

    expect("SYMBOL", "{");
    parseStatementList();
    expect("SYMBOL", "}");

    printf("Parsed for loop successfully at line %d\n", start.line);
}

void parsePrint() {
    match("IDENTIFIER", "printf");
    expect("SYMBOL", "(");
    while (!match("SYMBOL", ")")) {
        advance();
    }
    expect("SYMBOL", ";");
}

void parsePrimaryExpression() {
    if (match("IDENTIFIER", NULL)) {
        if (match("SYMBOL", "[")) {
            parseExpression();
            expect("SYMBOL", "]");
        } else if (match("SYMBOL", "(")) {
            while (!match("SYMBOL", ")")) {
                parseExpression();
                match("SYMBOL", ",");
            }
        }
    } else if (match("NUMBER", NULL)) {
        // literal
    } else if (match("STRING", NULL)) {
        // string literal
    } else {
        error("Expected expression");
    }
}

void parseExpression() {
    parsePrimaryExpression();
    while (match("OPERATOR", NULL)) {
        parsePrimaryExpression();
    }
}

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
    printf("Parsing Complete\n");

    return 0;
}
