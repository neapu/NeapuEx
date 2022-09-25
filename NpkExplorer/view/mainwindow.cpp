#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qmessagebox.h>
#include "../core/ExToolKit.h"
#include <qfiledialog.h>
#include <QListWidgetItem>
#include <QGraphicsRectItem>

using namespace extoolkit;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_instance = extoolkit::CreateExToolKit();
    ui->lineEditCurentFrame->setValidator(new QIntValidator);
    ui->lineEditMatrixWidth->setValidator(new QIntValidator);
    ui->lineEditMatrixHeight->setValidator(new QIntValidator);
    ui->lineEditFrameX->setValidator(new QIntValidator);
    ui->lineEditFrameY->setValidator(new QIntValidator);
    ui->lineEditFrameWidth->setValidator(new QIntValidator);
    ui->lineEditFrameHeight->setValidator(new QIntValidator);
    ui->lineEditTotalFrame->setValidator(new QIntValidator);
    ui->lineEditFps->setValidator(new QIntValidator);

    ui->graphicsView->setScene(new QGraphicsScene());
    ui->graphicsView->scene()->setBackgroundBrush(Qt::gray);
    ui->graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    ui->graphicsViewColor->setScene(new QGraphicsScene());
    ui->graphicsViewColor->scene()->setBackgroundBrush(Qt::gray);
    ui->graphicsViewColor->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    ui->groupBoxDDSInfo->hide();
    ui->groupBoxPalette->hide();

    QObject::connect(&m_playTimer, &QTimer::timeout, this, &MainWindow::on_timeout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 打开
void MainWindow::on_actionOpen_triggered(bool checked)
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开文件", QString(), "Npk Files (*.npk);;All Files (*.*)");
    if (fileName.isEmpty()) {
        return;
    }

    int ret = m_instance->Open(fileName.toStdString());
    if (ret != 0) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        return;
    }

    m_open = true;

    ui->checkBoxAutoPlay->setEnabled(true);
    ui->pushButtonFrameJump->setEnabled(true);
    ui->pushButtonFramePrevious->setEnabled(true);
    ui->pushButtonFrameNext->setEnabled(true);

    ClearImageInfo();
    ui->listWidgetImage->clear();
    ClearImage();

    auto imageNameList = m_instance->GetImageNameList();
    int index = 0;
    for (auto &name : imageNameList) {
        auto item = new QListWidgetItem(QString::fromStdString(name), ui->listWidgetImage);
        item->setData(Qt::UserRole + 1, index);
        index++;
    }
    if (ui->listWidgetImage->count() > 0) {
        ui->listWidgetImage->setCurrentRow(0);
        SwitchImage(0);
    }
}

// 关闭
void MainWindow::on_actionClose_triggered(bool checked)
{
    m_open = false;
    m_playTimer.stop();
    ui->checkBoxAutoPlay->setEnabled(false);
    ui->pushButtonFrameJump->setEnabled(false);
    ui->pushButtonFramePrevious->setEnabled(false);
    ui->pushButtonFrameNext->setEnabled(false);
    m_instance->Close();
    ClearImageInfo();
    ui->listWidgetImage->clear();
    ClearImage();
}

// 切换image
void MainWindow::on_listWidgetImage_itemClicked(QListWidgetItem *item)
{
    int index = item->data(Qt::UserRole + 1).toUInt();
    SwitchImage(index);
}

void MainWindow::on_pushButtonFramePrevious_clicked(bool checked)
{
    if (!m_open) return;
    m_instance->PreviousFrame();
    this->UpdateFrame();
}

void MainWindow::on_pushButtonFrameNext_clicked(bool checked)
{
    if (!m_open) return;
    m_instance->NextFrame();
    this->UpdateFrame();
    
}

void MainWindow::on_checkBoxAutoPlay_stateChanged(int state)
{
    if (!m_open) return;

    if (state == Qt::Checked) {
        int fps = ui->lineEditFps->text().toInt();
        if (fps == 0) {
            fps = 25;
            ui->lineEditFps->setText("25");
        }
        m_playTimer.setInterval(1000 / fps);
        m_playTimer.start();
    } else {
        m_playTimer.stop();
    }
}

void MainWindow::on_comboBoxPalette_currentIndexChanged(int index)
{
    m_instance->SetCurentPalette(index);
    this->ClearImage();
    this->ShowImage();
    this->LoadPalette();
}

void MainWindow::on_timeout()
{
    on_pushButtonFrameNext_clicked(false);
}

void MainWindow::UpdateImage()
{
    int version = m_instance->GetImageVersion();
    auto path = m_instance->GetImagePath();

    ui->lineEditVersion->setText(QString::number(version));
    ui->lineEditImagePath->setText(QString::fromStdString(path));

    if (version == 6 || version == 4) {
        // 调色板信息
        ui->groupBoxPalette->show();
        m_instance->SetCurentPalette(0);
        ui->comboBoxPalette->clear();
        int paletteCount = m_instance->GetPaletteCount();
        for (int i = 1; i <= paletteCount; i++) {
            ui->comboBoxPalette->addItem(QString::number(i));
        }
        this->LoadPalette();

        if (version == 4) {
            ui->groupBoxPalette->setEnabled(false);
        }
        else {
            ui->groupBoxPalette->setEnabled(true);
        }
    }
    else {
        ui->groupBoxPalette->hide();
    }
    this->UpdateFrame();
}

