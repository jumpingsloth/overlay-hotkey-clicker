#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void update_pos();
private:
    Ui::MainWindow *ui;

    void handle_hotkey();
    void handle_click();
    void update_label(int b_p_option);
    int speed;
    int automode;
    bool functionKeys;

    HWND target_window;
    HWND active_window;
    std::unique_ptr<std::thread> _hotkeyThread;
    std::unique_ptr<std::thread> _clickThread;
    bool _doEnd;
};
#endif // MAINWINDOW_H
