#include "qt_elevator_os.h"

// 构造函数，用于初始化主窗口
// 参数 parent 是父窗口指针，默认为 nullptr
qt_elevator_os::qt_elevator_os(QWidget* parent) : QMainWindow(parent), ui(new Ui::qt_elevator_osClass) {
    // 设置用户界面
    ui->setupUi(this);
    // 连接退出按钮的点击信号到一个 lambda 函数，点击时退出程序
    connect(ui->pushButton_exit, &QPushButton::clicked, this, [=] {exit(0); });
    // 连接运行按钮的点击信号到 my_run 槽函数
    connect(ui->pushButton_run, &QPushButton::clicked, this, &qt_elevator_os::my_run);
    // 连接停止按钮的点击信号到 my_stop 槽函数
    connect(ui->pushButton_stop, &QPushButton::clicked, this, &qt_elevator_os::my_stop);
    // 初始时停止按钮不可用
    ui->pushButton_stop->setEnabled(false);

    elevatorContainer = new QWidget(nullptr);
    elevatorContainer->setWindowTitle("2351289-周慧星-电梯");
    elevatorContainer->setWindowFlags(Qt::Window); // 设置为单独窗口
    elevatorContainer->hide();
}

// 析构函数，用于释放 ui 对象的内存
qt_elevator_os::~qt_elevator_os() {
    my_stop();
    delete ui;
    delete elevatorContainer;
}

// 运行按钮点击后的槽函数
void qt_elevator_os::my_run() {
    // 从 spinBox_floor 中获取用户输入的楼层数量
    int FLOOR_NUM = ui->spinBox_floor->value();
    // 从 spinBox_elevator 中获取用户输入的电梯数量
    int ELEVATOR_NUM = ui->spinBox_elevator->value();

    // 计算容器窗口的大小
    int containerWidth = ELEVATOR_NUM * 250 ;
    int containerHIGTH = ((FLOOR_NUM - 1) / 5 + 1) * 50 + 175;
    // 设置容器窗口的大小
    elevatorContainer->setGeometry(10, 40, containerWidth, containerHIGTH);

    elevatorContainer->show();
    ELEVATORS.clear();

    // 创建电梯
    for (int i = 0; i < ELEVATOR_NUM; i++) {
        elevator* ele = new elevator(elevatorContainer, i, FLOOR_NUM);
        // 计算电梯的位置并移动电梯窗口
        ele->move(i * 250 , 0);
        ele->set_title();
        // 显示电梯窗口
        ele->show();
        // 将电梯对象添加到电梯列表中
        ELEVATORS.push_back(ele);
    }

    // 创建建筑物对象
    building* bld = new building(nullptr, ELEVATORS, FLOOR_NUM, ui->comboBox->currentIndex() + 1);
    // 移动建筑物窗口到指定位置
    bld->move(100, 350);
    // 显示建筑物窗口
    bld->show();
    // 将建筑物对象指针赋值给成员变量 a_building
    BUILDING = bld;
    // 运行按钮点击后设置为不可用
    ui->pushButton_run->setEnabled(false);
    // 停止按钮设置为可用
    ui->pushButton_stop->setEnabled(true);
}

// 停止按钮点击后的槽函数
void qt_elevator_os::my_stop() {
    // 遍历电梯列表，删除每个电梯对象
    for (auto ele : ELEVATORS) {
        delete ele;
    }
    ELEVATORS.clear();
    // 删除建筑物对象
    if (BUILDING) {
        delete BUILDING;
        BUILDING= nullptr;
    }
    elevatorContainer->hide();
    // 运行按钮设置为可用
    ui->pushButton_run->setEnabled(true);
    // 停止按钮设置为不可用
    ui->pushButton_stop->setEnabled(false);
}
