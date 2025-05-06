import os
import zipfile
from datetime import datetime
import clip
import gradio as gr
import numpy as np
import torch
from PIL import Image
from upstash_vector import Index

# 配置环境
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
#收藏列表
favorites=[]
file_download=[]

custom_css = """
/* 整体背景 */
.gradio-container {
    background: linear-gradient(135deg, #f0f8ff 0%, #e6f0ff 100%) !important;
    min-height: 100vh;
}
/* 标题样式 */
#title {
    color: #4a89dc !important;
    text-align: center;
    font-family: 'Microsoft YaHei';
    font-size: 2.2em !important;
    text-shadow: 2px 2px 4px rgba(255,102,153,0.2);
}
#label {
    position: absolute;
    top: 10px;
    right: 10px;
    background-color: #4a89dc;
    color: white;
    padding: 5px 10px;
    border-radius: 5px;
    font-size: 16px;
}
/* 按钮样式 */
#search_btn, #download_btn, #fav_btn {
    background: #4a89dc !important;
    color: white !important;
    border-radius: 50% !important;
    padding: 12px 20px !important;
    transition: all 0.3s !important;
}
#search_btn:hover, #download_btn:hover, #fav_btn:hover {
    transform: scale(1.1);
    box-shadow: 0 4px 12px rgba(255,102,153,0.4) !important;
}
/* 图片展示区域 */
#gallery, #gallery_1 {
    border-radius: var(--border-radius) !important;
    background-color: white !important;
    padding: 2rem !important;
    box-shadow: var(--shadow-sm) !important;
    overflow-y: auto !important;
    max-height: 80vh !important;  # 限制最大高度为视窗的80%
}
"""


# 图像编码
def encode_image(image):
    if isinstance(image, str):
        image = Image.open(image)
    image = preprocess(image).unsqueeze(0).to(DEVICE)
    with torch.no_grad():
        features = model.encode_image(image)
    return features.cpu().numpy().astype(np.float32).tolist()[0]


# 文本编码
def encode_text(text):
    text = clip.tokenize([text]).to(DEVICE)
    with torch.no_grad():
        features = model.encode_text(text)
    return features.cpu().numpy().astype(np.float32).tolist()[0]


# 图像搜索
def search(query_image=None, query_text="", top_k=10, category=None):
    vectors = []
    weights = []
    if query_image is not None:
        image_vec = encode_image(query_image)
        vectors.append(image_vec)
        weights.append(0.7)  # 图像权重
    if query_text.strip():
        text_vec = encode_text(query_text)
        vectors.append(text_vec)
        weights.append(0.3)  # 文本权重
    if not vectors:
        return []
    avg_vector = np.average(vectors, axis=0, weights=weights).tolist()
    # 构建过滤条件（自动处理"全部"类别）
    metadata_filter = f"category='{category}'" if category and category != "All" else None
    # 执行搜索
    results = index.query(
        vector=avg_vector,
        top_k=top_k,
        include_metadata=True,
        filter=metadata_filter
    )
    # 准备返回结果
    images = [match.metadata["image_path"] for match in results]
    return images

#收藏
def favorite_save(image_paths):
    favorites.extend(image_paths)
    return favorites

# 下载图片
def zip_files(image_paths):
    temp_dir = "download_image"
    os.makedirs(temp_dir, exist_ok=True)
    zip_name = f"download_{datetime.now().strftime('%Y%m%d%H%M%S')}.zip"
    zip_path = os.path.join(temp_dir, zip_name)
    with zipfile.ZipFile(zip_path, 'w') as zipf:
        for item in image_paths:
            try:
                img_path = item[0] if isinstance(item, tuple) else item
                if not os.path.exists(img_path):
                    print(f"文件不存在: {img_path}")
                    continue
                zipf.write(img_path, os.path.basename(img_path))
            except Exception as e:
                print(f"添加文件到ZIP失败: {item} → {str(e)}")
    if zipf.namelist():
        file_download.append(zip_path)
        print(file_download)
        return file_download
    else:
        os.remove(zip_path)
        raise gr.Error("没有有效的文件可下载")


# 使用 Gradio 的 Blocks 接口创建一个交互式界面
with gr.Blocks(css=custom_css) as demo:
    gr.Markdown("""
               <div id="title">🖼️ 图像搜索--支持同时输入图像和文本联合搜索 🎨</div>
               <div id="label">2351289周慧星</div>
               """)
    with gr.Row():
        with gr.Column(scale=1):
            # 输入面板
            image_input = gr.Image(label="📁 上传查询图像", type="filepath", elem_id="image_put",height=267)
            text_input = gr.Textbox(label="🔤 搜索文本--请输入英文", placeholder="输入描述...", elem_id="text_input", )
            with gr.Accordion("⚙️ 高级选项", open=True):
                top_k = gr.Slider(1, 50, step=1, value=10, label="返回结果数量", elem_id="top_k")
                category = gr.Dropdown(
                    label="🏷️ 筛选类别",
                    choices=["All", "Fruit", "Packages", "Vegetables"],  # 根据数据集调整
                    value="All",
                    elem_id="category"
                )
            search_btn = gr.Button("🔍 开始搜索", variant="primary", elem_id="search_btn")
        with gr.Column(scale=2):
            # 结果展示
            gallery = gr.Gallery(
                label="🔍 搜索结果，点击可放大图片，按ESC键退出放大",
                columns=5,
                object_fit="cover",
                height=600,
                interactive=False,  # 完全禁用交互
                elem_id="gallery"
            )
            # 操作按钮
            with gr.Row():
                download_btn = gr.Button("📤 下载图片", elem_id="download_btn")
                fav_btn = gr.Button("⭐ 加入收藏夹", elem_id="fav_btn")
    with gr.Row():
        with gr.Column(scale=2):
            # 收藏展示
            gallery_1 = gr.Gallery(
                label="⭐ 收藏结果，点击可放大图片，按ESC键退出放大",
                columns=5,
                object_fit="cover",
                height=300,
                interactive=False,  # 完全禁用交互
                elem_id="gallery_1"
            )
        with gr.Column(scale=1):
            file_out = gr.Files(label="📤 下载结果",elem_id="file_out")

    # 搜索按钮
    search_btn.click(
        fn=search,
        inputs=[image_input, text_input, top_k, category],
        outputs=[gallery]  # 修改为组件列表
    )
    # 下载按钮绑定
    download_btn.click(
        fn=zip_files,
        inputs=gallery,
        outputs=file_out
    )
    # 收藏功能
    fav_btn.click(
        fn=favorite_save,
        inputs=gallery,
        outputs=gallery_1
    )

demo.launch(share=False)
