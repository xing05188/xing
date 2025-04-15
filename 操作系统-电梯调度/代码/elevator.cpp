#include "elevator.h"

// 电梯类的构造函数
elevator::elevator(QWidget* parent, int _no, int _FLOOR_NUM) : QWidget(parent), ui(new Ui::elevator) {
	// 设置用户界面
	ui->setupUi(this);
	// 保存电梯编号
	ele_number = _no;
	// 保存楼层总数
	FLOOR_NUM = _FLOOR_NUM;
	// 调整分组框的几何位置和大小
	ui->groupBox_ele_button->setGeometry(10, 10, 231,40+50 * ((FLOOR_NUM - 1) / 5 +1));
	ui->groupBox->setGeometry(10, 60 + 50 * ((FLOOR_NUM - 1) / 5 + 1), 231, 101);
	this->resize(250, ((FLOOR_NUM - 1) / 5 + 1) * 50 + 180);
	// 绘制目标楼层按钮
	for (int i = 0; i < FLOOR_NUM; i++) {
		// 创建一个新的按钮，并将其添加到分组框中
		QPushButton* btn = new QPushButton(ui->groupBox_ele_button);
		// 设置按钮的几何位置和大小
		btn->setGeometry(20 + 40 * (i % 5), 30 + 45 * (i / 5), 30, 30);
		// 设置按钮上显示的文本为楼层号
		btn->setText(QString::number(i + 1, 10));
		// 显示按钮
		btn->show();
		// 连接按钮的点击信号到一个 lambda 表达式
		connect(btn, &QPushButton::clicked, this, [=] {
			// 禁用该按钮
			Qbtns[unsigned(i)]->setEnabled(false);
			// 将该楼层添加到内部目标楼层列表中
			destsInsider.push_back(i);
			});
		// 将按钮添加到按钮列表中
		Qbtns.push_back(btn);
	}
	// 创建打开电梯门的 QPushButton
	QPushButton* pushButton_opendoor = new QPushButton(ui->groupBox_ele_button);
	QIcon icon1("image/open.png");
	pushButton_opendoor->setIcon(icon1);
	pushButton_opendoor->setIconSize(QSize(32, 32));
	pushButton_opendoor->setGeometry(QRect(20, 68 + 45 * ((FLOOR_NUM - 1 )/ 5), 32, 32));
	pushButton_opendoor->show();
	// 创建关闭电梯门的 QPushButton
	QPushButton* pushButton_closedoor = new QPushButton(ui->groupBox_ele_button);
	QIcon icon2("image/close.png");
	pushButton_closedoor->setIcon(icon2);
	pushButton_closedoor->setIconSize(QSize(32, 32));
	pushButton_closedoor->setGeometry(QRect(100, 68 + 45 * ((FLOOR_NUM - 1) / 5), 32, 32));
	pushButton_closedoor->show();
	// 创建触发电梯报警的 QPushButton
	QPushButton* pushButton_alert = new QPushButton(ui->groupBox_ele_button);
	QIcon icon3("image/alert.png");
	pushButton_alert->setIcon(icon3);
	pushButton_alert->setIconSize(QSize(32, 32));
	pushButton_alert->setGeometry(QRect(180, 68 + 45 * ((FLOOR_NUM - 1) / 5), 32, 32));
	pushButton_alert->show();

	// 创建一个定时器
	QTimer* timer = new QTimer(this);
	// 连接定时器的超时信号到 elevator 类的 timer_elevator_tick 槽函数
	connect(timer, &QTimer::timeout, this, &elevator::timer_elevator_tick);
	// 启动定时器，每隔 1500 毫秒触发一次
	timer->start(1500);

	QTimer* switchTimer = new QTimer(this);
	connect(switchTimer, &QTimer::timeout,this, &elevator::statu_change);
	switchTimer->start(200);

	// 连接开门按钮的点击信号到一个 lambda 表达式
	connect(pushButton_opendoor, &QPushButton::clicked, this, [=] {
		// 如果电梯处于静止状态，并且门是关闭或正在关闭状态，则打开门
		if (status == 0) { if (door == 0 || door == 2) open_door(); }
		else QMessageBox::about(nullptr, "错误!", "运行中无法开门.");
		});
	// 连接关门按钮的点击信号到一个 lambda 表达式
	connect(pushButton_closedoor, &QPushButton::clicked, this, [=] {
		// 如果门已经关闭或正在关闭，则弹出错误提示框
		if (door == 0 || door == 2) QMessageBox::about(nullptr, "错误!", "门已经关上了.");
		});
	// 连接报警按钮的点击信号到一个 lambda 表达式
	connect(pushButton_alert, &QPushButton::clicked, this, [=] { QMessageBox::about(nullptr, "警报!", "已发出警报."); });
}

