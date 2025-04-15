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

	QWidget* elevatorContainer;    //���ݴ���
	std::vector<elevator*> ELEVATORS;  //�����Ӵ���
	building* BUILDING = nullptr;    //¥�㴰��

	void my_run();       //���вۺ���
	void my_stop();      //��ͣ�ۺ���

private:
    Ui::qt_elevator_osClass *ui;

protected:
	void closeEvent(QCloseEvent* event) { exit(0); } 
};

#endif // !QT_ELEVATOR_OS_H
