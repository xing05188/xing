import pygame
import random
import math
import sys
import cv2
import mediapipe as mp
import pyautogui
import threading
import time

WIDTH, HEIGHT = 2000, 1100  # 地图尺寸
VIEW_W, VIEW_H = 1200, 800  # 屏幕窗口
FOOD_NUM = 320
SPIKE_NUM = 8
AI_NUM = 5
PLAYER_INIT_RADIUS = 32
FOOD_RADIUS = 7
SPIKE_RADIUS = 40
SPLIT_SPEED = 16
EJECT_SPEED = 20
gesture_q = False
gesture_e = False

def clamp(val, minv, maxv):
    return max(minv, min(val, maxv))

def dist(a, b):
    return math.hypot(a[0]-b[0], a[1]-b[1])

class Ball:
    def __init__(self, x, y, r, img=None, master=None):
        self.x = x
        self.y = y
        self.r = r
        self.img = img
        self.dx = 0
        self.dy = 0
        self.split_timer = 0  # 分裂冷却
        self.eject_timer = 0  # 吐球冷却
        self.master = master

    @property
    def mass(self):
        return self.r ** 2

    def draw(self, surf, cx, cy, scale=1.0):
        px = int((self.x - cx) * scale + VIEW_W // 2)
        py = int((self.y - cy) * scale + VIEW_H // 2)
        pr = int(self.r * scale)
        img = pygame.transform.scale(self.img, (pr * 2, pr * 2))
        surf.blit(img, (px - pr, py - pr))

    def move(self, tx, ty, speed_factor=1.0):
        dx, dy = tx - self.x, ty - self.y
        d = math.hypot(dx, dy)
        if d > 1e-2:
            dx, dy = dx / d, dy / d
            speed = clamp(50 / math.log(self.mass + 10), 2.5, 6) * speed_factor
            self.x += dx * speed
            self.y += dy * speed
            # 边界限制
            self.x = clamp(self.x, self.r, WIDTH - self.r)
            self.y = clamp(self.y, self.r, HEIGHT - self.r)

def random_color():
    r = random.randint(50, 255)
    g = random.randint(50, 255)
    b = random.randint(50, 255)
    return (r, g, b)

class Food:
    def __init__(self, x, y, r=FOOD_RADIUS):
        self.x = x
        self.y = y
        self.r = r
        self.color = random_color()

    def draw(self, surf, cx, cy, scale=1.0):
        px = int((self.x - cx) * scale + VIEW_W // 2)
        py = int((self.y - cy) * scale + VIEW_H // 2)
        pr = int(self.r * scale)
        pygame.draw.circle(surf, self.color, (px, py), pr)

class Spike:
    def __init__(self, x, y, r=SPIKE_RADIUS, img=None):
        self.x = x
        self.y = y
        self.r = r
        self.img = img

    def draw(self, surf, cx, cy, scale=1.0):
        px = int((self.x - cx) * scale + VIEW_W // 2)
        py = int((self.y - cy) * scale + VIEW_H // 2)
        pr = int(self.r * scale)
        img = pygame.transform.scale(self.img, (pr * 2, pr * 2))
        surf.blit(img, (px - pr, py - pr))

# --- 游戏初始化 ---
def random_pos(r):
    return random.randint(r, WIDTH - r), random.randint(r, HEIGHT - r)

# --- 手势识别线程 ---
def detect_gestures_thread():
    global gesture_q, gesture_e
    mp_hands = mp.solutions.hands
    hands = mp_hands.Hands(min_detection_confidence=0.8, min_tracking_confidence=0.8)
    mp_draw = mp.solutions.drawing_utils
    # 获取屏幕分辨率
    screen_width, screen_height = pyautogui.size()
    # 打开摄像头
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    last_x, last_y = 0, 0
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        # 翻转图像（镜像效果）
        frame = cv2.flip(frame, 1)
        # 转换为 RGB 图像
        rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        # 检测手部
        result = hands.process(rgb_frame)
        if result.multi_hand_landmarks:
            for hand_landmarks, handedness in zip(result.multi_hand_landmarks, result.multi_handedness):
                # 获取手的方向（"Left" 或 "Right"）
                hand_label = handedness.classification[0].label
                # 绘制手部关键点
                mp_draw.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

                if hand_label == "Right":
                    # 右手逻辑
                    cv2.putText(frame, "Right Hand", (20, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
                    # 鼠标移动部分
                    lm = hand_landmarks.landmark
                    screen_x = int(lm[0].x * screen_width)
                    screen_y = int(lm[0].y * screen_height)
                    move_threshold = 10
                    if abs(screen_x - last_x) > move_threshold or abs(screen_y - last_y) > move_threshold:
                        pyautogui.moveTo(screen_x, screen_y)
                        last_x, last_y = screen_x, screen_y
                    #1手势：仅食指伸直，其他手指弯曲
                    one_gesture = (
                        lm[8].y < lm[7].y and lm[7].y<lm[6].y and lm[12].y > lm[10].y and
                        lm[16].y > lm[14].y and lm[20].y > lm[18].y
                    )
                    # 2手势：食指与中指伸直，其他弯曲
                    two_gesture = (
                        lm[8].y < lm[7].y and  lm[7].y<lm[6].y and lm[12].y < lm[11].y and 
                        lm[11].y<lm[10].y and  lm[16].y > lm[14].y and lm[20].y > lm[18].y  
                    )
                    if one_gesture:
                        cv2.putText(frame, "1", (20, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
                        gesture_q = True
                    else:
                        gesture_q = False
                    if two_gesture:
                        cv2.putText(frame, "2", (20, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
                        gesture_e = True
                    else:
                        gesture_e = False

        # 显示摄像头画面
        cv2.imshow("Hand Tracking", frame)
        if cv2.waitKey(1) & 0xFF == 27:
            break
        time.sleep(0.01)

    # 释放资源
    cap.release()
    cv2.destroyAllWindows()

def run_game_with_gesture():
    # 启动手势识别线程
    gesture_thread = threading.Thread(target=detect_gestures_thread, daemon=True)
    gesture_thread.start()

    pygame.init()
    screen = pygame.display.set_mode((VIEW_W, VIEW_H))
    pygame.display.set_caption("球球大作战")
    clock = pygame.time.Clock()
    pygame.mixer.init()
    pygame.mixer.music.load("../Assets/ball/music.mp3")
    pygame.mixer.music.play(-1)
    # 加载图片
    BALL_IMG = pygame.image.load("../Assets/ball/ball.png").convert_alpha()
    Ball_img1 = pygame.image.load("../Assets/ball/ball1.png").convert_alpha()
    Ball_img2 = pygame.image.load("../Assets/ball/ball2.png").convert_alpha()
    Ball_img3 = pygame.image.load("../Assets/ball/ball3.png").convert_alpha()
    Ball_img4 = pygame.image.load("../Assets/ball/ball4.png").convert_alpha()
    Ball_img5 = pygame.image.load("../Assets/ball/ball5.png").convert_alpha()
    eject_img = pygame.image.load("../Assets/ball/eject.png").convert_alpha()
    Spike_IMG = pygame.image.load("../Assets/ball/spike.png").convert_alpha()
    player = Ball(WIDTH // 2, HEIGHT // 2, PLAYER_INIT_RADIUS, img=BALL_IMG)
    balls = [player]
    ai_balls = []
    ai_imgs = [Ball_img1, Ball_img2, Ball_img3, Ball_img4, Ball_img5]
    for i in range(AI_NUM):
        x, y = random_pos(PLAYER_INIT_RADIUS)
        ai = Ball(x, y, PLAYER_INIT_RADIUS, img=ai_imgs[i % len(ai_imgs)])
        ai.ai_target = random_pos(PLAYER_INIT_RADIUS)
        ai.ai_timer = random.randint(30, 120)  # 随机换目标的帧数
        ai_balls.append(ai)
        balls.append(ai)

    foods = [Food(*random_pos(FOOD_RADIUS)) for _ in range(FOOD_NUM)]
    spikes = [Spike(*random_pos(SPIKE_RADIUS), img=Spike_IMG) for _ in range(SPIKE_NUM)]
    ejects = []  # 吐球
    # 添加一个状态变量，防抖（只在手势从False变为True时触发一次）
    gesture_e_last = False
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit(); sys.exit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    pygame.quit(); sys.exit()
                if  event.key ==pygame.K_e and player.r > 24:
                    angle = math.atan2(pygame.mouse.get_pos()[1] - VIEW_H // 2, pygame.mouse.get_pos()[0] - VIEW_W // 2)
                    new_r = player.r / math.sqrt(2)
                    player.r = new_r
                    bx = player.x + math.cos(angle) * player.r * 2
                    by = player.y + math.sin(angle) * player.r * 2
                    split_ball = Ball(bx, by, new_r,img=BALL_IMG,master=player)
                    split_ball.dx = math.cos(angle) * SPLIT_SPEED
                    split_ball.dy = math.sin(angle) * SPLIT_SPEED
                    balls.append(split_ball)


        if gesture_e and not gesture_e_last and player.r > 24:
            angle = math.atan2(pygame.mouse.get_pos()[1] - VIEW_H // 2, pygame.mouse.get_pos()[0] - VIEW_W // 2)
            new_r = player.r / math.sqrt(2)
            player.r = new_r
            bx = player.x + math.cos(angle) * player.r * 2
            by = player.y + math.sin(angle) * player.r * 2
            split_ball = Ball(bx, by, new_r, img=BALL_IMG, master=player)
            split_ball.dx = math.cos(angle) * SPLIT_SPEED
            split_ball.dy = math.sin(angle) * SPLIT_SPEED
            balls.append(split_ball)
        gesture_e_last = gesture_e

        # 吐球冷却（单位：帧）
        if not hasattr(player, "eject_cd"):
            player.eject_cd = 0
        keys = pygame.key.get_pressed()
        if (keys[pygame.K_q] or gesture_q) and player.r > 18 and player.eject_cd == 0:
            angle = math.atan2(pygame.mouse.get_pos()[1] - VIEW_H // 2, pygame.mouse.get_pos()[0] - VIEW_W // 2)
            ex = player.x + math.cos(angle) * player.r
            ey = player.y + math.sin(angle) * player.r
            eject = Ball(ex, ey, FOOD_RADIUS + 6, img=eject_img)
            eject.dx = math.cos(angle) * EJECT_SPEED
            eject.dy = math.sin(angle) * EJECT_SPEED
            ejects.append(eject)
            player.r = math.sqrt(max(player.mass - (FOOD_RADIUS + 6) ** 2, 1))
            player.eject_cd = 8  # 冷却8帧

        if player.eject_cd > 0:
            player.eject_cd -= 1

        # 玩家移动,玩家和所有分裂球都能移动
        mx, my = pygame.mouse.get_pos()
        tx = player.x + (mx - VIEW_W // 2)
        ty = player.y + (my - VIEW_H // 2)
        for b in balls:
            if b is player:
                b.move(tx, ty)
            elif b in ai_balls:
                b.ai_timer -= 1
                if b.ai_timer <= 0 or dist((b.x, b.y), b.ai_target) < b.r:
                    b.ai_target = random_pos(PLAYER_INIT_RADIUS)
                    b.ai_timer = random.randint(60, 180)
                b.move(b.ai_target[0], b.ai_target[1])
            elif b.master is not None:
                b.x += b.dx
                b.y += b.dy
                b.dx *= 0.92
                b.dy *= 0.92
                if abs(b.dx) < 0.5 and abs(b.dy) < 0.5:
                    b.move(b.master.x, b.master.y, speed_factor=0.5)
                b.x = clamp(b.x, b.r, WIDTH - b.r)
                b.y = clamp(b.y, b.r, HEIGHT - b.r)
            else:
                pass

        for e in ejects:
            e.x += e.dx
            e.y += e.dy
            e.dx *= 0.92
            e.dy *= 0.92
            e.x = clamp(e.x, e.r, WIDTH - e.r)
            e.y = clamp(e.y, e.r, HEIGHT - e.r)

        # 吃食物
        for b in balls:
            for f in foods[:]:
                if dist((b.x, b.y), (f.x, f.y)) < b.r + f.r:
                    b.r = math.sqrt(b.mass + 25)
                    foods.remove(f)
        # 吃吐球
        for b in balls:
            for e in ejects[:]:
                if dist((b.x, b.y), (e.x, e.y)) < b.r + e.r:
                    b.r = math.sqrt(b.mass + e.r ** 2)
                    ejects.remove(e)

        # 吃球（吞噬）
        for b in balls[:]:
            for other in balls[:]:
                if (b is other) or (not other.master and b.master == other):
                    continue
                if (not b.master and other.master == b) or (b.r > other.r * 1.1):
                    if dist((b.x, b.y), (other.x, other.y)) < b.r:
                        b.r = math.sqrt(b.mass + other.mass)
                        balls.remove(other)
                        break

        # 刺球交互
        for b in balls[:]:
            for s in spikes[:]:
                if dist((b.x, b.y), (s.x, s.y)) < b.r + s.r and b.r > s.r * 1.05:
                    # 记录主球原位置
                    center_x, center_y = b.x, b.y
                    new_r = (b.r + s.r) / 2
                    for i in range(4):
                        angle = i * math.pi / 2
                        bx = center_x + math.cos(angle) * new_r * 2
                        by = center_y + math.sin(angle) * new_r * 2
                        split_ball = Ball(bx, by, new_r / math.sqrt(2), img=b.img, master=b)
                        split_ball.dx = math.cos(angle) * SPLIT_SPEED
                        split_ball.dy = math.sin(angle) * SPLIT_SPEED
                        balls.append(split_ball)
                    b.r = new_r  # 主球半径变小，留在原地
                    spikes.remove(s)  # 刺球消失
                    break

        # 食物数量维持
        while len(foods) < FOOD_NUM:
            foods.append(Food(*random_pos(FOOD_RADIUS)))
        # 刺球数量维持
        while len(spikes) < SPIKE_NUM / 2:
            spikes.append(Spike(*random_pos(SPIKE_RADIUS), img=Spike_IMG))
        # 视野跟随与缩放
        cam_x, cam_y = player.x, player.y
        scale = clamp(1.0 - (player.r - PLAYER_INIT_RADIUS) / 300, 0.5, 1.0)

        # 绘制
        screen.fill((68, 68, 68))  # 灰色
        for f in foods:
            f.draw(screen, cam_x, cam_y, scale)
        for s in spikes:
            s.draw(screen, cam_x, cam_y, scale)
        for e in ejects:
            e.draw(screen, cam_x, cam_y, scale)
        for b in balls:
            b.draw(screen, cam_x, cam_y, scale)
        # 边界
        bx = int((0 - cam_x) * scale + VIEW_W // 2)
        by = int((0 - cam_y) * scale + VIEW_H // 2)
        bw = int(WIDTH * scale)
        bh = int(HEIGHT * scale)
        pygame.draw.rect(screen, (180, 180, 200), (bx, by, bw, bh), 6)
        # UI
        font = pygame.font.SysFont(None, 32)
        txt = font.render(f"Mass:{int(player.mass)} [E]Split [Q]Eject", True, (255, 0, 0))
        screen.blit(txt, (16, 16))
        pygame.display.flip()
        clock.tick(60)

if __name__ == "__main__":
    run_game_with_gesture()