import random
import numpy as np
from collections import deque
from keras.optimizers import Adam
from keras.models import Sequential
from keras.layers import Dense, Input

class DQN_Agent:
    def __init__(self, state_size, action_size):
        # 状态的维度
        self.state_size = state_size
        # 动作的数量
        self.action_size = action_size
        # 经验回放缓冲区，使用双端队列，最大长度为2000
        self.Replay_Buffer = deque(maxlen=2000)
        # 折扣因子，用于计算未来奖励的折扣
        self.Gamma = 0.95
        # 探索率，初始值为1.0，意味着初始时完全随机探索
        self.Epsilon = 1.000
        # 探索率的最小值，当探索率降到该值后不再降低
        self.Epsilon_min = 0.001
        # 探索率的衰减率，每次经验回放后探索率乘以该值
        self.Epsilon_decay = 0.995
        # 学习率，用于优化模型的参数
        self.learning_rate = 0.0005
        # 主模型，用于预测动作价值
        self.model = self._build_model()
        # 目标模型，用于稳定训练过程
        self.target_model = self._build_model()
        # 初始化目标模型的权重与主模型相同
        self.update_target_model()
        # 目标模型更新的频率，每多少回合更新一次目标模型
        self.target_update_frequency = 10

    def _build_model(self):
        # 创建一个Sequential模型
        model = Sequential()
        # 输入层，指定输入的形状为状态的维度
        model.add(Input(shape=(self.state_size,)))
        # 第一个全连接层，有64个神经元，使用ReLU激活函数
        model.add(Dense(64, activation='relu'))
        # 第二个全连接层，有64个神经元，使用ReLU激活函数
        model.add(Dense(64, activation='relu'))
        # 输出层，神经元数量为动作的数量，使用线性激活函数
        model.add(Dense(self.action_size, activation='linear'))
        # 编译模型，使用均方误差损失函数和Adam优化器
        model.compile(loss='mse', optimizer=Adam(learning_rate=self.learning_rate))
        return model

    def replay_memorize(self, state, action, reward, next_state, done):
        # 将当前的状态、动作、奖励、下一个状态和是否结束的信息存入经验回放缓冲区
        self.Replay_Buffer.append((state, action, reward, next_state, done))

    def update_target_model(self):
        self.target_model.set_weights(self.model.get_weights())

    def select_next_action(self, state):
        # 如果随机数小于等于探索率，则随机选择一个动作
        if np.random.rand() <= self.Epsilon:
            return random.randrange(self.action_size)
        # 否则，使用主模型预测每个动作的价值
        q_values = self.model.predict(state, verbose=0)
        # 选择价值最大的动作的索引
        return np.argmax(q_values[0])

    def experience_replay(self, batch_size):
        # 从经验回放缓冲区中随机采样一个小批量的数据
        minibatch = random.sample(self.Replay_Buffer, batch_size)
        for state, action, reward, next_state, done in minibatch:
            # 目标值初始化为当前奖励
            td_target = reward
            if not done:
                # 如果回合未结束，计算目标值为当前奖励加上折扣后的未来最大奖励
                td_target = reward + self.Gamma * np.amax(self.target_model.predict(next_state, verbose=0)[0])
            # 使用主模型预测当前状态的动作价值
            current_q_values = self.model.predict(state, verbose=0)
            # 更新目标动作的价值为计算得到的目标值
            current_q_values[0][action] = td_target
            # 使用更新后的动作价值训练主模型
            self.model.fit(state, current_q_values, epochs=1, verbose=0)
        # 如果探索率大于最小值，则衰减探索率
        if self.Epsilon > self.Epsilon_min:
            self.Epsilon *= self.Epsilon_decay

    def load(self, name):
        # 从指定文件加载主模型的权重
        self.model.load_weights(name)

    def save(self, name):
        # 将主模型的权重保存到指定文件
        self.model.save_weights(name)


