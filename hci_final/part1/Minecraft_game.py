from ursina import *
from ursina.prefabs.first_person_controller import FirstPersonController
from perlin_noise import PerlinNoise
import random

app = Ursina()
#加载配置文件
try:
    Arm_Texture = load_texture("Arm_Texture.png", path="hci_final/Assets/minecraft/")
    Brick_Texture = load_texture("Brick_Block.png", path="hci_final/Assets/minecraft/")
    Grass_Texture = load_texture("Grass_Block.png", path="hci_final/Assets/minecraft/")
    Stone_Texture = load_texture("Stone_Block.png", path="hci_final/Assets/minecraft/")
    Dirt_Texture = load_texture("Dirt_Block.png", path="hci_final/Assets/minecraft/")
    Wood_Texture = load_texture("Wood_Block.png", path="hci_final/Assets/minecraft/")
    Sky_Texture = load_texture("Skybox.png", path="hci_final/Assets/minecraft/")
    TNT_Texture = load_texture("TNT.png", path="hci_final/Assets/minecraft/")
    Leaf_Texture = load_texture("leaf.png", path="hci_final/Assets/minecraft/")
except Exception as e:
    print(f"Error loading: {e}")

#选择的方块
block_pick = 1

#更新
def update():
    if held_keys['escape']:
        application.quit()
    global block_pick
    if held_keys["left mouse"] or held_keys["right mouse"]:
        hand.active()
    else:
        hand.passive()
    if held_keys["1"]: block_pick = 1
    if held_keys["2"]: block_pick = 2
    if held_keys["3"]: block_pick = 3
    if held_keys["4"]: block_pick = 4
    if held_keys["5"]: block_pick = 5
    if held_keys["6"]: block_pick = 6
    if held_keys["7"]: block_pick = 7

#方块
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
    
    #放置方块，消除方块
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

#天空
class Sky(Entity):
    def __init__(self):
        super().__init__(
            parent = scene,
            model = "sphere",
            texture = Sky_Texture,
            scale = 150,
            double_sided = True
        )

#手臂
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

#生成树
def generate_tree(position):
    # 树干
    trunk_height = random.randint(4, 6)
    for i in range(trunk_height):
        Voxel(position=(position[0], position[1] + i, position[2]), texture=Wood_Texture)
    # 树叶
    leaf_start = position[1] + trunk_height - 1
    for dx in range(-2, 3):
        for dz in range(-2, 3):
            for dy in range(0, 3):
                if abs(dx) + abs(dz) + dy < 4:  # 控制树叶形状
                    Voxel(position=(position[0] + dx, leaf_start + dy, position[2] + dz), texture=Leaf_Texture)

# 生成房子的函数
def generate_house(position):
    width=7
    depth=7
    height=5
    # 墙壁（带门和窗户）
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
    # 屋顶
    roof_height = int(max(width,depth) / 2) + 2
    for x in range(width + 2):
        for z in range(depth + 2):
            dist_x = max(abs(x - width/2), 0.5)
            dist_z = max(abs(z - depth/2), 0.5)
            roof_y = roof_height - int(dist_x + dist_z)
            for y in range(1,roof_y+1):
                Voxel(position=(position[0] + x - 1, position[1] + height + y, position[2] + z - 1), texture=Wood_Texture)

# 定义地形生成函数
def generate_terrain(size=20, height=3):
    noise = PerlinNoise(octaves=3, seed=random.randint(1, 1000))
    for x in range(-size, size):
        for z in range(-size, size):
            # 使用噪声函数生成地形高度
            noise_value = noise([x / 10, z / 10])
            ground_height = int(noise_value * height)
            for y in range(-height, ground_height + 1):
                if y == ground_height:
                    texture = Grass_Texture
                else:
                    texture = Dirt_Texture
                Voxel(position=(x, y, z), texture=texture)
    generate_house(position=(3, 0, 3))  # 生成房子
    # 随机生成树
    tree_count = int(size * size / 20)  # 根据地形大小调整树的数量
    for _ in range(tree_count):
        x = random.randint(-size + 2, size - 2)
        z = random.randint(-size + 2, size - 2)
        if (1<=x<=12 and 1<=z<=12):
            continue
        noise_value = noise([x / 10, z / 10])
        ground_height = int(noise_value * height)
        generate_tree(position=(x, ground_height, z))


    
generate_terrain()
player = FirstPersonController()
sky = Sky()
hand = Hand()

app.run()