from ursina import *
from ursina.prefabs.first_person_controller import FirstPersonController
import cv2
import torch
import torchvision.transforms as transforms
import torchvision.models as models
from PIL import Image
import threading
import time
from perlin_noise import PerlinNoise
import random

# ========== 手势识别相关 ==========
# 加载 ResNet152 模型
model = models.resnet152()
model.fc = torch.nn.Linear(model.fc.in_features, 34)
checkpoint = torch.load("../models/ResNet152.pth", map_location=torch.device('cpu'))
if "MODEL_STATE" in checkpoint:
    model_state = checkpoint["MODEL_STATE"]
else:
    model_state = checkpoint
model.load_state_dict(model_state)
model.eval()

gesture_classes = [
    "grabbing",        # 0
    "grip",            # 1
    "holy",            # 2
    "point",           # 3
    "call",            # 4
    "three3",          # 5
    "timeout",         # 6
    "xsign",           # 7
    "hand_heart",      # 8
    "hand_heart2",     # 9
    "little_finger",   # 10
    "middle_finger",   # 11
    "take_picture",    # 12
    "dislike",         # 13
    "fist",            # 14
    "four",            # 15
    "like",            # 16
    "mute",            # 17
    "ok",              # 18
    "one",             # 19
    "palm",            # 20
    "peace",           # 21
    "peace_inverted",  # 22
    "rock",            # 23
    "stop",            # 24
    "stop_inverted",   # 25
    "three",           # 26
    "three2",          # 27
    "two_up",          # 28
    "two_up_inverted", # 29
    "three_gun",       # 30
    "thumb_index",     # 31
    "thumb_index2",    # 32
    "no_gesture"       # 33
]
#可自行修改手势类别，需与模型训练时保持一致
gesture_to_key = {
    19: "w",
    21: "s",
    20: "a",
    15: "d",
    18: "space",
    1: "1",
    2: "2",
    3: "3",
    4: "4",
    5: "5",
    6: "6",
    7: "7",
}

# 虚拟键
virtual_keys = {
    "w": False, "s": False, "a": False, "d": False, "space": False,
    "1": False, "2": False, "3": False, "4": False, "5": False, "6": False, "7": False
}

transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.54, 0.499, 0.474], std=[0.234, 0.235, 0.231])
])

