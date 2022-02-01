#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Windows.h"
#include <QDebug>
#include <QTimer>
#include <iostream>
#include <thread>
#include <QMessageBox>
#include <string>

void MainWindow::update_label(int b_p_option) {
    std::string option = "";
    if (b_p_option == 1) {
        option = "build:";
    } else if (b_p_option == 2) {
        option = "pvp:";
    }
    ui->label->setText(QString::fromStdString("speed:" + option + " " + std::to_string(speed) + "\nautomode: " + (automode ? "active" : "inactive")));
}

void MainWindow::handle_hotkey() {
    if (functionKeys) {
        std::cout << "register hotkey: f1; status: " << RegisterHotKey(NULL, 1, NULL, VK_F1) << std::endl;
        std::cout << "register hotkey: f2; status: " << RegisterHotKey(NULL, 2, NULL, VK_F2) << std::endl;
        std::cout << "register hotkey: f3; status: " << RegisterHotKey(NULL, 3, NULL, VK_F3) << std::endl;
        std::cout << "register hotkey: f4; status: " << RegisterHotKey(NULL, 4, NULL, VK_F4) << std::endl;
        std::cout << "register hotkey: tab; status: " << RegisterHotKey(NULL, 5, NULL, VK_TAB) << std::endl;
    } else {
        std::cout << "register hotkey: 1; status: " << RegisterHotKey(NULL, 1, NULL, 0x31) << std::endl;
        std::cout << "register hotkey: 2; status: " << RegisterHotKey(NULL, 2, NULL, 0x32) << std::endl;
        std::cout << "register hotkey: 3; status: " << RegisterHotKey(NULL, 3, NULL, 0x33) << std::endl;
        std::cout << "register hotkey: 4; status: " << RegisterHotKey(NULL, 4, NULL, 0x34) << std::endl;
        std::cout << "register hotkey: tab; status: " << RegisterHotKey(NULL, 5, NULL, VK_TAB) << std::endl;
    }

    std::cout << "default speed:\t\t" << speed << std::endl;
    std::cout << "automode " << (automode ? "active" : "inactive") << std::endl;
    update_label(0);

    MSG msg = {0};

    while(true) {
        if (PeekMessage(&msg, NULL, WM_HOTKEY, WM_HOTKEY, PM_REMOVE) > 0) {
            if (msg.message == WM_HOTKEY) {
                switch (msg.wParam)
                {
                case 1:
                    speed = 1000;
                    std::cout << "speed set to building:\t" << speed << std::endl;
                    update_label(1);
                    break;
                case 2:
                    speed -= 100;
                    if (speed < 100) {
                        speed = 100;
                    }
                    std::cout << "speed set to:\t\t" << speed << std::endl;
                    update_label(0);
                    break;
                case 3:
                    speed += 100;
                    if (speed > 1000) {
                        speed = 1000;
                    }
                    std::cout << "speed set to:\t\t" << speed << std::endl;
                    update_label(0);
                    break;
                case 4:
                    speed = 10;
                    std::cout << "speed set to pvp:\t" << speed << std::endl;
                    update_label(2);
                    break;
                case 5:
                    automode = !automode;
                    std::cout << "auto mode " << (automode ? "activated" : "deactivated") << std::endl;
                    update_label(0);
                    break;
                default:
                    std::cout << "default" << std::endl;
                    break;
                }
            }
        }
    }

    std::cout << "handle hotkey end" << std::endl;
}

void MainWindow::handle_click() {
    INPUT input;
    while (!_doEnd) {
        if ((GetKeyState(VK_LBUTTON) & 0x8000) || (GetKeyState(VK_LBUTTON) & 0x8000) || (GetKeyState(VK_RBUTTON) & 0x8000)) {
            automode = false;
        }
        if (GetKeyState(VK_MBUTTON) & 0x8000) {
            automode = false;

            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            SendInput(1, &input, sizeof(input));

            Sleep(speed);

            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, &input, sizeof(input));

            if (speed <= 10) {
                Sleep(10);
            }
        } else if (automode) {
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            SendInput(1, &input, sizeof(input));

            Sleep(speed);

            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, &input, sizeof(input));

            if (speed <= 10) {
                Sleep(10);
            }
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _doEnd(false)
{
    ui->setupUi(this);

    speed = 1000;
    automode = false;
    functionKeys = true;

    QFont f("Arial", 14, QFont::Bold);
    ui->label->setFont(f);
    QPalette palette = ui->label->palette();
    palette.setColor(ui->label->foregroundRole(), Qt::red);
    ui->label->setPalette(palette);
    ui->label->setText("mode");

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    // minecraft target window "GLFW30"
    target_window = FindWindowA("Notepad", 0);
    if (!target_window) {
      qDebug() << "window not found";
      return;
    }
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_pos()));
    timer->start(50); // update interval in milliseconds

    QString manual_text = "(f)1:\tset speed to building (1000)\n"
                          "(f)2:\treduce speed by 100\n"
                          "(f)3:\tincrease speed by 100\n"
                          "(f)4:\tset speed to pvp (10/10)\n"
                          "tab:\tactivate/deactivate automode\n";
    QMessageBox::information(this, "Manual", manual_text, QMessageBox::Ok);

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Function Keys", "Do you want to use Function Keys?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        functionKeys = true;
        std::cout << "using function keys" << std::endl;
    } else {
        functionKeys = false;
        std::cout << "using number keys" << std::endl;
    }

    _hotkeyThread = std::make_unique<std::thread>(&MainWindow::handle_hotkey, this);
    _clickThread = std::make_unique<std::thread>(&MainWindow::handle_click, this);
}

MainWindow::~MainWindow()
{
    qDebug() << "deconstructor called";
    _doEnd = true;
    _hotkeyThread->join();
    _clickThread->join();
    delete ui;
}

void MainWindow::update_pos() {
    RECT rect;
    if (GetWindowRect(target_window, &rect)) {
        setGeometry(rect.left, rect.top, rect.right - rect.left,
                    rect.bottom - rect.top);
        active_window = GetForegroundWindow();
        if (target_window == active_window) {
            ui->label->setVisible(true);
        } else {
            ui->label->setVisible(false);
        }
    } else {
        //maybe window was closed
        qDebug() << "GetWindowRect failed";
        QApplication::quit();
        std::cout << "end of program" << std::endl;
    }
}