// 电梯类的析构函数，释放用户界面指针
elevator::~elevator() {
	delete ui;
}

int elevator::dests_size() {
	return int(destsInsider.size()+ destsOutside.size());
}

void elevator::statu_change() {
	if (status == 0) {
		ui->label_status->setPixmap(status_stop);
	}
	else if (status == 1) {
		ui->label_status->setPixmap(status_up[sum]);
		sum = (sum + 1) % 3;
	}
	else {
		ui->label_status->setPixmap(status_down[sum]);
		sum = (sum + 1) % 3;
	}
}
// 设置电梯标签的文本，显示电梯编号
void elevator::set_title() {
	ui->label_ELE_NUMBER->setText("电梯" + QString::number(ele_number + 1));
}

// 打开电梯门的函数
void elevator::open_door() {
	QElapsedTimer t1,t2,t3;
	door = 3;
	renew_label();
	t1.start();
	while (t1.elapsed() < 1000) QCoreApplication::processEvents();
	door = 1;
	ui->label_door->setText(doorStr[1]);
	t2.start();
	while (t2.elapsed() < 1500) QCoreApplication::processEvents();
	door = 2;
	ui->label_door->setText(doorStr[2]);
	t3.start();
	while (t3.elapsed() < 1000) QCoreApplication::processEvents();
	door = 0;
	ui->label_door->setText(doorStr[0]);
}

// 更新电梯状态和显示信息的函数
void elevator::renew_label() {
	// 设置电梯状态标签的图片
	statu_change();
	// 设置电梯门状态标签的文本
	ui->label_door->setText(doorStr[door]);
	// 格式化当前楼层号，使其显示为两位数
	QString displayText = QString("%1").arg(currentFloor + 1, 2, 10, QChar('0'));
	// 在 LCD 显示屏上显示当前楼层号
	ui->lcdDisplay_1->display(displayText);
}

// 接收电梯请求的函数
bool elevator::recive_request(bool up, int floor, bool forceRecive) {
	// 如果不是强制接收请求，并且电梯的运行方向和请求方向不匹配，则拒绝请求
	if (!forceRecive && ((up && status == 2 && currentFloor > floor)
		|| (!up && status == 1 && currentFloor < floor)
		)) return false;
	// 检查该楼层是否已经在外部目标楼层列表中
	bool hasIn = false;
	for (auto i : destsOutside) if (i == floor) hasIn = true;
	// 如果不在列表中，则将该楼层添加到外部目标楼层列表中
	if (!hasIn) destsOutside.push_back(floor);

	// 根据电梯状态检查是否有新的任务
	status == 0 ? check_when_pause() : check_when_run();
	return true;
}

// 取消电梯请求的函数
// 参数:
// - floor: 要取消的请求楼层
void elevator::cancel_request(int floor) {
	// 在外部目标楼层列表中查找该楼层
	auto it = std::find(destsOutside.begin(), destsOutside.end(), floor);
	// 如果找到，则从列表中移除该楼层，并启用对应的按钮
	if (it != destsOutside.end()) {
		destsOutside.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}
}

