/********************************************************************************
** Form generated from reading UI file 'building.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BUILDING_H
#define UI_BUILDING_H

#include <QtCore/QVariant>
#include <QtWidgets>

QT_BEGIN_NAMESPACE

class Ui_building
{
public:
    // 定义一个 QGroupBox 指针，用于显示电梯相关信息
    QGroupBox* groupBox_eles;
    // 定义一个 QGroupBox 指针，用于显示楼层按钮
    QGroupBox* groupBox_btns;
    // 定义一个 QLabel 指针，用于显示底部的提示信息
    QLabel* label_bar;

    void setupUi(QWidget* building)
    {
        // 检查主窗口是否有对象名，如果没有则设置对象名为 "building"
        if (building->objectName().isEmpty())
            building->setObjectName("building");
        // 设置主窗口的大小为 450x620 像素
        building->resize(450, 420);
        building->setMinimumSize(QSize(450, 300));
        building->setMaximumSize(QSize(450, 540));

        // 定义一个 QFont 对象，设置字体大小为 12 磅
        QFont font;
        font.setPointSize(12);
        // 定义另一个 QFont 对象，设置字体大小为 9 磅
        QFont font1;
        font1.setPointSize(9);

        // 设置主窗口的字体为 font
        building->setFont(font);
        // 设置主窗口的透明度为 1.0（不透明）
        building->setWindowOpacity(1.0);
        // 设置主窗口的标题
        building->setWindowTitle("2381289周慧星-楼层");

        // 创建一个 QGroupBox 对象，标题为 "电梯"，父窗口为 building
        groupBox_eles = new QGroupBox("电梯", building);
        // 设置 QGroupBox 在主窗口中的位置和大小
        groupBox_eles->setGeometry(QRect(10, 10, 430, 105));
        // 美化电梯信息的 QGroupBox 样式
        groupBox_eles->setStyleSheet("QGroupBox {"
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

        // 创建一个 QGroupBox 对象，标题为 "楼层按钮"，父窗口为 building
        groupBox_btns = new QGroupBox("楼层按钮", building);
        // 设置 QGroupBox 在主窗口中的位置和大小
        groupBox_btns->setGeometry(QRect(10, 120, 430, 260));
        groupBox_btns->setStyleSheet("QGroupBox {"
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

        label_bar = new QLabel("", building);
        // 设置 QLabel 在主窗口中的位置和大小
        label_bar->setGeometry(QRect(10, 390, 430, 20));
        // 设置 QLabel 的字体为 font1
        label_bar->setFont(font1);
        // 美化底部提示信息的 QLabel 样式
        label_bar->setStyleSheet("QLabel {"
            "    background-color: #D3D3D3;"  // 背景颜色为浅灰色
            "    color: #000000;"  // 文字颜色为黑色
            "    border-radius: 5px;"  // 边框圆角为 5 像素
            "    padding: 2px 5px;"  // 内边距为 2 到 5 像素
            "}");

        // 自动连接信号和槽
        QMetaObject::connectSlotsByName(building);
    } // setupUi
};

namespace Ui {
    class building : public Ui_building {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BUILDING_H
