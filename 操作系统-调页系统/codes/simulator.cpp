#include "simulator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QStatusBar>
#include <QFont>
#include <QApplication>
#include <QRandomGenerator>

PageReplacementSimulator::PageReplacementSimulator(QWidget* parent)
    : QMainWindow(parent)
{
    // 初始化UI
    initUI();
    QApplication::setFont(QFont("Microsoft YaHei", 12));  // 全局默认字体
    setWindowTitle("调页系统模拟器-2351289周慧星");
    resize(800, 600);
}

PageReplacementSimulator::~PageReplacementSimulator()
{
    delete timer;
}

void PageReplacementSimulator::initUI()
{
    // 主窗口布局
    QWidget* centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);  // 主布局

    // 左侧控制面板
    QGroupBox* controlPanel = new QGroupBox("控制面板");
    QVBoxLayout* controlLayout = new QVBoxLayout;  // 控制面板布局
    // 内存块大小旋转框
    memoryBlockSpinBox = new QSpinBox;
    memoryBlockSpinBox->setMinimum(2);
    memoryBlockSpinBox->setMaximum(8);
    memoryBlockSpinBox->setValue(memoryBlocks);
    memoryBlockSpinBox->setSingleStep(1);  // 每次调整1个内存块
    memoryBlockSpinBox->setStyleSheet("QSpinBox {padding: 5px; border-radius: 4px; border: 1px solid #aaa;}");
    connect(memoryBlockSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &PageReplacementSimulator::changeMemoryBlocks);
    controlLayout->addWidget(new QLabel("内存块数量:"));
    controlLayout->addWidget(memoryBlockSpinBox);
    // 指令数旋转框
    instructionCountSpinBox = new QSpinBox;
    instructionCountSpinBox->setMinimum(100);
    instructionCountSpinBox->setMaximum(500);
    instructionCountSpinBox->setValue(totalInstructions);
    instructionCountSpinBox->setSingleStep(10);  // 每次调整10条指令
    instructionCountSpinBox->setStyleSheet("QSpinBox {padding: 5px; border-radius: 4px; border: 1px solid #aaa;}");
    connect(instructionCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &PageReplacementSimulator::changeTotalInstructions);
    controlLayout->addWidget(new QLabel("指令总数:"));
    controlLayout->addWidget(instructionCountSpinBox);
    // 算法选择框
    algorithmCombo = new QComboBox;
    algorithmCombo->addItems({ "FIFO算法", "LRU算法","CLOCK算法","RANDOM算法"});    //算法往这加
    connect(algorithmCombo, &QComboBox::currentTextChanged, this, &PageReplacementSimulator::changeAlgorithm);
    controlLayout->addWidget(new QLabel("置换算法选择:"));
    controlLayout->addWidget(algorithmCombo);

    // 速度控制框
    speedCombo = new QComboBox;
    speedCombo->addItems({ "慢速", "中速", "快速" });
    speedCombo->setCurrentIndex(1);
    connect(speedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PageReplacementSimulator::changeSpeed);
    controlLayout->addWidget(new QLabel("模拟速度选择:"));
    controlLayout->addWidget(speedCombo);

    // 开始按钮
    startButton = new QPushButton("开始");
    startButton->setStyleSheet("QPushButton {"
        "    background-color: #4CAF50;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}");
    connect(startButton, &QPushButton::clicked, this, &PageReplacementSimulator::startSimulation);
    controlLayout->addWidget(startButton);
    // 暂停按钮
    pauseButton = new QPushButton("暂停");
    pauseButton->setStyleSheet("QPushButton {"
        "    background-color: #FF9800;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e68a00;"
        "}");
    connect(pauseButton, &QPushButton::clicked, this, &PageReplacementSimulator::pauseSimulation);
    pauseButton->setEnabled(false);
    controlLayout->addWidget(pauseButton);
    // 重置按钮
    resetButton = new QPushButton("重置");
    resetButton->setStyleSheet("QPushButton {"
        "    background-color: #F44336;"
        "}"
        "QPushButton:hover {"
        "    background-color: #d32f2f;"
        "}");
    connect(resetButton, &QPushButton::clicked, this, &PageReplacementSimulator::resetSimulation);
    controlLayout->addWidget(resetButton);

    // 统计信息
    QGroupBox* statsGroup = new QGroupBox("统计信息"); //统计信息组
    QVBoxLayout* statsLayout = new QVBoxLayout;      
    // 当前指令数
    instructionLCD = new QLCDNumber;
    instructionLCD->setDigitCount(3);          
    instructionLCD->setFixedSize(60, 35);     
    instructionLCD->setSegmentStyle(QLCDNumber::Flat);  
    instructionLCD->setStyleSheet("QLCDNumber {"
        "   background: black;color: red;"
        "}"
    );
    statsLayout->addWidget(new QLabel("已执行指令数:"));
    statsLayout->addWidget(instructionLCD);
    // 缺页次数
    pageFaultLCD = new QLCDNumber;
    pageFaultLCD->setDigitCount(3);
    pageFaultLCD->setFixedSize(60, 35);
    pageFaultLCD->setSegmentStyle(QLCDNumber::Flat);
    pageFaultLCD->setStyleSheet("QLCDNumber {"
        "   background: black;color: red;"
        "}"
    );
    statsLayout->addWidget(new QLabel("缺页次数:"));
    statsLayout->addWidget(pageFaultLCD);
    // 缺页率
    pageFaultRateLCD = new QLCDNumber;
    pageFaultRateLCD->setFixedSize(70, 40);
    pageFaultRateLCD->setSegmentStyle(QLCDNumber::Flat);
    pageFaultRateLCD->setStyleSheet("QLCDNumber {"
        "   background: black;color: red;"
        "}"
    );
    statsLayout->addWidget(new QLabel("缺页率:"));
    statsLayout->addWidget(pageFaultRateLCD);

    statsGroup->setLayout(statsLayout);
    controlLayout->addWidget(statsGroup);

    controlPanel->setLayout(controlLayout);
    mainLayout->addWidget(controlPanel, 1);

    // 右侧可视化区域
    QWidget* visualPanel = new QWidget;
    QVBoxLayout* visualLayout = new QVBoxLayout;

    // 内存块可视化
    memoryWidget = new MemoryWidget(memoryBlocks);   //4个
    visualLayout->addWidget(memoryWidget);

    // 当前指令信息
    currentInstructionLabel = new QLabel("准备开始模拟...");
    currentInstructionLabel->setAlignment(Qt::AlignCenter);
    visualLayout->addWidget(currentInstructionLabel);

    // 页表状态
    pageTableLabel = new QLabel("页表状态: 未初始化");
    pageTableLabel->setAlignment(Qt::AlignCenter);
    visualLayout->addWidget(pageTableLabel);

    visualPanel->setLayout(visualLayout);   // 可视化面板布局
    mainLayout->addWidget(visualPanel, 2);   // 加到主面板

    // 定时器
    timer = new QTimer(this);     
    connect(timer, &QTimer::timeout, this, &PageReplacementSimulator::stepSimulation);
}
//生成320条指令序列，随机生成
QVector<int> PageReplacementSimulator::generateInstructionSequence()
{
    QVector<int> sequence;
    int m = QRandomGenerator::global()->bounded(0, totalInstructions); // 随机起始点
    sequence.append(m);

    while (sequence.size() < totalInstructions) {
        // 顺序执行下一条(50%)
        if (sequence.size() < totalInstructions) {
            int nextInstr = (sequence.last() + 1) % totalInstructions;
            sequence.append(nextInstr);
        }
        // 前地址跳转(25%)
        if (sequence.size() < totalInstructions && QRandomGenerator::global()->generateDouble() < 0.5) {
            if (0 < sequence.last()) {
                int m1 = QRandomGenerator::global()->bounded(0, sequence.last());
                sequence.append(m1);
                if (sequence.size() < totalInstructions) {
                    int nextInstr = (sequence.last() + 1) % totalInstructions;
                    sequence.append(nextInstr);
                }
            }
        }
        // 后地址跳转(25%)
        if (sequence.size() < totalInstructions && QRandomGenerator::global()->generateDouble() < 0.5) {
            if (sequence.last() + 2 < totalInstructions) {
                int m2 = QRandomGenerator::global()->bounded(sequence.last() + 2, totalInstructions);
                sequence.append(m2);
                if (sequence.size() < totalInstructions) {
                    int nextInstr = (sequence.last() + 1) % totalInstructions;
                    sequence.append(nextInstr);
                }
            }  
        }
    }
    return sequence.mid(0, totalInstructions); // 确保正好320条
}
//改变内存块
void PageReplacementSimulator::changeMemoryBlocks(int value)
{
    if(value==memoryBlocks) return;  // 数量未变，直接返回
    memoryBlocks = value;
    if (!running) {
        memoryWidget->updateBlockCount(value);  // 更新内存块数量
    }
    statusBar()->showMessage(QString("内存块数量已改为: %1").arg(value), 2000);
}
//改变指令数
void PageReplacementSimulator::changeTotalInstructions(int value)
{
    if(value == totalInstructions) return;  // 数量未变，直接返回
    totalInstructions = value;
    totalPages = value / pageSize;
    statusBar()->showMessage(QString("指令数量已改为: %1").arg(value), 2000);
}
// 切换页面置换算法
void PageReplacementSimulator::changeAlgorithm(const QString& algo)
{
    algorithm = algo;
    statusBar()->showMessage(QString("算法已切换为: %1").arg(algo), 2000);
}
// 切换模拟速度
void PageReplacementSimulator::changeSpeed(int index)
{
    QVector<int> speeds = { 1000, 500, 200 }; // 定义速度数组，单位为毫秒
    speed = speeds[index];
    if (running) {
        timer->setInterval(speed);
        statusBar()->showMessage(QString("速度已改为: %1 ms").arg(speed), 2000);
    }
}
// 开始模拟
void PageReplacementSimulator::startSimulation()
{
    if (!running) {
        if (!stop) {
            memory.resize(memoryBlocks, -1); // 初始化内存块
            instructionSequence = generateInstructionSequence(); // 生成指令序列
            pageTable.resize(totalPages);  // 初始化页表
        }
        running = true;
        stop = false;
        startButton->setEnabled(false);
        pauseButton->setEnabled(true);
        timer->start(speed);
        statusBar()->showMessage("模拟已开始", 2000);
    }
}
// 暂停模拟
void PageReplacementSimulator::pauseSimulation()
{
    if (running) {
        running = false;
        stop = true;
        startButton->setEnabled(true);
        pauseButton->setEnabled(false);
        timer->stop();
        statusBar()->showMessage("模拟已暂停", 2000);
    }
}
// 重置模拟
void PageReplacementSimulator::resetSimulation()
{
    running = false;
    timer->stop();
    // 重置所有状态
    currentInstruction = 0;
    pageFaults = 0;
    clockHand = 0;
    pageTable.fill(PageTableEntry());
    memory.fill(-1);
    fifoQueue.clear();
    lruMap.clear();
    // 更新UI
    currentInstructionLabel->setText("准备开始模拟...");
    pageTableLabel->setText("页表状态: 未初始化");
    updateUI();
    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    statusBar()->showMessage("模拟已重置", 2000);
}
// 单步执行模拟
void PageReplacementSimulator::stepSimulation()
{
    if (currentInstruction >= totalInstructions) {
        pauseSimulation();
        statusBar()->showMessage("模拟完成!", 3000);
        return;
    }
    // 获取当前指令
    int instr = instructionSequence[currentInstruction];
    int page = instr / pageSize;
    int offset = instr % pageSize;
    // 检查页表
    if (!pageTable[page].valid) {
        // 缺页处理
        pageFaults++;
        handlePageFault(page);
    }
    if (algorithm == "CLOCK算法" && pageTable[page].valid) {
		// 如果是Clock算法，设置访问位
		pageTable[page].referenceBit = true;
	}
    // 更新LRU记录
    if (algorithm == "LRU算法" && pageTable[page].valid) {
        lruMap.erase(page);
        lruMap[page] = true;
    }
    // 计算物理地址
    int frame = pageTable[page].frame;
    int physicalAddr = frame * pageSize + offset;
    // 更新当前指令
    currentInstruction++;
    // 更新UI
    updateUI(instr, page, physicalAddr);
}
// 处理缺页异常
void PageReplacementSimulator::handlePageFault(int page)
{
    if (algorithm == "FIFO算法") {
        handlePageFaultFIFO(page);
    }
    else if(algorithm == "LRU算法") {
        handlePageFaultLRU(page);
    }
    else if (algorithm == "CLOCK算法") {
		handlePageFaultClock(page);  
	}
    else {
        handlePageFaultRANDOM(page);
    }
}

