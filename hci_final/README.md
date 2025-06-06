# Final Project of Human-Computer Interaction (HCI) - Gesture-Controlled Game Collection
- 2351289 Zhou Huixing

This project is the final project for the Human-Computer Interaction (HCI) course in the Software Engineering major at Tongji University. The project consists of two gesture-controlled games: a gesture-based version of "Ball Battle" and a gesture-based version of "Minecraft". Through gesture recognition technology, players can use gestures to control the actions of characters in the games.

## Project Structure
```
hci_final/
│
├── Assets/                 # Various game and launcher image/audio resources
│   ├── app/                # Launcher and button images
│   ├── ball/               # Resources for Ball Battle
│   └── minecraft/          # Resources for Minecraft
├── models/
│   └── ResNet152.pth       # Pretrained model for gesture recognition
├── part1/
│   ├── Minecraft_game.py   # Code for the non-integrated gesture-controlled Minecraft game
│   ├── ball_game.py        # Code for the non-integrated gesture-controlled Ball Battle game
│   ├── gesture_recognition.py # Code related to gesture recognition
│   └── README.md           # Instruction document for Part1
├── part2/
│   ├── app1.py             # Gesture-based Ball Battle
│   ├── app2.py             # Gesture-based Minecraft
│   ├── main.py             # Graphical launcher
│   └── README.md           # Instruction document for part2
```

## Install Dependencies
```bash
pip install pygame ursina opencv-python mediapipe torch torchvision pillow perlin_noise pyautogui
```

## Quick Start
### 1. Run Part1 Individually

- **Ball Battle**
```bash
python hci_final/part1/ball_game.py
```
- **Minecraft**
```bash
python hci_final/part1/Minecraft_game.py
```
- **Gesture Recognition Demonstration**
```bash
python hci_final/part1/gesture_recognition.py   
```

### 2. Launch the Graphical Main Menu in Part2 (with Gesture Operations)
```bash
python hci_final/part2/main.py
```
**Select a Game**: On the graphical launcher interface, click the "Ball Battle" or "Minecraft" button to start the corresponding game.

---

## Function Introduction
For detailed information, refer to the README.md files in the part1 and part2 directories.