// lexer.h
#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKEN_SIZE 100
#define MAX_TOKENS 10000

// Token structure
typedef struct
{
  char type[20];   // e.g., "KEYWORD", "IDENTIFIER"
  char value[100]; // the actual token (like "int", "main", "=")
  int line;        // line number in source code
} Token;

extern Token tokens[MAX_TOKENS];
extern int tokenCount;

int analyze(const char *filename);
void printTokens(const char *outFile);

#endif
