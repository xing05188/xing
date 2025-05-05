import os
import matplotlib.pyplot as plt
from DQN import DQN_Agent
import gymnasium as gym
import numpy as np
# 定义训练的回合数和每回合的最多执行步数
# 可根据实际情况调整该值以获得更好的训练效果
EPISODES = 1000
MAX_STEPS = 500

if __name__ == "__main__":
    # 创建 CartPole-v1 环境
    env = gym.make('CartPole-v1')
    # 获取环境状态的维度
    state_size = env.observation_space.shape[0]
    # 获取可采取的动作数量
    action_size = env.action_space.n
    # 创建 DQN 智能体
    agent = DQN_Agent(state_size, action_size)
    # 加载模型权重
    model_path = "./save/cartpole-dqn.weights.h5"
    if os.path.exists(model_path):
        agent.load(model_path)
        print(f"成功加载模型: {model_path}")
    else:
        print(f"模型文件 {model_path} 不存在，将从头开始训练。")
    # 小批量数据的大小
    batch_size = 32
    # 保存模型权重的目录
    save_dir = './save'
    # 如果保存目录不存在，则创建该目录
    if not os.path.exists(save_dir):
        os.makedirs(save_dir)
    # 存储每个回合的总奖励
    rewards = []
    # 存储每个回合的探索率
    epsilons = []
    # 存储每个回合的损失值
    losses = []
    # 打开文件用于写入信息
    log_file = open('training_log.txt', 'w')
    for episode in range(EPISODES):
        # 重置环境，获取初始状态
        state, _ = env.reset()
        # 将状态转换为适合模型输入的形状
        state = state.reshape(1, -1)
        # 总奖励初始化为 0
        total_reward = 0
        # 本回合的损失值初始化为 0
        total_loss = 0
        step_count = 0
        for time in range(MAX_STEPS):
            # 智能体选择下一个动作
            action = agent.select_next_action(state)
            # 执行动作，获取下一个状态、奖励、是否终止、是否截断的信息
            next_state, reward, terminated, truncated, _ = env.step(action)
            # 判断回合是否结束
            done = terminated or truncated
            # 如果回合结束，给予 -10 的奖励
            reward = reward if not done else -10
            # 累加总奖励
            total_reward += reward
            # 将下一个状态转换为适合模型输入的形状
            next_state = np.reshape(next_state, [1, state_size])
            # 将当前的状态、动作、奖励、下一个状态和是否结束的信息存入经验回放缓冲区
            agent.replay_memorize(state, action, reward, next_state, done)
            # 更新当前状态为下一个状态
            state = next_state
            # 打印当前回合、时间步、状态和总奖励的信息
            print(f"Episode: {episode + 1}/{EPISODES}, Time: {time}, State: {state[0]}")
            if len(agent.Replay_Buffer) > batch_size:
                # 记录当前的权重
                old_weights = agent.model.get_weights()
                # 如果经验回放缓冲区中的数据数量大于小批量数据的大小，则进行经验回放
                agent.experience_replay(batch_size)
                # 记录更新后的权重
                new_weights = agent.model.get_weights()
                loss = np.sum([np.sum((old - new) ** 2) for old, new in zip(old_weights, new_weights)])
                total_loss += loss
                step_count += 1

            if done:
                # 保存主模型的权重到指定文件
                agent.save(os.path.join(save_dir, f'cartpole-dqn.weights.h5'))
                # 打印当前回合的得分和探索率
                log_info = f"episode: {episode + 1}/{EPISODES}, reward: {total_reward}, epsilon: {agent.Epsilon:.3f}, loss:{total_loss}"
                print(log_info)
                # 将信息写入文件
                log_file.write(log_info + '\n')
                break

        if (episode + 1) % agent.target_update_frequency == 0:
            # 每达到目标模型更新的频率，更新目标模型的权重
            agent.update_target_model()

        # 记录当前回合的总奖励
        rewards.append(total_reward)
        # 记录当前回合的探索率
        epsilons.append(agent.Epsilon)
        # 记录当前回合的平均损失
        if step_count > 0:
            losses.append(total_loss / step_count)
        else:
            losses.append(0)

    # 关闭环境
    log_file.close()
    env.close()

    # 计算平均奖励，窗口大小设为 10
    window_size = 10
    moving_averages = []
    for i in range(len(rewards)):
        if i < window_size:
            moving_averages.append(np.mean(rewards[:i + 1]))
        else:
            moving_averages.append(np.mean(rewards[i - window_size + 1:i + 1]))

    # 绘制奖励曲线、平均奖励曲线、探索率曲线和损失曲线
    plt.figure(figsize=(12, 8))

    plt.subplot(2, 2, 1)
    plt.plot(rewards, label='Total_Rewards', alpha=0.7)
    plt.xlabel('Episode')
    plt.ylabel('Total_Reward')
    plt.title('Episode----Rewards')
    plt.legend()

    plt.subplot(2, 2, 2)
    plt.plot(moving_averages, label='Average_Reward', color='orange')
    plt.xlabel('Episode')
    plt.ylabel('Average Reward')
    plt.title('Episode----Average_Reward')
    plt.legend()

    plt.subplot(2, 2, 3)
    plt.plot(epsilons, label='Exploration Rate (Epsilon)', color='green')
    plt.xlabel('Episode')
    plt.ylabel('Epsilon')
    plt.title('Episode----Exploration Rate (Epsilon)')
    plt.legend()

    plt.subplot(2, 2, 4)
    plt.plot(losses, label='Loss', color='red')
    plt.xlabel('Episode')
    plt.ylabel('Loss')
    plt.title('Episode----Training Loss')
    plt.legend()

    plt.tight_layout()
    plt.show()