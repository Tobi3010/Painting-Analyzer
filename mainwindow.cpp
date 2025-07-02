#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pix("C:/Dev/Painting-Analyzer/images/carla4.jpg");

    QSize picSize = ui->picture->size();
    ui->picture->setPixmap(pix.scaled(picSize.width(), picSize.height(), Qt::KeepAspectRatio));


    QString sizeString = QString("(%1,%2)").arg(picSize.width()).arg(picSize.height());
    QTextStream out(stdout);
    out << sizeString;
}

MainWindow::~MainWindow()
{
    delete ui;
}
