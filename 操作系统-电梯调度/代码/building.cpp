#include "building.h"

building::building(QWidget* parent, std::vector<elevator*> _eles, int _FLOOR_NUM, int _ELE_SELECT_MODE) : QWidget(parent), ui(new Ui::building) {
	ui->setupUi(this);
	// 将传入的电梯指针向量赋值给类的成员变量
	eles = _eles;
	// 保存楼层总数
	FLOOR_NUM = _FLOOR_NUM;
	// 计算电梯的数量
	ELE_NUM = int(eles.size());
	// 保存电梯选择模式
	ELE_SELECT_MODE = _ELE_SELECT_MODE;
	// 设置全局字体
	QFont font("Arial", 11);
	this->setFont(font);

	// 绘制电梯相关的界面元素
	for (int i = 0; i < ELE_NUM; i++) {
		// 创建一个标签用于显示电梯编号
		QLabel* eleNo = new QLabel("电梯" + QString::number(i + 1), ui->groupBox_eles);
		eleNo->setGeometry(20 + 87 * i, 30, 40, 20);
		eleNo->setAlignment(Qt::AlignHCenter);
		eleNo->setStyleSheet("color: red; font-size: 16px;");
		// 显示标签
		eleNo->show();

		// 创建LCD数字显示
		QLCDNumber* lcdDisplay = new QLCDNumber(ui->groupBox_eles);
		lcdDisplay->setGeometry(20 + 87 * i, 55, 40, 40);
		lcdDisplay->setDigitCount(2);
		lcdDisplay->setSegmentStyle(QLCDNumber::Filled);
		lcdDisplay->setStyleSheet("QLCDNumber {"
			"background-color: black; color: red; "
			"border: 2px solid gray; "
			"border-radius: 5px; "
			"padding: 5px; "
			"box-shadow: 0 0 5px rgba(0, 0, 0, 0.5);"
			"}");
		// 格式化显示内容，确保显示两位数字，不足两位时前面补 0
		QString displayText = QString("%1").arg(1, 2, 10, QChar('0'));
		// 在LCD上显示当前楼层
		lcdDisplay->display(displayText);
		lcdDisplays.push_back(lcdDisplay);
	}

	// 绘制楼层按钮相关的界面元素
	for (int i = 0; i < FLOOR_NUM; i++) {
		// 创建一个标签用于显示楼层编号
		QLabel* floorNo = new QLabel(ui->groupBox_btns);
		floorNo->setGeometry(20 + 40 * (i % 10), 30 + 120 * (i / 10), 30, 30);
		floorNo->setAlignment(Qt::AlignHCenter);
		floorNo->setStyleSheet("color: #333333;");
		floorNo->setText(QString::number(i + 1, 10));
		// 显示标签
		floorNo->show();



		// 创建按钮
		QPushButton* floorBtnUp = new QPushButton(ui->groupBox_btns);
		QPushButton* floorBtnDown = new QPushButton(ui->groupBox_btns);
		// 设置按钮图标
		QIcon icon1("image/up.png");
		QIcon icon2("image/down.png");
		floorBtnUp->setIcon(icon1);
		floorBtnUp->setIconSize(QSize(32, 32)); // 设置图标大小
		floorBtnDown->setIcon(icon2);
		floorBtnDown->setIconSize(QSize(32, 32));
		// 设置按钮的位置和大小
		floorBtnUp->setGeometry(20 + 40 * (i % 10), 60 + 120 * (i / 10), 32, 32);
		floorBtnDown->setGeometry(20 + 40 * (i % 10), 100 + 120 * (i / 10), 32, 32);
		// 显示按钮
		floorBtnUp->show();
		floorBtnDown->show();
		// 将按钮添加到楼层上行按钮向量中
		floorBtnsUp.push_back(floorBtnUp);
		floorBtnsDown.push_back(floorBtnDown);
		// 连接按钮的点击信号到一个 lambda 函数，点击后禁用该按钮
		connect(floorBtnsUp[unsigned(i)], &QPushButton::clicked, this, [=] {floorBtnsUp[unsigned(i)]->setEnabled(false); });
		// 连接按钮的点击信号到 ele_select_send 函数，处理电梯调度请求
		connect(floorBtnsUp[unsigned(i)], &QPushButton::clicked, this, [=] {ele_select_send(true, i); });
		connect(floorBtnsDown[unsigned(i)], &QPushButton::clicked, this, [=] {floorBtnsDown[unsigned(i)]->setEnabled(false); });
		connect(floorBtnsDown[unsigned(i)], &QPushButton::clicked, this, [=] {ele_select_send(false, i); });
	}
	// 一楼不需要下行按钮，隐藏一楼的下行按钮
	floorBtnsDown[0]->hide();
	// 顶层不需要上行按钮，隐藏顶层的上行按钮
	floorBtnsUp[unsigned(FLOOR_NUM) - 1]->hide();

	// 根据楼层数量调整楼层按钮分组框的高度
	ui->groupBox_btns->setGeometry(10, 120, 430, 20 + 120 * ((FLOOR_NUM - 1) / 10 + 1));
	// 根据电梯数量和楼层数量调整状态标签的位置和大小
	ui->label_bar->setGeometry(10, 150 + 120 * ((FLOOR_NUM - 1) / 10 + 1), 430, 20);
	// 根据电梯数量和楼层数量调整窗口的大小
	this->resize(450, 180 + 120 * ((FLOOR_NUM - 1) / 10 + 1));

	// 创建一个定时器，每 100 毫秒触发一次 timeout 信号
	QTimer* timer = new QTimer(this);
	// 连接定时器的 timeout 信号到 timer_building_tick 函数
	connect(timer, &QTimer::timeout, this, &building::timer_building_tick);
	// 启动定时器
	timer->start(100);
}

