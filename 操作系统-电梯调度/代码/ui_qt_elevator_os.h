/********************************************************************************
** Form generated from reading UI file 'qt_elevator_os.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_ELEVATOR_OS_H
#define UI_QT_ELEVATOR_OS_H

#include <QtCore/QVariant>
#include <QtWidgets>

QT_BEGIN_NAMESPACE

// 定义 UI 类，用于设置和管理电梯操作系统的用户界面
class Ui_qt_elevator_osClass
{
public:
	// 中央窗口部件，是主窗口的核心区域
	QWidget* centralWidget;
	// 用于分组显示电梯相关设置的容器
	QGroupBox* groupBox;
	// 用于输入电梯数量的旋转框
	QSpinBox* spinBox_elevator;
	// 电梯数量的标签，用于提示用户输入内容
	QLabel* label_elevator;
	// 楼层数的标签，用于提示用户输入内容
	QLabel* label_floor;
	// 用于输入楼层数的旋转框
	QSpinBox* spinBox_floor;
	// 算法选择的标签，用于提示用户输入内容
	QLabel* label_floor_2;
	// 用于分组显示操作按钮的容器
	QGroupBox* groupBox_2;
	// 运行按钮，点击后启动电梯系统
	QPushButton* pushButton_run;
	// 停止按钮，点击后停止电梯系统
	QPushButton* pushButton_stop;
	// 退出按钮，点击后退出程序
	QPushButton* pushButton_exit;
	// 用于选择算法的旋转框
	QComboBox* comboBox;
	QLabel* resultLabel;

	// 该函数用于设置 UI 界面的布局和初始属性
	void setupUi(QMainWindow* qt_elevator_osClass)
	{
		if (qt_elevator_osClass->objectName().isEmpty())
			qt_elevator_osClass->setObjectName("qt_elevator_osClass");
		qt_elevator_osClass->resize(430, 240);
		qt_elevator_osClass->setMinimumSize(QSize(430, 240));
		qt_elevator_osClass->setMaximumSize(QSize(430, 240));
		QFont font;
		font.setPointSize(12);
		// 将字体应用到主窗口
		qt_elevator_osClass->setFont(font);
		qt_elevator_osClass->setWindowOpacity(1.0);
		qt_elevator_osClass->setWindowTitle("2351289周慧星-电梯调度");

		// 创建中央窗口部件，并将其设置为主窗口的子部件
		centralWidget = new QWidget(qt_elevator_osClass);
		qt_elevator_osClass->setCentralWidget(centralWidget);
		centralWidget->setStyleSheet("background-color: #f0f0f0;");

		// 创建分组框，用于分组显示电梯相关设置，并将其设置为中央窗口部件的子部件
		groupBox = new QGroupBox("电梯设置", centralWidget);
		groupBox->setGeometry(QRect(20, 10, 390, 120));
		groupBox->setFont(font);
		groupBox->setStyleSheet("QGroupBox {"
			"    border: 2px solid #98FB98;"
			"    border-radius: 5px;"
			"    margin-top: 10px;"
			"    padding: 10px;"
			"    background-color: #F5FFFA;"
			"}"
			"QGroupBox::title {"
			"    subcontrol-origin: margin;"
			"    subcontrol-position: top center;"
			"    padding: 0 5px;"  // 标题内边距为 0 到 10 像素
			"    background-color: #CCFFCC;"  // 标题背景颜色为浅黄绿色
			"    border-radius: 5px;"  // 标题边框圆角为 5 像素
			"}");

		// 创建电梯数量的旋转框，并将其设置为分组框的子部件
		spinBox_elevator = new QSpinBox(groupBox);
		spinBox_elevator->setGeometry(QRect(110, 30, 71, 31));
		spinBox_elevator->setMinimum(2);
		spinBox_elevator->setMaximum(5);
		spinBox_elevator->setValue(5);

		// 创建电梯数量的标签，并将其设置为分组框的子部件
		label_elevator = new QLabel("电梯数量:",groupBox);
		label_elevator->setGeometry(QRect(20, 30, 71, 31));

		// 创建楼层数的旋转框，并将其设置为分组框的子部件
		spinBox_floor = new QSpinBox(groupBox);
		spinBox_floor->setGeometry(QRect(310, 30, 71, 31));
		spinBox_floor->setMinimum(5);
		spinBox_floor->setMaximum(30);
		spinBox_floor->setValue(20);

		// 创建楼层数的标签，并将其设置为分组框的子部件
		label_floor = new QLabel("楼层数量:",groupBox);
		label_floor->setGeometry(QRect(220, 30, 71, 31));

		// 创建算法选择的标签，并将其设置为分组框的子部件
		label_floor_2 = new QLabel("算法选择:", groupBox);
		label_floor_2->setGeometry(QRect(20, 80, 71, 31));

		// 创建 QComboBox 控件
		comboBox = new QComboBox(groupBox);
		comboBox->setGeometry(QRect(110, 80, 151, 31));
		// 添加选项
		comboBox->addItem("LOOK 调度算法");
		comboBox->addItem("最短剩余时间调度算法");
		comboBox->addItem("分组调度算法");
		comboBox->addItem("FCFS算法+评分最优");
		comboBox->addItem("FCFS算法+任务最少");
		comboBox->addItem("FCFS算法+顺序分配");

		resultLabel = new QLabel("当前选中算法1", groupBox);
		resultLabel->setGeometry(QRect(275, 80, 91, 31));
		QObject::connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
			[=](int index) {
				resultLabel->setText("当前选中算法" + QString::number(index+1));
			});

		// 创建用于分组显示操作按钮的分组框，并将其设置为中央窗口部件的子部件
		groupBox_2 = new QGroupBox("按钮选择",centralWidget);
		groupBox_2->setGeometry(QRect(20, 140, 390, 80));
		groupBox_2->setStyleSheet("QGroupBox {"
			"    border: 2px solid #87CEFA;"
			"    border-radius: 5px;"
			"    margin-top: 10px;"
			"    padding: 10px;"
			"    background-color: #F0F8FF;"
			"}"
			"QGroupBox::title {"
			"    subcontrol-origin: margin;"
			"    subcontrol-position: top center;"
			"    padding: 0 5px;"  // 标题内边距为 0 到 10 像素
			"    background-color: #E0FFFF;"  // 标题背景颜色为浅蓝绿色
			"    border-radius: 5px;"  // 标题边框圆角为 5 像素
			"}");

		// 创建运行按钮，并将其设置为分组框的子部件
		pushButton_run = new QPushButton("开始",groupBox_2);
		pushButton_run->setGeometry(QRect(20, 25, 80, 41));
		pushButton_run->setStyleSheet("QPushButton {"
			"    background-color: #4CAF50;"
			"    color: white;"
			"    padding: 10px 20px;"
			"    border: 2px solid #000;"
			"    border-radius: 5px;"
			"    box - shadow: 2px 2px 5px rgba(0, 0, 0, 0.3);" 
			"}"
			"QPushButton:hover {"
			"    background-color: #45a049;"
			"}");

		// 创建停止按钮，并将其设置为分组框的子部件
		pushButton_stop = new QPushButton("停止",groupBox_2);
		pushButton_stop->setGeometry(QRect(160, 25, 80, 41));
		pushButton_stop->setStyleSheet("QPushButton {"
			"    background-color: #FF9800;"
			"    color: white;"
			"    padding: 10px 20px;"
			"    border: 2px solid #000;"
			"    border-radius: 5px;"
			"    box - shadow: 2px 2px 5px rgba(0, 0, 0, 0.3);"
			"}"
			"QPushButton:hover {"
			"    background-color: #e68a00;"
			"}");

		// 创建退出按钮，并将其设置为分组框的子部件
		pushButton_exit = new QPushButton("退出",groupBox_2);
		pushButton_exit->setGeometry(QRect(300, 25, 80, 41));
		pushButton_exit->setStyleSheet("QPushButton {"
			"    background-color: #F44336;"
			"    color: white;"
			"    padding: 10px 20px;"
			"    border: 2px solid #000;"
			"    border-radius: 5px;"
			"    box - shadow: 2px 2px 5px rgba(0, 0, 0, 0.3);"
			"}"
			"QPushButton:hover {"
			"    background-color: #d32f2f;"
			"}");
		// 根据对象名自动连接信号和槽
		QMetaObject::connectSlotsByName(qt_elevator_osClass);
	} // setupUi   
};

// 命名空间 Ui 下的类，继承自 Ui_qt_elevator_osClass
namespace Ui {
	class qt_elevator_osClass : public Ui_qt_elevator_osClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_ELEVATOR_OS_H