// 处理FIFO缺页异常
void PageReplacementSimulator::handlePageFaultFIFO(int page)
{
    if (fifoQueue.size() < memoryBlocks) {
        // 有空闲块
        int frame = int(fifoQueue.size());
        memory[frame] = page;
    }
    else {
        // 需要置换
        int victim = fifoQueue.front();
        fifoQueue.pop_front();
        int frame = pageTable[victim].frame;
        pageTable[victim].valid = false;
        memory[frame] = page;
    }
    fifoQueue.push_back(page);
    // 更新页表
    pageTable[page].valid = true;
    pageTable[page].frame = memory.indexOf(page);
}

// 处理LRU缺页异常
void PageReplacementSimulator::handlePageFaultLRU(int page)
{
    if (lruMap.size() < memoryBlocks) {
        // 有空闲块
        int frame = int(lruMap.size());
        memory[frame] = page;
    }
    else {
        // 需要置换
        int victim = lruMap.begin()->first;
        lruMap.erase(lruMap.begin());
        int frame = pageTable[victim].frame;
        pageTable[victim].valid = false;
        memory[frame] = page;
    }
    // 更新页表和LRU记录
    pageTable[page].valid = true;
    pageTable[page].frame = memory.indexOf(page);
    lruMap[page] = true;
}

