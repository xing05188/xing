# 用户交互技术（HCI）期末项目 - 手势控制游戏集合
- 2351289 周慧星

本项目是同济大学软件工程专业用户交互技术（HCI）课程的期末项目。项目包含两部分手势控制游戏：手势版球球大作战和手势版我的世界。通过手势识别技术，玩家可以使用手势来控制游戏中的角色动作。

## 项目结构
```
hci_final/
│
├── Assets/                 # 各类游戏与启动器图片/音频资源
│   ├── app/                # 启动器和按钮图片
│   ├── ball/               # 球球大作战资源
│   └── minecraft/          # 我的世界资源
├── models/
│   └── ResNet152.pth       # 手势识别预训练模型
├── part1/
│   ├── Minecraft_game.py   # 未整合手势控制的我的世界游戏代码
│   ├── ball_game.py        # 未整合手势控制的球球大作战游戏代码
│   ├── gesture_recognition.py # 手势识别相关代码
│   └── README.md           # Part1说明文档
├── part2/
│   ├── app1.py             # 手势版球球大作战
│   ├── app2.py             # 手势版我的世界
│   ├── main.py             # 图形化启动器
│   └── README.md           # part2说明文档
```

## 安装依赖库
```bash
pip install pygame ursina opencv-python mediapipe torch torchvision pillow perlin_noise pyautogui
```


## 快速开始
### 1. part1单独运行

- **球球大作战**  
```bash
python hci_final/part1/ball_game.py
```
- **我的世界**  
```bash
python hci_final/part1/Minecraft_game.py
```
- **手势识别演示**  
```bash
python hci_final/part1/gesture_recognition.py   
```

### 2. part2启动图形化主菜单（含手势操作）
```bash
python hci_final/part2/main.py
```
**选择游戏**：在图形化启动器界面中，点击“球球大作战”或“我的世界”按钮，即可启动相应的游戏。

---

## 功能简介
详细见part1和part2目录下的README.md文件。