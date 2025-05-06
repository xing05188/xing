import os
from PIL import Image
import clip
import torch
import numpy as np
from tqdm import tqdm
from upstash_vector import Index

#配置环境
UPSTASH_URL = os.getenv("UPSTASH_URL")
UPSTASH_TOKEN = os.getenv("UPSTASH_TOKEN")
if not UPSTASH_URL or not UPSTASH_TOKEN:
    raise ValueError("UPSTASH_URL 或 UPSTASH_TOKEN 未设置，请检查环境变量。")
DEVICE = "cuda" if torch.cuda.is_available() else "cpu"
model, preprocess = clip.load("ViT-B/32", device=DEVICE)
# 连接Upstash Vector
index = Index(url=UPSTASH_URL, token=UPSTASH_TOKEN)
try:
    info = index.info()
    print("成功连接到 Upstash 数据库:", info)
except Exception as e:
    print("连接失败:", e)

# 遍历数据集
dataset_path = "dataset/"
if not os.path.exists(dataset_path):
    raise FileNotFoundError(f"数据集路径 {dataset_path} 不存在，请检查路径。")
for root, dirs, files in os.walk(dataset_path):
    for file in tqdm(files):
        if file.lower().endswith(('.png', '.jpg', '.jpeg')):
            img_path = os.path.join(root, file)
            try:
                relative_path = os.path.relpath(root, dataset_path)
                first_dir = relative_path.split(os.sep)[0]
                image = preprocess(Image.open(img_path)).unsqueeze(0).to(DEVICE)
                with torch.no_grad():
                    features = model.encode_image(image)
                vector = features.cpu().numpy().astype(np.float32).tolist()[0]
                index.upsert(
                    vectors=[{
                        "id": file.split('.')[0],
                        "vector": vector,
                        "metadata": {
                            "category": first_dir,
                            "image_path": img_path
                        }
                    }]
                )
            except Exception as e:
                print(f"预处理图像失败: {img_path}: {str(e)}")