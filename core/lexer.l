%{
#include <stdio.h>
#include <stdlib.h>

int line = 1;

FILE *tokenFile;

void printToken(const char *type, const char *value) {
    // Print to terminal
    printf("LINE %d | %-15s -> %s\n", line, type, value);

    // Also write to token.txt
    if (tokenFile) {
        fprintf(tokenFile, "LINE %d | %-15s -> %s\n", line, type, value);
    }
}

%}

%option noyywrap

DIGIT           [0-9]
ID              [a-zA-Z_][a-zA-Z0-9_]*
FLOAT_LITERAL   {DIGIT}+\.{DIGIT}+
INT_LITERAL     {DIGIT}+
CHAR_LITERAL    '\''(\\.|[^\\'])'\''
STRING_LITERAL  \"(\\.|[^\\"])*\"
WHITESPACE      [ \t\r]+

%%

"int"|"float"|"double"|"char"|"void"|"long"|"short"|"bool"      { printToken("DATA_TYPE", yytext); }
"if"|"else"|"switch"|"case"|"default"                          { printToken("CONTROL_KEYWORD", yytext); }
"for"|"while"|"do"|"break"|"continue"                          { printToken("LOOP_KEYWORD", yytext); }
"return"|"goto"                                                 { printToken("CONTROL_FLOW", yytext); }
"#"[^\n]*                                                      { printToken("PREPROCESSOR", yytext); }

{FLOAT_LITERAL}                                                { printToken("FLOAT_LITERAL", yytext); }
{INT_LITERAL}                                                  { printToken("INT_LITERAL", yytext); }
{CHAR_LITERAL}                                                 { printToken("CHAR_LITERAL", yytext); }
{STRING_LITERAL}                                               { printToken("STRING_LITERAL", yytext); }


"++"|"--"                                                      { printToken("UNARY_OPERATOR", yytext); }
"=="|"!="|"<="|">="|"&&"|"||"                                   { printToken("LOGICAL_OPERATOR", yytext); }
"="|"+"|"-"|"*"|"/"|"%"|"<"|">"                                 { printToken("OPERATOR", yytext); }


"("                                                            { printToken("LEFT_PAREN", yytext); }
")"                                                            { printToken("RIGHT_PAREN", yytext); }
"{"                                                            { printToken("LEFT_BRACE", yytext); }
"}"                                                            { printToken("RIGHT_BRACE", yytext); }
"["                                                            { printToken("LEFT_BRACKET", yytext); }
"]"                                                            { printToken("RIGHT_BRACKET", yytext); }

";"                                                            { printToken("SEMICOLON", yytext); }
","                                                            { printToken("COMMA", yytext); }
"."                                                            { printToken("DOT", yytext); }


{ID}                                                           { printToken("IDENTIFIER", yytext); }


"//".*                                                         { /* skip single-line comment */ }
"/*"([^*]*|\*+[^*/])*\*+"/"                                    { /* skip multi-line comment */ }


\n                                                             { line++; }
{WHITESPACE}                                                   { /* skip */ }


.                                                              { printToken("UNKNOWN", yytext); }

%%

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file.c>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("File error");
        return 1;
    }

    yyin = fp;

    // Open token.txt for writing
    tokenFile = fopen("token.txt", "w");
    if (!tokenFile) {
        perror("Could not open token.txt");
        fclose(fp);
        return 1;
    }

    yylex();

    fclose(fp);
    fclose(tokenFile);
    return 0;
}