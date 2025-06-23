#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Token tokens[MAX_TOKENS];
int tokenCount = 0;
int lineNumber = 1;

char *keywords[] = {
    "auto", "bool", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int",
    "long", "register", "restrict", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Alignas",
    "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn", "_Static_assert",
    "_Thread_local"
};
int keywordCount = sizeof(keywords) / sizeof(keywords[0]);

// List of operators
char *operators[] = {
    "+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "++", "--", ":" ,"&"};
int operatorCount = sizeof(operators) / sizeof(operators[0]);

// Symbols (punctuation)
char *symbols[] = {
    "(", ")", "{", "}", "[", "]", ";", ",", "'"};
int symbolCount = sizeof(symbols) / sizeof(symbols[0]);

// Helper functions
int isKeyword(const char *word){
  for (int i = 0; i < keywordCount; i++)
  {
    if (strcmp(word, keywords[i]) == 0)
      return 1;
  }
  return 0;
}

int isOperator(const char *word){
  for (int i = 0; i < operatorCount; i++){
    if (strcmp(word, operators[i]) == 0)
      return 1;
  }
  return 0;
}

int isSymbol(char ch){
  for (int i = 0; i < symbolCount; i++){
    if (symbols[i][0] == ch && strlen(symbols[i]) == 1)
      return 1;
  }
  return 0;
}

// Add a token to our token list
void addToken(const char *type, const char *value){
  strcpy(tokens[tokenCount].type, type);
  strcpy(tokens[tokenCount].value, value);
  tokens[tokenCount].line = lineNumber;
  tokenCount++;
}

// Lexer Logic
int analyze(const char *filename){
  FILE *fp = fopen(filename, "r");
  if (!fp){
    printf("Error: Could not open file %s\n", filename);
    return 0;
  }

  char ch, word[MAX_TOKEN_SIZE];
  int index = 0;
  lineNumber = 1;
  int inComment = 0;

  while ((ch = fgetc(fp)) != EOF){
    // Handle preprocessor directives
    if (ch == '#'){
      char directiveLine[MAX_TOKEN_SIZE];
      int dIndex = 0;
      directiveLine[dIndex++] = ch;

      while ((ch = fgetc(fp)) != EOF && ch != '\n')
      {
        if (dIndex < MAX_TOKEN_SIZE - 1)
          directiveLine[dIndex++] = ch;
      }

      directiveLine[dIndex] = '\0';
      addToken("PREPROCESSOR", directiveLine);
      lineNumber++;
      continue;
    } 

    if (ch == '"') {
      char strLiteral[MAX_TOKEN_SIZE];
      int strIdx = 0;
      strLiteral[strIdx++] = ch;

      while ((ch = fgetc(fp)) != EOF && ch != '"') {
          strLiteral[strIdx++] = ch;
      }
      strLiteral[strIdx++] = '"';
      strLiteral[strIdx] = '\0';

      addToken("STRING", strLiteral);
      continue;
    }

    if (inComment){
      if (ch == '*' && (ch = fgetc(fp)) == '/'){
        inComment = 0;
      }
      else if (ch == '\n'){
        lineNumber++;
      }
      continue;
    }

    if (ch == '\'') {
      char charLiteral[4];
      charLiteral[0] = '\'';
      charLiteral[1] = fgetc(fp);  
      charLiteral[2] = fgetc(fp); 
      charLiteral[3] = '\0';

      if (charLiteral[2] != '\'') {
         break;
      }

      addToken("CHAR", charLiteral);
      continue;
    }

    if (ch == '/' && index == 0){
      char next = fgetc(fp);
      if (next == '/'){
        while ((ch = fgetc(fp)) != EOF && ch != '\n')
          ;
        lineNumber++;
        continue;
      }
      else if (next == '*')
      {
        inComment = 1;
        continue;
      }
      else
      {
        ungetc(next, fp);
        word[index++] = ch;
      }
    }
    else if (ch == '\n')
    {
      if (index != 0)
      {
        word[index] = '\0';
        if (isKeyword(word))
          addToken("KEYWORD", word);
        else if (isdigit(word[0]) || (word[0] == '.' && isdigit(word[1])))
        { // Handle potential starting with '.'
          addToken("NUMBER", word);
        }
        else
        {
          addToken("IDENTIFIER", word);
        }
        index = 0;
      }
      lineNumber++;
      continue;
    }
    else if (isspace(ch))
    {
      if (index != 0)
      {
        word[index] = '\0';
        if (isKeyword(word))
          addToken("KEYWORD", word);
        else if (isdigit(word[0]) || (word[0] == '.' && isdigit(word[1])))
        { // Handle potential starting with '.'
          addToken("NUMBER", word);
        }
        else
        {
          addToken("IDENTIFIER", word);
        }
        index = 0;
      }
      continue;
    }
    else if (isalpha(ch) || ch == '_')
    {
      word[index++] = ch;
    }
    else if (isdigit(ch) || (ch == '.' && index > 0 && isdigit(word[index - 1])))
    { // Allow '.' only after a digit
      word[index++] = ch;
    }
    else if (isSymbol(ch))
    {
      if (index != 0)
      {
        word[index] = '\0';
        if (isKeyword(word))
          addToken("KEYWORD", word);
        else if (isdigit(word[0]) || (word[0] == '.' && isdigit(word[1])))
        { // Handle potential starting with '.'
          addToken("NUMBER", word);
        }
        else
        {
          addToken("IDENTIFIER", word);
        }
        index = 0;
      }
      char symbolStr[2] = {ch, '\0'};
      addToken("SYMBOL", symbolStr);
    }
    else
    {
      if (index != 0)
      {
        word[index] = '\0';
        if (isKeyword(word))
          addToken("KEYWORD", word);
        else if (isdigit(word[0]) || (word[0] == '.' && isdigit(word[1])))
        { // Handle potential starting with '.'
          addToken("NUMBER", word);
        }
        else
        {
          addToken("IDENTIFIER", word);
        }
        index = 0;
      }
      char op[3] = {ch, '\0', '\0'};
      char next = fgetc(fp);
      op[1] = next;

      if (isOperator(op))
      {
        addToken("OPERATOR", op);
      }
      else
      {
        ungetc(next, fp);
        op[1] = '\0';
        if (isOperator(op))
        {
          addToken("OPERATOR", op);
        }
        else
        {
          addToken("UNKNOWN", op);
        }
      }
    }
  }

  // Check for any remaining word at the end of the file
  if (index != 0)
  {
    word[index] = '\0';
    if (isKeyword(word))
      addToken("KEYWORD", word);
    else if (isdigit(word[0]) || (word[0] == '.' && isdigit(word[1])))
    { // Handle potential starting with '.'
      addToken("NUMBER", word);
    }
    else
    {
      addToken("IDENTIFIER", word);
    }
  }

  fclose(fp);
  return 1;
}

// Output tokens to console and file
void printTokens(const char *outFile)
{
  FILE *fp = fopen(outFile, "w");
  if (!fp)
  {
    printf("Could not open file to write tokens.\n");
    return;
  }

  printf("\n----------- TOKENS -----------\n");
  for (int i = 0; i < tokenCount; i++)
  {
    printf("LINE %d | %-10s -> %s\n", tokens[i].line, tokens[i].type, tokens[i].value);
    fprintf(fp, "LINE %d | %-10s -> %s\n", tokens[i].line, tokens[i].type, tokens[i].value);
  }
  printf("------------------------------\n");

  fclose(fp);
}