// 电梯静止时检查任务的函数
void elevator::check_when_pause() {
	// 将内部和外部目标楼层列表合并到临时列表中
	dests.insert(dests.end(), destsInsider.begin(), destsInsider.end());
	dests.insert(dests.end(), destsOutside.begin(), destsOutside.end());
	// 如果临时列表为空，则直接返回
	if (dests.size() == 0) return;

	// 标记是否有上行和下行任务
	bool upDest = false;
	bool downDest = false;
	// 遍历临时列表，检查是否有上行和下行任务
	for (auto i : dests) {
		if (i < currentFloor) downDest = true;
		if (i > currentFloor) upDest = true;
		// 如果当前楼层有任务，则打开门
		if (i == currentFloor) open_door();
	}
	// 从内部目标楼层列表中移除当前楼层的任务，并启用对应的按钮
	auto it = std::find(destsInsider.begin(), destsInsider.end(), currentFloor);
	if (it != destsInsider.end()) {
		destsInsider.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}
	// 从外部目标楼层列表中移除当前楼层的任务，并启用对应的按钮
	it = std::find(destsOutside.begin(), destsOutside.end(), currentFloor);
	if (it != destsOutside.end()) {
		destsOutside.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}

	// 根据当前楼层和任务情况，设置电梯的运行状态
	if (currentFloor >= FLOOR_NUM / 2 && (upDest))                 status = 1;
	else if (currentFloor >= FLOOR_NUM / 2 && !upDest && downDest) status = 2;
	else if (currentFloor <= FLOOR_NUM / 2 && (downDest))          status = 2;
	else if (currentFloor <= FLOOR_NUM / 2 && upDest && !downDest) status = 1;
	// 清空临时列表
	dests.clear();
}

// 电梯运行时检查任务的函数
void elevator::check_when_run() {
	// 将内部和外部目标楼层列表合并到临时列表中
	dests.insert(dests.end(), destsInsider.begin(), destsInsider.end());
	dests.insert(dests.end(), destsOutside.begin(), destsOutside.end());
	// 如果临时列表为空，则将电梯状态设置为静止
	if (dests.size() == 0) {
		status = 0;
		return;
	}
	// 标记是否有上行和下行任务
	bool upDest = false;
	bool downDest = false;
	// 遍历临时列表，检查是否有上行和下行任务
	for (auto i : dests) {
		if (i < currentFloor) { downDest = true; }
		if (i > currentFloor) { upDest = true; }
		// 如果当前楼层有任务，则暂停电梯，打开门，然后恢复原状态
		if (i == currentFloor) {
			auto beforeStatus = status;
			status = 0;
			open_door();
			status = beforeStatus;
		}
	}
	// 从内部目标楼层列表中移除当前楼层的任务，并启用对应的按钮
	auto it = std::find(destsInsider.begin(), destsInsider.end(), currentFloor);
	if (it != destsInsider.end()) {
		destsInsider.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}
	// 从外部目标楼层列表中移除当前楼层的任务，并启用对应的按钮
	it = std::find(destsOutside.begin(), destsOutside.end(), currentFloor);
	if (it != destsOutside.end()) {
		destsOutside.erase(it);
		Qbtns[unsigned(currentFloor)]->setEnabled(true);
	}

	// 根据当前任务情况，调整电梯的运行状态
	if (status == 1 && !upDest && downDest)        status = 2;
	else if (status == 2 && upDest && !downDest) status = 1;
	else if (!upDest && !downDest)                status = 0;
	// 清空临时列表
	dests.clear();
}

// 定时器触发的函数，用于更新电梯的当前楼层和状态
void elevator::timer_elevator_tick() {
	// 根据电梯的运行状态更新当前楼层
	currentFloor += status == 1 ? 1 : status == 2 ? -1 : 0;
	// 根据电梯状态检查是否有新的任务
	status == 0 ? check_when_pause() : check_when_run();
	// 更新电梯的显示信息
	renew_label();
}

// 计算电梯完成当前任务队列中剩余任务所需时间的函数
int elevator::calculateRemainingTime() {
	// 用于累计剩余时间，初始化为0
	int time = 0;
	// 遍历内部目标楼层列表
	for (int dest : destsInsider) {
		// 累加从当前楼层到每个内部目标楼层的距离（假设每个任务从当前楼层移动到目标楼层需要1个时间单位）
		time += abs(dest - currentFloor);
	}
	// 遍历外部目标楼层列表
	for (int dest : destsOutside) {
		// 累加从当前楼层到每个外部目标楼层的距离
		time += abs(dest - currentFloor);
	}
	// 返回累计的剩余时间
	return time;
}