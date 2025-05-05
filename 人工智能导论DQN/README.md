# 深度Q网络（DQN）实现

## 一、项目概述
本项目基于深度Q网络（DQN）算法，实现了一个智能体来解决OpenAI Gym中的`CartPole-v1`环境问题。DQN算法结合了深度学习和Q学习，利用神经网络近似Q值，并通过经验回放机制提高训练效率。

## 二、环境要求
在运行程序之前，你需要确保已经安装了必要的Python库。本程序主要依赖于以下库：
- **Python版本**：Python 3.x
- **Python库**：
    - `gymnasium`：用于创建和管理强化学习环境。
    - `numpy`：用于数值计算。
    - `keras`：用于构建和训练神经网络。
    - `tensorflow`：作为`keras`的后端引擎。
    - `random`：用于随机数生成。
    - `matplotlib`：用于绘制训练过程中的统计图表。
    - `os`：用于文件和目录操作。

## 三、安装依赖
在命令行中执行以下命令来安装所需的依赖库：
```bash
pip install gymnasium numpy keras tensorflow matplotlib "gymnasium[classic-control]"
```
这里需要注意的是，`keras` 通常是基于 `tensorflow` 后端运行的，所以也需要安装 `tensorflow`。"gymnasium[classic-control]"好像不会主动装，所以额外装。

## 四、代码结构
### 文件结构
项目主要包含3个Python文件 `DQN.py`，`train.py`，`test.py`。

## 五、运行程序
1. **训练**：在命令行中执行以下命令启动训练：
```bash
python train.py
```
- **模型保存**：每隔10个回合，模型的权重将被保存到 `./save` 目录下，文件名格式为 `cartpole-dqn_episode_<episode_number>.weights.h5`。
- **结果输出**：训练过程中会打印每个回合的得分、探索率和损失值。训练结束后，会弹出一个窗口显示四个图表
2. **测试**：
- **加载模型**：确保`./save` 目录下有 `cartpole-dqn_episode_<episode_number>.weights.h5`模型。
- **测试模型**：在命令行中执行以下命令启动训练：
```bash
python test.py
```

## 六、超参数调整
可以在 `DQNAgent` 类的 `__init__` 方法中调整以下超参数来优化训练效果：
- **`Gamma`**：折扣因子，控制未来奖励的重要性。较大的值更注重长期奖励，较小的值更关注即时奖励。
- **`Epsilon`**：探索率，初始值为1.0，随着训练逐渐衰减。较高的探索率会使智能体更多地进行随机探索，较低的探索率则更倾向于利用已学习到的策略。
- **`Epsilon_min`**：探索率的最小值，确保智能体在训练后期仍有一定的探索能力。
- **`Epsilon_decay`**：探索率的衰减率，控制探索率随时间的下降速度。
- **`learning_rate`**：神经网络的学习率，影响模型参数更新的步长。较大的学习率可能导致训练不稳定，较小的学习率可能使训练速度变慢。

## 七、注意事项
- 训练过程可能需要一定的时间，具体时间取决于你的计算机性能。
- 你可以根据需要调整 `EPISODES`、`batch_size` 等参数，以获得更好的训练效果。
- 确保 `./save` 目录有足够的空间来保存模型权重文件。 