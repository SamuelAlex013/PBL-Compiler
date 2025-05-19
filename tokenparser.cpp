#include "tokenparser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

QList<Token> parseTokensFromFile(const QString& filePath) {
    QList<Token> tokens;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Could not open token file!";
        return tokens;
    }

    QTextStream in(&file);
    QRegularExpression regex(R"(LINE\s+(\d+)\s+\|\s+(\w+)\s+->\s+(.*))");

    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match = regex.match(line);
        if (match.hasMatch()) {
            Token token;
            token.line = match.captured(1).toInt();
            token.type = match.captured(2).trimmed();
            token.value = match.captured(3).trimmed();
            tokens.append(token);
        } else {
            qDebug() << "Skipped line (no match):" << line;
        }
    }
    return tokens;
}
