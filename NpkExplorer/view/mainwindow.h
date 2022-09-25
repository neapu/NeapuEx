#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QTimer>

namespace extoolkit {
class ExToolKit;
class FrameHandle;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class QListWidgetItem;
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered(bool checked);
    void on_actionClose_triggered(bool checked);
    void on_listWidgetImage_itemClicked(QListWidgetItem *item);
    void on_pushButtonFramePrevious_clicked(bool checked);
    void on_pushButtonFrameNext_clicked(bool checked);
    void on_checkBoxAutoPlay_stateChanged(int state);
    void on_comboBoxPalette_currentIndexChanged(int index);

    void on_timeout();

private:
    // 当切换Image时，会发生改变的界面
    void UpdateImage();
    // 当切换Frame时，会发生改变的界面
    void UpdateFrame();

    void Clear();

    void UpdateImageInfo();
    void ClearImageInfo();
    void ShowImage();
    void ClearImage();
    void SwitchImage(int _index);
    void LoadPalette();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<extoolkit::ExToolKit> m_instance = nullptr;
    std::shared_ptr<extoolkit::FrameHandle> m_curentFrame = nullptr;
    QTimer m_playTimer;
    bool m_open = false;
};
#endif // MAINWINDOW_H
