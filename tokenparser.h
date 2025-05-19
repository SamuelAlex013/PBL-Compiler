// File: ui/tokenparser.h
#ifndef TOKENPARSER_H
#define TOKENPARSER_H

#include <QString>
#include <QList>

struct Token {
    int line;
    QString type;
    QString value;
};

QList<Token> parseTokensFromFile(const QString& filePath);

#endif // TOKENPARSER_H
