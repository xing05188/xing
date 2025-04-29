import random
import gymnasium as gym
import numpy as np
from collections import deque
from keras.optimizers import Adam
from keras.models import Sequential
from keras.layers import Dense, Input
import os

# 定义训练的回合数，可根据实际情况调整该值以获得更好的训练效果
EPISODES = 1000

class DQNAgent:
    def __init__(self, state_size, action_size):
        # 状态空间的维度，用于确定神经网络输入层的大小
        self.state_size = state_size
        # 动作空间的维度，用于确定神经网络输出层的大小
        self.action_size = action_size
        # 经验回放的记忆缓冲区，使用双端队列存储过去的经验
        self.memory = deque(maxlen=2000)
        # 折扣因子，用于平衡即时奖励和未来奖励的重要性
        self.gamma = 0.95
        # 初始探索率，控制智能体进行随机探索的概率
        self.epsilon = 1.0
        # 最小探索率，防止智能体过早收敛到局部最优解
        self.epsilon_min = 0.001
        # 探索率衰减率，随着训练的进行逐渐降低探索率
        self.epsilon_decay = 0.995
        # 学习率，控制神经网络参数更新的步长
        self.learning_rate = 0.001
        # 构建神经网络模型
        self.model = self._build_model()
        self.target_model=self._build_model()
        self.update_target_model()
        self.target_update_frequency=10

    def _build_model(self):
        # 创建一个顺序模型
        model = Sequential()
        # 输入层，输入形状为状态空间的维度
        model.add(Input(shape=(self.state_size,)))
        # 第一个隐藏层，包含 24 个神经元，使用 ReLU 激活函数
        model.add(Dense(24, activation='relu'))
        # 第二个隐藏层，包含 24 个神经元，使用 ReLU 激活函数
        model.add(Dense(24, activation='relu'))
        # 输出层，神经元数量等于动作空间的维度，使用线性激活函数
        model.add(Dense(self.action_size, activation='linear'))
        # 编译模型，使用均方误差损失函数和 Adam 优化器
        model.compile(loss='mse', optimizer=Adam(learning_rate=self.learning_rate))
        return model

    def memorize(self, state, action, reward, next_state, done):
        # 将当前状态、动作、奖励、下一个状态和终止标志存储到记忆缓冲区
        self.memory.append((state, action, reward, next_state, done))

    def update_target_model(self):
        self.target_model.set_weights(self.model.get_weights())


    def act(self, state):
        # 根据探索率决定是随机选择动作还是根据模型预测选择动作
        if np.random.rand() <= self.epsilon:
            # 随机选择一个动作
            return random.randrange(self.action_size)
        # 使用模型预测每个动作的 Q 值
        act_values = self.model.predict(state, verbose=0)
        # 选择 Q 值最大的动作
        return np.argmax(act_values[0])

    def replay(self, batch_size):
        # 从记忆缓冲区中随机采样一个小批量的经验
        minibatch = random.sample(self.memory, batch_size)
        for state, action, reward, next_state, done in minibatch:
            # 初始化目标 Q 值为当前奖励
            target = reward
            if not done:
                # 如果回合未结束，使用贝尔曼方程计算目标 Q 值
                target = reward + self.gamma * np.amax(self.target_model.predict(next_state, verbose=0)[0])
            # 计算当前状态的Q值（仍使用评估网络）
            current_q_values = self.model.predict(state, verbose=0)
            current_q_values[0][action] = target  # 更新选中动作的Q值
            # 训练评估网络
            self.model.fit(state, current_q_values, epochs=1, verbose=0)
        # 如果探索率大于最小探索率，逐渐降低探索率
        if self.epsilon > self.epsilon_min:
            self.epsilon *= self.epsilon_decay

    def load(self, name):
        # 从指定文件加载模型权重
        self.model.load_weights(name)

    def save(self, name):
        # 将模型权重保存到指定文件
        self.model.save_weights(name)


if __name__ == "__main__":
    # 创建 CartPole-v1 环境
    env = gym.make('CartPole-v1')
    # 获取状态空间的维度
    state_size = env.observation_space.shape[0]
    # 获取动作空间的维度
    action_size = env.action_space.n
    # 初始化 DQN 智能体
    agent = DQNAgent(state_size, action_size)
    # 定义经验回放的批量大小
    batch_size = 32
    # 定义保存模型权重的目录
    save_dir = './save'
    if not os.path.exists(save_dir):
        # 如果目录不存在，则创建该目录
        os.makedirs(save_dir)

    for e in range(EPISODES):
        # 重置环境，获取初始状态
        state, _ = env.reset()
        # 将状态转换为适合模型输入的形状
        state = state.reshape(1, -1)
        total_reward = 0
        for time in range(500):
            # 根据当前状态选择动作
            action = agent.act(state)
            # 执行动作，获取下一个状态、奖励、终止标志和其他信息
            next_state, reward, terminated, truncated, _ = env.step(action)
            # 判断回合是否结束
            done = terminated or truncated
            # 如果回合结束，给予一个负奖励
            reward = reward if not done else -10
            total_reward += reward
            # 将下一个状态转换为适合模型输入的形状
            next_state = np.reshape(next_state, [1, state_size])
            # 将当前经验存储到记忆缓冲区
            agent.memorize(state, action, reward, next_state, done)
            # 更新当前状态为下一个状态
            state = next_state
            # 打印当前回合、时间步和状态信息
            print(f"Episode: {e + 1}/{EPISODES}, Time: {time}, State: {state[0]}, Total Reward: {total_reward}")
            if done:
                # 打印回合结束信息，包括回合数、得分和当前探索率
                print(f"episode: {e + 1}/{EPISODES}, score: {time}, e: {agent.epsilon:.2f}")
                agent.save(os.path.join(save_dir, f'cartpole-dqn_episode_{e + 1}.weights.h5'))
                break
            if len(agent.memory) > batch_size:
                # 如果记忆缓冲区中的样本数量超过批量大小，进行经验回放
                agent.replay(batch_size)
        if(e+1)%agent.target_update_frequency==0:
            agent.update_target_model()
            print(f"[INFO] Target network updated at episode {e + 1}")
        if (e + 1) % 10 == 0:
            # 每隔 10 个回合保存一次模型权重
            agent.save(os.path.join(save_dir, f'cartpole-dqn_episode_{e + 1}.weights.h5'))

    # 关闭环境
    env.close()
