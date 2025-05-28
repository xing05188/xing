#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <deque>
#include <map>
#include "memorywidget.h"
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLCDNumber>
#include <QSpinBox>

QT_BEGIN_NAMESPACE
class QComboBox;
class QPushButton;
class QLabel;
class QLCDNumber;
QT_END_NAMESPACE

class PageReplacementSimulator : public QMainWindow
{
    Q_OBJECT

public:
    PageReplacementSimulator(QWidget* parent = nullptr);
    ~PageReplacementSimulator();

private slots:
    void changeMemoryBlocks(int value);
    void changeTotalInstructions(int value);
    void changeAlgorithm(const QString& algo);   // 切换页面置换算法
    void changeSpeed(int index);             // 切换模拟速度
    void startSimulation();      // 开始模拟
    void pauseSimulation();     // 暂停模拟
    void resetSimulation();     // 重置模拟
    void stepSimulation();      // 单步执行模拟

private:
    void initUI();
    QVector<int> generateInstructionSequence();  // 生成指令序列
    void handlePageFault(int page);         // 处理缺页异常
    void handlePageFaultFIFO(int page);     // 处理FIFO缺页异常，算法
    void handlePageFaultLRU(int page);      // 处理LRU缺页异常，算法
    void handlePageFaultClock(int page);    // 处理Clock缺页异常，算法   
    void handlePageFaultRANDOM(int page);   // 处理RANDOM随机缺页异常，算法
    void updateUI(int currentInstr = -1, int currentPage = -1, int physicalAddr = -1);// 更新UI显示

    // 模拟参数
    int totalInstructions = 320;     // 总指令数
    const int pageSize = 10;              // 页面大小
    int totalPages = totalInstructions / pageSize;   //所需页数
    int memoryBlocks = 4;    //分配的页数
    int currentInstruction = 0;    // 当前指令
    int pageFaults = 0;            // 缺页次数
    QString algorithm = "FIFO算法";    // 当前算法
    bool running = false;           // 模拟是否在运行
    bool stop = false;       // 是否停止模拟
    int speed = 500; // ms         // 模拟速度

    // 数据结构
    struct PageTableEntry {   // 页表项
        bool valid = false;
        int frame = -1;   //块号
        bool referenceBit = false;  // 访问位，用于Clock算法
    };
    int clockHand=0;  // Clock算法的指针
    QVector<PageTableEntry> pageTable;  // 页表
    QVector<int> memory;            // 物理内存
    std::deque<int> fifoQueue;     // FIFO队列
    std::map<int, bool> lruMap;    // LRU映射
    QVector<int> instructionSequence;   // 指令序列

    // UI组件
    MemoryWidget* memoryWidget;
    QSpinBox* memoryBlockSpinBox;
    QSpinBox* instructionCountSpinBox;
    QComboBox* algorithmCombo;
    QComboBox* speedCombo;
    QPushButton* startButton;
    QPushButton* pauseButton;
    QPushButton* resetButton;
    QLCDNumber* instructionLCD;
    QLCDNumber* pageFaultLCD;
    QLCDNumber* pageFaultRateLCD;
    QLabel* currentInstructionLabel;
    QLabel* pageTableLabel;
    QTimer* timer;
};

#endif 
