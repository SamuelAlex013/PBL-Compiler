#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_SIZE 100
#define MAX_TOKENS 1000



// Token structure
typedef struct
{
  char type[20];   // e.g., "KEYWORD", "IDENTIFIER"
  char value[100]; // the actual token (like "int", "main", "=")
  int line;        // line number in source code
} Token;

Token tokens[MAX_TOKENS];
int tokenCount = 0;
int lineNumber = 1;

// List of simple keywords (you can expand this)
char *keywords[] = {
    "int", "float", "return", "if", "else", "while", "for", "void", "char", "double", "long", "short", "bool"};
int keywordCount = sizeof(keywords) / sizeof(keywords[0]);

// List of operators
char *operators[] = {
    "+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "++", "--"};
int operatorCount = sizeof(operators) / sizeof(operators[0]);

// Symbols (punctuation)
char *symbols[] = {
    "(", ")", "{", "}", "[", "]", ";", ","};
int symbolCount = sizeof(symbols) / sizeof(symbols[0]);

// Helper functions
int isKeyword(const char *word)
{
  for (int i = 0; i < keywordCount; i++)
  {
    if (strcmp(word, keywords[i]) == 0)
      return 1;
  }
  return 0;
}

int isOperator(const char *word)
{
  for (int i = 0; i < operatorCount; i++)
  {
    if (strcmp(word, operators[i]) == 0)
      return 1;
  }
  return 0;
}

int isSymbol(char ch)
{
  for (int i = 0; i < symbolCount; i++)
  {
    if (symbols[i][0] == ch && strlen(symbols[i]) == 1)
      return 1;
  }
  return 0;
}

// Add a token to our token list
void addToken(const char *type, const char *value)
{
  strcpy(tokens[tokenCount].type, type);
  strcpy(tokens[tokenCount].value, value);
  tokens[tokenCount].line = lineNumber;
  tokenCount++;
}

// Lexer Logic
int analyze(const char *filename)
{
  FILE *fp = fopen(filename, "r");
  if (!fp)
  {
    printf("Error: Could not open file %s\n", filename);
    return 0;
  }

  char ch, word[MAX_TOKEN_SIZE];
  int index = 0;
  lineNumber = 1;
  int inComment = 0;

  while ((ch = fgetc(fp)) != EOF)
  {
    if (inComment)
    {
      if (ch == '*' && (ch = fgetc(fp)) == '/')
      {
        inComment = 0;
      }
      else if (ch == '\n')
      {
        lineNumber++;
      }
      continue;
    }

    if (ch == '/' && index == 0)
    {
      char next = fgetc(fp);
      if (next == '/')
      {
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

// Main Function

int main() {
    char filename[300];

    printf("Enter full path to source code file (use quotes if it contains spaces):\n> ");
    fgets(filename, sizeof(filename), stdin);

    // Remove trailing newline
    filename[strcspn(filename, "\n")] = '\0';

    // If input starts and ends with quotes, remove both
    int len = strlen(filename);
    if (filename[0] == '"' && filename[len - 1] == '"') {
        // Shift left by 1 and set null terminator at new end
        memmove(filename, filename + 1, len - 2);
        filename[len - 2] = '\0';  // not -1!
    }

    // Debug print
    printf("Opening file: %s\n", filename);

    if (!analyze(filename)) {
        printf("Error: Could not open file %s\n", filename);
        return 1;
    }

    printTokens("tokens.txt");

    printf("\nTokens saved in tokens.txt\n");
    return 0;
}