// 析构函数，释放 ui 对象的内存
building::~building() {
	delete ui;
}

// 定时器触发的函数，每 100 毫秒调用一次，用于更新界面和处理电梯到达事件
void building::timer_building_tick() {
	for (unsigned int i = 0; i < unsigned(ELE_NUM); i++) {
		// 更新电梯的滑块和当前楼层标签的显示
		renew_label(i);
		// 如果电梯处于空闲状态（到达目标楼层）
		if (eles[i]->status == 0) {
			// 启用该楼层的上行和下行按钮
			floorBtnsUp[unsigned(eles[i]->currentFloor)]->setEnabled(true);
			floorBtnsDown[unsigned(eles[i]->currentFloor)]->setEnabled(true);
		}
	}
}
// 参数：
// i: 电梯的索引
void building::renew_label(unsigned int i) {
	// 设置当前楼层标签的文本为电梯的当前楼层加 1
	QString displayText = QString("%1").arg(eles[i]->currentFloor + 1, 2, 10, QChar('0'));
	lcdDisplays[i]->display(displayText);
}

bool building::send_request(bool up, int floor, elevator* ele, bool forceRecive) {
	return(ele->recive_request(up, floor, forceRecive));
}

int building::ele_rate(bool reqUp, int reqFloor, int eleFloor, int eleStatus,int ele_number) {
	// 电梯负载评分计算
	double loadRating = static_cast<double>(eles[ele_number]->dests_size()) / (ELE_NUM * FLOOR_NUM);

	// 优化距离评分计算，使用指数函数加大距离影响
	double distanceRating = std::pow(1.1, std::abs(eleFloor - reqFloor)) / std::pow(1.1, FLOOR_NUM);
	if (eleStatus == 0) distanceRating *= 3;

	// 计算状态评分，逻辑不变
	double statusRating = eleStatus == 0 ? 1.0 : reqUp ? eleStatus == 1 ? eleFloor < reqFloor ? 1.0 : 0.2
		: eleFloor < reqFloor ? 0.6 : 0.4
		: eleStatus == 2 ? eleFloor > reqFloor ? 1.0 : 0.2
		: eleFloor > reqFloor ? 0.6 : 0.4;

	// 综合各项评分，返回最终评分
	return static_cast<int>(100.0 * (distanceRating * 0.3 + statusRating * 0.4 + loadRating * 0.3));
}


//算法部分;
// 处理电梯调度请求，根据不同的选择模式选择合适的电梯
// 参数：
// up: 是否是上行请求
// floor: 请求的楼层
void building::ele_select_send(bool up, int floor) {
	// 在状态标签上显示开始处理请求的信息
	ui->label_bar->setText("开始处理来自" + QString::number(floor + 1, 10) + "层的电梯调度请求...");
	if (ELE_SELECT_MODE == 1) {
		LOOKAlgorithm(up, floor);
	}
	else if (ELE_SELECT_MODE == 2) {
		ShortestRemainingTimeFirst(up, floor);
	}
	else if (ELE_SELECT_MODE == 3) {
		GroupScheduling(up, floor);
	}
	else if (ELE_SELECT_MODE == 4) {
		ALGORITHM1(up, floor);
	}
	else if (ELE_SELECT_MODE == 5) {
		ALGORITHM2(up, floor);
	}
	else {
		ALGORITHM3(up, floor);
	}
}

