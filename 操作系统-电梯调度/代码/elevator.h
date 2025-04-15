#ifndef ELEVATOR_H
#define ELEVATOR_H

// 包含必要的标准库和Qt库头文件
#include <vector>
#include <algorithm>
#include <QTimer>
#include <QWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <QPushButton>
#include <QElapsedTimer>
#include <QPixmap>
// 包含自动生成的UI头文件
#include "ui_elevator.h"

// 注释说明：该类表示电梯内部的控制面板
namespace Ui { class elevator; }

// 定义电梯类，继承自QWidget
class elevator : public QWidget {
    // 声明使用Qt的信号和槽机制
    Q_OBJECT
public:
    explicit elevator(QWidget* parent = nullptr, int _no = 0, int _FLOOR_NUM = 20);
    // 析构函数，用于释放对象占用的资源
    ~elevator();
private:
    // 指向自动生成的UI对象的指针
    Ui::elevator* ui;
public:
    // 电梯的编号，用于唯一标识该电梯
    int ele_number = 0;
    // 电梯门的状态，取值范围及含义如下：
    int door = 0;
    // 电梯的运行状态，取值范围及含义如下：
    int status = 0;
    // 电梯当前所在的楼层
    int currentFloor = 0;
    // 建筑物的总楼层数
    int FLOOR_NUM = 20;

    int sum = 0;
    // 存储电梯不同状态对应的图片
    //上升
    QPixmap status_up[3] = {
        QPixmap(QString("image/up1.png")),
        QPixmap(QString("image/up2.png")),
        QPixmap(QString("image/up3.png"))
    };
    //下降
    QPixmap status_down[3] = {
        QPixmap(QString("image/down1.png")),
        QPixmap(QString("image/down2.png")),
        QPixmap(QString("image/down3.png"))
    };
    //暂停
    QPixmap status_stop= QPixmap(QString("image/stop.png"));
    // 存储电梯门不同状态对应的字符串描述
    // doorStr[0] 对应门关闭状态的描述
    // doorStr[1] 对应门打开状态的描述
    // doorStr[2] 对应门正在关闭状态的描述
    // doorStr[3] 对应门正在打开状态的描述
    QString doorStr[4] = { "已关门", "已开门", "关门中", "开门中" };
    // 电梯的所有目标楼层，由内部请求和外部请求的目标楼层合并而成
    std::vector<int> dests;
    // 电梯内部乘客按下按钮请求到达的目标楼层
    std::vector<int> destsInsider;
    // 电梯外部发出的请求到达的目标楼层，通过recive_request()函数添加
    std::vector<int> destsOutside;
    // 存储窗口上的按钮指针，用于后续操作按钮
    std::vector<QPushButton*> Qbtns;
public:
    int dests_size();
    // 设置窗口标题的函数
    void set_title();
    // 打开电梯门的函数
    void open_door();
    // 更新标签显示信息的函数
    void renew_label();
    // 电梯运行时进行检查的函数
    void check_when_run();
    // 电梯暂停时进行检查的函数
    void check_when_pause();
    // 定时器定时触发的函数，每1500毫秒执行一次
    void timer_elevator_tick();
    void statu_change();
    // 取消指定楼层请求的函数
    void cancel_request(int floor);
    // 接收来自建筑物外部的任务请求，并将其添加到外部目标楼层列表中
    // 参数说明：
    // - up：请求的方向，true表示上行，false表示下行，默认为true
    // - floor：请求的楼层，默认为1
    // - forceRecive：是否强制接收请求，默认为false
    // 说明：具体可参考类 "building" 中的 "send_request()" 函数
    bool recive_request(bool up = true, int floor = 1, bool forceRecive = false);


    int calculateRemainingTime();
};

// 结束头文件保护
#endif // ELEVATOR_H
