#ifndef QT_ELEVATOR_OS_H
#define QT_ELEVATOR_OS_H

#include"Windows.h"
#include"building.h"
#include<QCloseEvent>
#include <QtWidgets/QMainWindow>
#include "ui_qt_elevator_os.h"

namespace Ui { class qt_elevator_osClass; }

class qt_elevator_os : public QMainWindow
{
    Q_OBJECT

public:
    qt_elevator_os(QWidget *parent = nullptr);
    ~qt_elevator_os();

	QWidget* elevatorContainer;    //电梯窗口
	std::vector<elevator*> ELEVATORS;  //电梯子窗口
	building* BUILDING = nullptr;    //楼层窗口

	void my_run();       //运行槽函数
	void my_stop();      //暂停槽函数

private:
    Ui::qt_elevator_osClass *ui;

protected:
	void closeEvent(QCloseEvent* event) { exit(0); } 
};

#endif // !QT_ELEVATOR_OS_H