// 处理Clock缺页异常
void PageReplacementSimulator::handlePageFaultClock(int page) {
	int Frame = memory.indexOf(-1); // 空闲帧的索引
    if (Frame == -1) {
		// 没有空闲帧，使用Clock算法
        while (true) {
            int currentPage= memory[clockHand]; // 当前指针指向的页面
            if (pageTable[currentPage].referenceBit) {
				// 如果访问位为1，清除访问位并移动指针
				pageTable[currentPage].referenceBit = false;
                clockHand = (clockHand + 1) % memoryBlocks;  //移动指针
			}
            else {
				pageTable[currentPage].valid = false;  // 清除旧页面的信息
				Frame = clockHand;  // 替换的帧位置
				break;
			}
			
		}
	}  
	// 设置新页面的信息
	memory[Frame] = page;
	pageTable[page].valid = true;
	pageTable[page].frame = Frame;
	pageTable[page].referenceBit = true;  // 设置访问位为1
	clockHand = (clockHand + 1) % memoryBlocks;  // 移动指针到下一个位置
}   

//RANDOM随机算法
void PageReplacementSimulator::handlePageFaultRANDOM(int page) {
    // 查找空闲帧
    int Frame = memory.indexOf(-1);
    if (Frame == -1) {
        // 没有空闲帧，随机选择一个页面进行替换
        Frame = QRandomGenerator::global()->bounded(0, memoryBlocks);
        int victimPage = memory[Frame];
        // 清除旧页面的信息
        pageTable[victimPage].valid = false;
    }  
    // 设置新页面的信息
    memory[Frame] = page;
    pageTable[page].valid = true;
    pageTable[page].frame = Frame;  
}
// 更新UI显示
void PageReplacementSimulator::updateUI(int currentInstr, int currentPage, int physicalAddr)
{
    // 更新内存块显示
    memoryWidget->updateBlocks(memory);
    // 更新统计信息
    instructionLCD->display(currentInstruction);
    pageFaultLCD->display(pageFaults);

    if (currentInstruction > 0) {
        double faultRate = (double(pageFaults) / currentInstruction) * 100;
        pageFaultRateLCD->display(QString::number(faultRate, 'f', 1));
    }
    else {
		pageFaultRateLCD->display(0.0);  // 初始时显示0.0
	}
    // 更新当前指令信息
    if (currentInstr != -1) {
        QString info = QString("当前指令: %1  (页: %2, 偏移: %3) ").arg(currentInstr).arg(currentPage).arg(currentInstr % pageSize);
        info += QString("\n物理地址: %1").arg(physicalAddr);
        if (!pageTable[currentPage].valid) {
            info += " [缺页]";
        }
        currentInstructionLabel->setText(info);
    }

    // 更新页表状态
    QString pageTableStatus = "页表状态:\n";
    for (int page = 0,i = 0; page < totalPages; ++page) {
        if (pageTable[page].valid) {
            i++;
            pageTableStatus += QString("页%1 → 块%2   ").arg(page).arg(pageTable[page].frame);
            if (i%4==0) {
                pageTableStatus += "\n";
            }
        }
    }
    pageTableLabel->setText(pageTableStatus.trimmed());
}