#ifndef BUILDING_H
#define BUILDING_H

#include "elevator.h"
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <QLabel>
#include <QString>
#include <QSlider>
#include <QStatusBar>
#include "ui_building.h"

// 该类表示每一层电梯外部的按钮
// 它负责管理电梯系统的一些基本操作，如选择电梯、发送请求等

namespace Ui { class building; }

class building : public QWidget {
    Q_OBJECT
public:
   
    explicit building(QWidget* parent = nullptr,
        std::vector<elevator*> _eles = {},
        int _FLOOR_NUM = 20, int _ELE_SELECT_MODE = 1
    );
   
    ~building();
private:
    // 指向 UI 界面的指针
    Ui::building* ui;
public:
    // 电梯的数量，默认为 5
    int ELE_NUM = 5;
    // 楼层的总数，默认为 20
    int FLOOR_NUM = 20;
    // 电梯选择模式，默认为 1
    int ELE_SELECT_MODE = 1;
    // 存储电梯对象指针的向量
    std::vector<elevator*> eles;
    // 存储 LCD 显示屏指针的向量，用于显示电梯信息
    std::vector<QLCDNumber* >   lcdDisplays;
    // 存储每一层向上按钮指针的向量
    std::vector<QPushButton*> floorBtnsUp;
    // 存储每一层向下按钮指针的向量
    std::vector<QPushButton*> floorBtnsDown;
    // 算法1，存储电梯评分的向量，每个元素是一个 pair，包含电梯编号和评分
    std::vector<std::pair<int, int>> eleRatings;
    //算法2，储存每个电梯的目前正在进行的任务
    std::vector<std::pair<int, int>> ele_dest_num;
public:
    /**
     * @brief 建筑定时器的滴答函数
     */
    void timer_building_tick();
    /**
     * @brief 更新指定电梯的标签信息
     * @param i 电梯的索引
     */
    void renew_label(unsigned int i);
    /**
     * @brief 选择并发送电梯请求
     * @param up 是否为向上请求，默认为 true
     * @param floor 请求的楼层，默认为 1
     */
    void ele_select_send(bool up = true, int floor = 1);
    /**
     * @brief 计算电梯的评分
     * @param reqUp 请求是否为向上
     * @param reqFloor 请求的楼层
     * @param eleFloor 电梯当前所在的楼层
     * @param eleStatus 电梯的当前状态
     * @return 电梯的评分
     */
    int  ele_rate(bool reqUp, int reqFloor, int eleFloor, int eleStatus, int ele_number);
    /**
     * @brief 发送电梯请求
     * @param up 是否为向上请求，默认为 true
     * @param floor 请求的楼层，默认为 1
     * @param ele 要发送请求的电梯对象指针，默认为 nullptr
     * @param forceRecive 是否强制电梯接收请求，默认为 false
     * @return 如果请求发送成功返回 true，否则返回 false
     */
    bool send_request(bool up = true, int floor = 1, elevator* ele = nullptr, bool forceRecive = false);

    void ALGORITHM1(bool up, int floor);
    void ALGORITHM2(bool up, int floor);
    void ALGORITHM3(bool up, int floor);
    void LOOKAlgorithm(bool up, int floor);
   
    void ShortestRemainingTimeFirst(bool up, int floor);
    void GroupScheduling(bool up, int floor);
};

#endif // BUILDING_H
