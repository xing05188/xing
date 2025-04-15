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

// �����ʾÿһ������ⲿ�İ�ť
// ������������ϵͳ��һЩ������������ѡ����ݡ����������

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
    // ָ�� UI �����ָ��
    Ui::building* ui;
public:
    // ���ݵ�������Ĭ��Ϊ 5
    int ELE_NUM = 5;
    // ¥���������Ĭ��Ϊ 20
    int FLOOR_NUM = 20;
    // ����ѡ��ģʽ��Ĭ��Ϊ 1
    int ELE_SELECT_MODE = 1;
    // �洢���ݶ���ָ�������
    std::vector<elevator*> eles;
    // �洢 LCD ��ʾ��ָ���������������ʾ������Ϣ
    std::vector<QLCDNumber* >   lcdDisplays;
    // �洢ÿһ�����ϰ�ťָ�������
    std::vector<QPushButton*> floorBtnsUp;
    // �洢ÿһ�����°�ťָ�������
    std::vector<QPushButton*> floorBtnsDown;
    // �㷨1���洢�������ֵ�������ÿ��Ԫ����һ�� pair���������ݱ�ź�����
    std::vector<std::pair<int, int>> eleRatings;
    //�㷨2������ÿ�����ݵ�Ŀǰ���ڽ��е�����
    std::vector<std::pair<int, int>> ele_dest_num;
public:
    /**
     * @brief ������ʱ���ĵδ���
     */
    void timer_building_tick();
    /**
     * @brief ����ָ�����ݵı�ǩ��Ϣ
     * @param i ���ݵ�����
     */
    void renew_label(unsigned int i);
    /**
     * @brief ѡ�񲢷��͵�������
     * @param up �Ƿ�Ϊ��������Ĭ��Ϊ true
     * @param floor �����¥�㣬Ĭ��Ϊ 1
     */
    void ele_select_send(bool up = true, int floor = 1);
    /**
     * @brief ������ݵ�����
     * @param reqUp �����Ƿ�Ϊ����
     * @param reqFloor �����¥��
     * @param eleFloor ���ݵ�ǰ���ڵ�¥��
     * @param eleStatus ���ݵĵ�ǰ״̬
     * @return ���ݵ�����
     */
    int  ele_rate(bool reqUp, int reqFloor, int eleFloor, int eleStatus, int ele_number);
    /**
     * @brief ���͵�������
     * @param up �Ƿ�Ϊ��������Ĭ��Ϊ true
     * @param floor �����¥�㣬Ĭ��Ϊ 1
     * @param ele Ҫ��������ĵ��ݶ���ָ�룬Ĭ��Ϊ nullptr
     * @param forceRecive �Ƿ�ǿ�Ƶ��ݽ�������Ĭ��Ϊ false
     * @return ��������ͳɹ����� true�����򷵻� false
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
