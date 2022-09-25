#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qmessagebox.h>
#include "public.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_instance = CreateEXWardrobe();
    int rst = m_instance->Init();
    if (rst != 0) {
        QMessageBox::warning(this, "error", "初始化错误");
        exit(-1);
    }

    auto charactorNameList = m_instance->GetCharactorList();
    for (auto& item : charactorNameList) {
        ui->charactorBox->addItem(QString::fromStdString(item.second), QString::fromStdString(item.first));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

