/********************************************************************************
** Form generated from reading UI file 'elevator.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ELEVATOR_H
#define UI_ELEVATOR_H

#include <QtCore/QVariant>
#include <QtWidgets>

QT_BEGIN_NAMESPACE

class Ui_elevator
{
public:
    // 定义一个 QGroupBox 用于显示目的地楼层选择区域
    QGroupBox* groupBox_ele_button;
    // 定义一个 QGroupBox 用于显示电梯状态信息区域
    QGroupBox* groupBox;
    // 定义一个 QLabel 用于显示“状态”文本
    QLabel* label;
    // 定义一个 QLabel 用于显示电梯的运行状态（如“上”、“下”等）
    QLabel* label_status;
    // 定义一个 QLabel 用于显示“门状态”文本
    QLabel* label_3;
    // 定义一个 QLabel 用于显示电梯门的状态（如“开”、“关”）
    QLabel* label_door;
    // 定义一个 QLabel 用于显示电梯当前所在的楼层
    QLCDNumber* lcdDisplay_1;
    // 定义一个 QLabel 用于显示大楼的总楼层数
    QLabel* label_ELE_NUMBER;
  
    void setupUi(QWidget* elevator)
    {
        if (elevator->objectName().isEmpty())
            elevator->setObjectName("elevator");
        // 设置窗口大小
        elevator->resize(250, 380);
        elevator->setMinimumSize(QSize(250, 230));
        elevator->setMaximumSize(QSize(250, 480));
        // 创建一个字体对象，设置字体大小为 12
        QFont font;
        font.setPointSize(12);
        elevator->setFont(font);
        elevator->setStyleSheet("background: #ADD8E6; color: black;");

        // 创建目的地楼层选择区域的 QGroupBox
        groupBox_ele_button = new QGroupBox("按钮", elevator);
        groupBox_ele_button->setGeometry(QRect(10, 10, 231, 241));
        groupBox_ele_button->setStyleSheet("QGroupBox {"
            "    border: 2px solid #87CEFA;"  // 边框颜色为浅蓝色
            "    border-radius: 10px;"  // 边框圆角为 10 像素
            "    margin-top: 10px;"
            "    padding: 15px;"  // 内边距为 15 像素
            "    background-color: #F0F8FF;"  // 背景颜色为淡蓝色
            "}"
            "QGroupBox::title {"
            "    subcontrol-origin: margin;"
            "    subcontrol-position: top center;"
            "    padding: 0 10px;"  // 标题内边距为 0 到 10 像素
            "    background-color: #E0FFFF;"  // 标题背景颜色为浅蓝绿色
            "    border-radius: 5px;"  // 标题边框圆角为 5 像素
            "}");

        // 创建电梯状态信息区域的 QGroupBox
        groupBox = new QGroupBox("状态", elevator);
        groupBox->setGeometry(QRect(10, 260, 231, 101));
        groupBox->setStyleSheet("QGroupBox {"
            "    border: 2px solid #98FB98;"  // 边框颜色为浅绿色
            "    border-radius: 10px;"  // 边框圆角为 10 像素
            "    margin-top: 10px;"
            "    padding: 15px;"  // 内边距为 15 像素
            "    background-color: #F5FFFA;"  // 背景颜色为淡绿色
            "}"
            "QGroupBox::title {"
            "    subcontrol-origin: margin;"
            "    subcontrol-position: top center;"
            "    padding: 0 10px;"  // 标题内边距为 0 到 10 像素
            "    background-color: #CCFFCC;"  // 标题背景颜色为浅黄绿色
            "    border-radius: 5px;"  // 标题边框圆角为 5 像素
            "}");

        // 创建“状态”文本的 QLabel
        label = new QLabel("状态:", groupBox);
        label->setGeometry(QRect(20, 30, 35, 21));
        // 加载图片
        QPixmap pixmap("image/stop.png");
        // 创建一个QLabel用于显示图片
        label_status = new QLabel(groupBox);
        label_status->setGeometry(QRect(85, 28, 32, 32));
        label_status->setPixmap(pixmap);
        // 调整QLabel大小以适应图片
        label_status->setScaledContents(true);
        // 创建“门状态”文本的 QLabel
        label_3 = new QLabel("门状态:", groupBox);
        label_3->setGeometry(QRect(20, 65, 50, 21));

        // 创建显示电梯门状态的 QLabel
        label_door = new QLabel("已关门", groupBox);
        label_door->setGeometry(QRect(85, 65, 45, 21));

        // 创建LCD数字显示
        lcdDisplay_1 = new QLCDNumber(groupBox);
        lcdDisplay_1->setGeometry(QRect(145, 30, 55, 55));
        lcdDisplay_1->setDigitCount(2);
        lcdDisplay_1->setSegmentStyle(QLCDNumber::Filled);
        lcdDisplay_1->setStyleSheet("QLCDNumber {"
            "background-color: black; color: red; "
            "border: 2px solid gray; "
            "border-radius: 5px; "
            "padding: 5px; "
            "box-shadow: 0 0 5px rgba(0, 0, 0, 0.5);"
            "}");
        // 格式化显示内容，确保显示两位数字，不足两位时前面补 0
        QString displayText = QString("%1").arg(1, 2, 10, QChar('0'));
        // 在LCD上显示当前楼层
        lcdDisplay_1->display(displayText);

        // 创建显示大楼总楼层数的 QLabel
        label_ELE_NUMBER = new QLabel("", elevator);
        label_ELE_NUMBER->setGeometry(QRect(20, 10, 50, 21));
        label_ELE_NUMBER->setStyleSheet("color: red; font-size: 16px;");

        // 设置窗口的标题
        elevator->setWindowTitle("电梯");

        // 自动连接信号和槽
        QMetaObject::connectSlotsByName(elevator);
    } // setupUi
};

namespace Ui {
    class elevator : public Ui_elevator {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ELEVATOR_H