//算法1，FCFS,将任务分配给评分最优的电梯
void building::ALGORITHM1(bool up, int floor) {
	// 清空电梯评分向量
	eleRatings.clear();
	// 计算每部电梯的评分
	for (int i = 0; i < ELE_NUM; i++)
		eleRatings.push_back({ i, ele_rate(up, floor, eles[unsigned(i)]->currentFloor, eles[unsigned(i)]->status,i) });
	// 对电梯评分向量进行排序，评分低的在前
	std::sort(eleRatings.begin(), eleRatings.end(),
		[](std::pair<int, int>& a, std::pair<int, int>& b) {
			return a.second < b.second;
		});
	// 标记是否成功发送请求
	bool successSend = false;
	// 遍历排序后的电梯评分向量，尝试向电梯发送请求
	for (auto i : eleRatings) {
		if (send_request(up, floor, eles[unsigned(i.first)])) {
			successSend = true;
			// 在状态标签上显示已调度电梯的信息
			ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(i.first + 1, 10) + "号电梯.");
			return;
		}
	}
	// 如果所有电梯都拒绝请求，强制调度评分最低的电梯
	if (successSend == false) {
		send_request(up, floor, eles[unsigned(eleRatings.begin()->first)], true);
		// 在状态标签上显示强制调度电梯的信息
		ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(eleRatings.begin()->first + 1, 10) + "号电梯.");
	}
}

//算法2，FCFS,将当前任务分配给任务最少的电梯
void building::ALGORITHM2(bool up, int floor) {
	ele_dest_num.clear();
	for (int i = 0; i < ELE_NUM; i++)
		ele_dest_num.push_back({ i, eles[i]->dests_size() });
	std::sort(ele_dest_num.begin(), ele_dest_num.end(),
		[](std::pair<int, int>& a, std::pair<int, int>& b) {
			return a.second < b.second;
		});
	// 标记是否成功发送请求
	bool successSend = false;
	for (auto i : ele_dest_num) {
		if (send_request(up, floor, eles[unsigned(i.first)])) {
			successSend = true;
			// 在状态标签上显示已调度电梯的信息
			ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(i.first + 1, 10) + "号电梯.");
			return;
		}
	}
	if (successSend == false) {
		send_request(up, floor, eles[unsigned(ele_dest_num.begin()->first)], true);
		ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(ele_dest_num.begin()->first + 1, 10) + "号电梯.");
	}
}

//算法3，FCFS,顺序分配
void building::ALGORITHM3(bool up, int floor) {
	// 标记是否成功发送请求
	bool successSend = false;
	for (int i = 0; i < ELE_NUM;i++) {
		if (send_request(up, floor, eles[i])) {
			successSend = true;
			// 在状态标签上显示已调度电梯的信息
			ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(i + 1, 10) + "号电梯.");
			return;
		}
	}
	if (!successSend) {
		send_request(up, floor, eles[0], true);
		ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度0号电梯.");
	}
}

// LOOK调度算法实现函数
void building::LOOKAlgorithm(bool up, int floor) {
	// 用于记录最近电梯的索引，初始化为-1
	int closestElevatorIndex = -1;
	// 用于记录最近电梯与请求楼层的最小距离，初始化为最大整数值
	int minDistance = INT_MAX;
	// 遍历所有电梯
	for (int i = 0; i < ELE_NUM; ++i) {
		// 获取当前电梯对象
		elevator* ele = eles[i];
		// 如果电梯处于空闲状态
		if (ele->status == 0) {
			// 计算当前电梯与请求楼层的距离
			int distance = abs(ele->currentFloor - floor);
			// 如果该距离小于当前最小距离
			if (distance < minDistance) {
				// 更新最小距离
				minDistance = distance;
				// 更新最近电梯的索引
				closestElevatorIndex = i;
			}
		}
		// 如果电梯正在运行，且运行方向与请求方向一致
		else if ((up && ele->status == 1 && ele->currentFloor <= floor) || (!up && ele->status == 2 && ele->currentFloor >= floor)) {
			// 计算当前电梯与请求楼层的距离
			int distance = abs(ele->currentFloor - floor);
			// 如果该距离小于当前最小距离
			if (distance < minDistance) {
				// 更新最小距离
				minDistance = distance;
				// 更新最近电梯的索引
				closestElevatorIndex = i;
			}
		}
	}
	// 如果找到了合适的电梯
	if (closestElevatorIndex != -1) {
		// 向该电梯发送请求
		send_request(up, floor, eles[closestElevatorIndex]);
		// 在状态标签上显示已调度电梯的信息
		ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(closestElevatorIndex + 1, 10) + "号电梯.");
	}
	else {
		// 若未找到合适电梯，强制调度第一部电梯
		send_request(up, floor, eles[0], true);
		// 在状态标签上显示强制调度电梯的信息
		ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度0号电梯.");
	}
}