def predict(image):
    image = Image.fromarray(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
    input_tensor = transform(image).unsqueeze(0)
    with torch.no_grad():
        outputs = model(input_tensor)
    predicted_class = torch.argmax(outputs, dim=1).item()
    return predicted_class

def gesture_control_thread():
    cap = cv2.VideoCapture(0)
    prev_idx = None
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        frame = cv2.flip(frame, 1)
        predicted_class = predict(frame)

        # 先全部松开
        for k in virtual_keys:
            virtual_keys[k] = False

        # 如果是控制范围内的手势
        if predicted_class in gesture_to_key:
            key = gesture_to_key[predicted_class]
            virtual_keys[key] = True

        # 显示手势识别结果
        cv2.putText(frame, gesture_classes[predicted_class], (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        cv2.imshow("Hand_Gesture", frame)
        if cv2.waitKey(1) & 0xFF == 27:
            break
        time.sleep(0.1)  # 减少 CPU 占用

    cap.release()
    cv2.destroyAllWindows()

# ========== Minecraft 游戏相关 ==========
window.borderless = False
window.size = (960*2, 640*2)
app = Ursina()
try:
    Arm_Texture = load_texture("Arm_Texture.png", path="../Assets/minecraft/")
    Brick_Texture = load_texture("Brick_Block.png", path="../Assets/minecraft/")
    Grass_Texture = load_texture("Grass_Block.png", path="../Assets/minecraft/")
    Stone_Texture = load_texture("Stone_Block.png", path="../Assets/minecraft/")
    Dirt_Texture = load_texture("Dirt_Block.png", path="../Assets/minecraft/")
    Wood_Texture = load_texture("Wood_Block.png", path="../Assets/minecraft/")
    Sky_Texture = load_texture("Skybox.png", path="../Assets/minecraft/")
    TNT_Texture = load_texture("TNT.png", path="../Assets/minecraft/")
    Leaf_Texture = load_texture("leaf.png", path="../Assets/minecraft/")
except Exception as e:
    print(f"Error loading: {e}")

block_pick = 1

def update():
    global block_pick
    # 支持物理键盘和虚拟键
    if held_keys['escape']:
        application.quit()
    if held_keys["left mouse"] or held_keys["right mouse"]:
        hand.active()
    else:
        hand.passive()

    # 方块选择
    for i in range(1, 8):
        if held_keys[str(i)] or virtual_keys[str(i)]:
            block_pick = i

    # 玩家移动和跳跃
    if held_keys["w"] or virtual_keys["w"]:
        player.position += player.forward * 5 * time.dt
    if held_keys["s"] or virtual_keys["s"]:
        player.position -= player.forward * 5 * time.dt
    if held_keys["a"] or virtual_keys["a"]:
        player.position -= player.right * 5 * time.dt
    if held_keys["d"] or virtual_keys["d"]:
        player.position += player.right * 5 * time.dt
    if (held_keys["space"] or virtual_keys["space"]) and player.grounded:
        player.jump()

class Voxel(Button):
    def __init__(self, position = (0, 0, 0), texture = Grass_Texture):
        super().__init__(
            parent = scene,
            position = position,
            model = "cube", 
            origin_y = 0.5,
            texture = texture,
            color = color.color(0, 0, random.uniform(0.9, 1)),
            highlight_color = color.light_gray,
        )
    def input(self,key):
        if self.hovered:
            if key == "left mouse down":
                if block_pick == 1: Voxel(position = self.position + mouse.normal, texture = Grass_Texture)
                if block_pick == 2: Voxel(position = self.position + mouse.normal, texture = Stone_Texture)
                if block_pick == 3: Voxel(position = self.position + mouse.normal, texture = Brick_Texture)
                if block_pick == 4: Voxel(position = self.position + mouse.normal, texture = Dirt_Texture)
                if block_pick == 5: Voxel(position = self.position + mouse.normal, texture = Wood_Texture)
                if block_pick == 6: Voxel(position = self.position + mouse.normal, texture = TNT_Texture)
                if block_pick == 7: Voxel(position = self.position + mouse.normal, texture = Leaf_Texture)
            if key == "right mouse down":
                destroy(self)

class Sky(Entity):
    def __init__(self):
        super().__init__(
            parent = scene,
            model = "sphere",
            texture = Sky_Texture,
            scale = 150,
            double_sided = True
        )

class Hand(Entity):
    def __init__(self):
        super().__init__(
            parent = camera.ui,
            model = "cube",
            texture = Arm_Texture,
            scale = 0.2,
            rotation = Vec3(150, -10, 0),
            position = Vec2(0.4, -0.6)
        )
    def active(self):
        self.position = Vec2(0.3, -0.5)
    def passive(self):
        self.position = Vec2(0.4, -0.6)

def generate_tree(position):
    trunk_height = random.randint(4, 6)
    for i in range(trunk_height):
        Voxel(position=(position[0], position[1] + i, position[2]), texture=Wood_Texture)
    leaf_start = position[1] + trunk_height - 1
    for dx in range(-2, 3):
        for dz in range(-2, 3):
            for dy in range(0, 3):
                if abs(dx) + abs(dz) + dy < 4:
                    Voxel(position=(position[0] + dx, leaf_start + dy, position[2] + dz), texture=Leaf_Texture)

def generate_house(position):
    width=7
    depth=7
    height=5
    for y in range(1, height + 1):
        for x in range(width):
            for z in range(depth):
                wx, wy, wz = position[0] + x, position[1] + y, position[2] + z
                if (z == 0 and x == width // 2 and (y == 1 or y == 2)):
                    continue
                if ((x == 0 or x == width - 1) and y == 3 and z in [depth // 3, depth - 1 - depth // 3]):
                    continue
                if x == 0 or x == width - 1 or z == 0 or z == depth - 1:
                    Voxel(position=(wx, wy, wz), texture=Brick_Texture)
    roof_height = int(max(width,depth) / 2) + 2
    for x in range(width + 2):
        for z in range(depth + 2):
            dist_x = max(abs(x - width/2), 0.5)
            dist_z = max(abs(z - depth/2), 0.5)
            roof_y = roof_height - int(dist_x + dist_z)
            for y in range(1,roof_y+1):
                Voxel(position=(position[0] + x - 1, position[1] + height + y, position[2] + z - 1), texture=Wood_Texture)

def generate_terrain(size=20, height=3):
    noise = PerlinNoise(octaves=3, seed=random.randint(1, 1000))
    for x in range(-size, size):
        for z in range(-size, size):
            noise_value = noise([x / 10, z / 10])
            ground_height = int(noise_value * height)
            for y in range(-height, ground_height + 1):
                if y == ground_height:
                    texture = Grass_Texture
                else:
                    texture = Dirt_Texture
                Voxel(position=(x, y, z), texture=texture)
    generate_house(position=(3, 0, 3))
    tree_count = int(size * size / 20)
    for _ in range(tree_count):
        x = random.randint(-size + 2, size - 2)
        z = random.randint(-size + 2, size - 2)
        if (1<=x<=12 and 1<=z<=12):
            continue
        noise_value = noise([x / 10, z / 10])
        ground_height = int(noise_value * height)
        generate_tree(position=(x, ground_height, z))

if __name__ == "__main__":
    t = threading.Thread(target=gesture_control_thread)
    t.daemon = True
    t.start()
    generate_terrain()
    player = FirstPersonController()
    sky = Sky()
    hand = Hand()
    app.run()