void MainWindow::UpdateFrame()
{
    m_curentFrame = m_instance->GetCurentFrameHandle();
    int frameCount = m_instance->GetFrameCount();
    int frameIndex = m_instance->GetCurentFrame();
    int colorFormat = m_curentFrame->ColorFormat();

    ui->lineEditTotalFrame->setText(QString::number(frameCount));
    ui->lineEditCurentFrame->setText(QString::number(frameIndex + 1));
    ui->lineEditMatrixWidth->setText(QString::number(m_curentFrame->Width()));
    ui->lineEditMatrixHeight->setText(QString::number(m_curentFrame->Height()));
    ui->lineEditFrameX->setText(QString::number(m_curentFrame->PosX()));
    ui->lineEditFrameY->setText(QString::number(m_curentFrame->PosY()));
    ui->lineEditFrameWidth->setText(QString::number(m_curentFrame->FrameWidth()));
    ui->lineEditFrameHeight->setText(QString::number(m_curentFrame->FrameHeight()));
    ui->lineEditFrameColorFormat->setText(QString::fromStdString(ColorFormatToString(colorFormat)));
    ui->lineEditLink->setText(QString::number(m_curentFrame->LinkTo()));
    if (m_curentFrame->IsDDSFormat()) {
        int ddsFrameCount = m_instance->GetDDSFrameCount();
        QString ddsFormat = QString::fromStdString(m_instance->GetDDSFormat());
        auto [ddsWidth, ddsHeight] = m_instance->GetDDSFrameSize();
        ui->groupBoxDDSInfo->show();
        ui->lineEditDDSFrame->setText(QString::number(m_curentFrame->DDSIndex() + 1));
        ui->lineEditDDSTotalFrame->setText(QString::number(ddsFrameCount));
        ui->lineEditDXTFormat->setText(ddsFormat);
        ui->lineEditDDSWidth->setText(QString::number(ddsWidth));
        ui->lineEditDDSHeight->setText(QString::number(ddsHeight));
        ui->lineEditDDSCatLeft->setText(QString::number(m_curentFrame->DDSLeft()));
        ui->lineEditDDSCatRight->setText(QString::number(m_curentFrame->DDSRight()));
        ui->lineEditDDSCatTop->setText(QString::number(m_curentFrame->DDSTop()));
        ui->lineEditDDSCatBotton->setText(QString::number(m_curentFrame->DDSBotton()));
    }
    else {
        ui->groupBoxDDSInfo->hide();
    }

    this->ClearImage();
    this->ShowImage();
}

void MainWindow::Clear()
{
    ClearImageInfo();
    ClearImage();
    ui->groupBoxDDSInfo->hide();
    ui->groupBoxPalette->hide();
}

void MainWindow::ClearImageInfo()
{
    ui->lineEditVersion->clear();
    ui->lineEditImagePath->clear();
    ui->lineEditTotalFrame->clear();
    ui->lineEditCurentFrame->clear();
    ui->lineEditMatrixWidth->clear();
    ui->lineEditMatrixHeight->clear();
    ui->lineEditFrameX->clear();
    ui->lineEditFrameY->clear();
    ui->lineEditFrameWidth->clear();
    ui->lineEditFrameHeight->clear();
    ui->lineEditFrameColorFormat->clear();
    ui->lineEditLink->clear();
    ui->lineEditDDSFrame->clear();
    ui->lineEditDDSTotalFrame->clear();
    ui->lineEditDXTFormat->clear();
    ui->lineEditDDSWidth->clear();
    ui->lineEditDDSHeight->clear();
    ui->lineEditDDSCatLeft->clear();
    ui->lineEditDDSCatRight->clear();
    ui->lineEditDDSCatTop->clear();
    ui->lineEditDDSCatBotton->clear();
    ui->lineEditFps->clear();
}

static QImage ConvertFrame(std::shared_ptr<FrameHandle> _frame)
{
    QImage rst(_frame->Width(), _frame->Height(), QImage::Format_ARGB32);
    for (int y = 0; y < _frame->Height(); y++) {
        for (int x = 0; x < _frame->Width(); x++) {
            auto clr = _frame->GetColor(x, y);
            rst.setPixelColor(QPoint(x, y), QColor(clr.r, clr.g, clr.b, clr.a));
        }
    }
    return rst;
}

void MainWindow::ShowImage()
{
    auto frameHandle = m_instance->GetCurentFrameHandle();
    if (frameHandle == nullptr) {
        return;
    }
    QImage image = ConvertFrame(frameHandle);
    ui->graphicsView->scene()->clear();
    ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(image));
}

void MainWindow::ClearImage()
{
    ui->graphicsView->scene()->clear();
}

void MainWindow::SwitchImage(int _index)
{
    m_playTimer.stop();
    ui->checkBoxAutoPlay->setChecked(false);
    m_instance->SetCurentImageIndex(_index);
    this->Clear();
    this->UpdateImage();
}

void MainWindow::LoadPalette()
{
    ui->graphicsViewColor->scene()->clear();
    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(1);
    auto colorList = m_instance->GetPaletteColors();
    int index = 0;
    for (auto &color : colorList) {
        int x = (index % 15) * 20 + (index % 15) * 2 + 2;
        int y = (index / 15) * 20 + (index / 15) * 2 + 2;
        auto *item = new QGraphicsRectItem();
        item->setRect(x, y, 20, 20);
        item->setPen(pen);
        item->setBrush(QColor(color.r, color.g, color.b, color.a));
        ui->graphicsViewColor->scene()->addItem(item);
        index++;
    }
}