// 最短剩余时间优先调度算法实现函数
void building::ShortestRemainingTimeFirst(bool up, int floor) {
	// 用于存储每个电梯的索引及其剩余时间的向量
	std::vector<std::pair<int, int>> elevatorTimes;
	// 遍历所有电梯
	for (int i = 0; i < ELE_NUM; ++i) {
		// 计算当前电梯的剩余时间
		int remainingTime = eles[i]->calculateRemainingTime();
		// 将电梯索引和剩余时间存储到向量中
		elevatorTimes.push_back({ i, remainingTime });
	}
	// 根据剩余时间对电梯进行排序，剩余时间短的在前
	std::sort(elevatorTimes.begin(), elevatorTimes.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
		return a.second < b.second;
		});
	// 标记是否成功发送请求，初始化为false
	bool successSend = false;
	// 遍历排序后的电梯列表
	for (auto i : elevatorTimes) {
		// 向当前电梯发送请求
		if (send_request(up, floor, eles[unsigned(i.first)])) {
			// 如果请求发送成功，更新标记
			successSend = true;
			// 在状态标签上显示已调度电梯的信息
			ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(i.first + 1, 10) + "号电梯.");
			// 返回，结束调度
			return;
		}
	}
	// 如果所有电梯都拒绝请求
	if (!successSend) {
		// 强制调度剩余时间最短的电梯
		send_request(up, floor, eles[unsigned(elevatorTimes.begin()->first)], true);
		// 在状态标签上显示强制调度电梯的信息
		ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(elevatorTimes.begin()->first + 1, 10) + "号电梯.");
	}
}


// 分组调度算法实现函数
void building::GroupScheduling(bool up, int floor) {
	// 计算中间楼层，用于划分高低层
	const int middleFloor = FLOOR_NUM / 2;
	// 用于存储目标电梯组的向量
	std::vector<int> targetGroup;
	// 如果请求楼层在中层及以下
	if (floor <= middleFloor) {
		// 遍历所有电梯
		for (int i = 0; i < ELE_NUM; ++i) {
			// 如果电梯当前楼层在中层及以下，将其加入目标组
			if (eles[i]->currentFloor <= middleFloor) {
				targetGroup.push_back(i);
			}
		}
	}
	// 如果请求楼层在中层以上
	else {
		// 遍历所有电梯
		for (int i = 0; i < ELE_NUM; ++i) {
			// 如果电梯当前楼层在中层以上，将其加入目标组
			if (eles[i]->currentFloor > middleFloor) {
				targetGroup.push_back(i);
			}
		}
	}
	// 标记是否成功发送请求，初始化为false
	bool successSend = false;
	// 遍历目标电梯组
	for (const auto& i : targetGroup) {
		// 向当前电梯发送请求
		if (send_request(up, floor, eles[i])) {
			// 如果请求发送成功，更新标记
			successSend = true;
			// 在状态标签上显示已调度电梯的信息
			ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(i + 1, 10) + "号电梯.");
			// 返回，结束调度
			return;
		}
	}
	// 如果目标组内无电梯响应请求
	if (!successSend) {
		// 从其他组或全部电梯中选择
		for (int i = 0; i < ELE_NUM; ++i) {
			// 向当前电梯发送请求
			if (send_request(up, floor, eles[i])) {
				// 如果请求发送成功，更新标记
				successSend = true;
				// 在状态标签上显示已调度电梯的信息
				ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度" + QString::number(i + 1, 10) + "号电梯.");
				// 返回，结束调度
				return;
			}
		}
		// 如果所有电梯都无响应，强制调度第一部电梯
		send_request(up, floor, eles[0], true);
		// 在状态标签上显示强制调度电梯的信息
		ui->label_bar->setText("已为来自" + QString::number(floor + 1, 10) + "层的请求调度0号电梯.");
	}
}

