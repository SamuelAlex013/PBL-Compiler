#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tokenparser.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QList<Token> tokens = parseTokensFromFile(QCoreApplication::applicationDirPath() + "/core/output/tokens.txt");
    qDebug() << "Loaded" << tokens.size() << "tokens.";
    loadTokensIntoTable(tokens);
}

MainWindow::~MainWindow() {
    delete ui;
}

QBrush getBrushForType(const QString &type) {
    if (type == "KEYWORD") return QBrush(Qt::blue);
    if (type == "IDENTIFIER") return QBrush(Qt::darkGreen);
    if (type == "SYMBOL") return QBrush(QColor(255, 140, 0));  // orange
    if (type == "OPERATOR") return QBrush(Qt::red);
    if (type == "UNKNOWN") return QBrush(Qt::gray);
    return QBrush(Qt::black);
}

QString getIconForType(const QString &type) {
    if (type == "KEYWORD") return "🟦 ";
    if (type == "IDENTIFIER") return "🟩 ";
    if (type == "SYMBOL") return "🔣 ";
    if (type == "OPERATOR") return "🔺 ";
    if (type == "UNKNOWN") return "❓ ";
    return "";
}

void MainWindow::loadTokensIntoTable(const QList<Token>& tokens) {
    ui->tableWidget->setRowCount(tokens.size());
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels({"Line", "Type", "Value"});
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setSortingEnabled(true);

    for (int i = 0; i < tokens.size(); ++i) {
        const Token& token = tokens[i];

        auto *lineItem = new QTableWidgetItem(QString::number(token.line));
        lineItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 0, lineItem);

        auto *typeItem = new QTableWidgetItem(getIconForType(token.type) + token.type);
        typeItem->setForeground(getBrushForType(token.type));
        typeItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 1, typeItem);

        auto *valueItem = new QTableWidgetItem(token.value);
        valueItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 2, valueItem);
    }
}
