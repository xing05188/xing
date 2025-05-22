# 大语言模型部署流程
- 2351289周慧星
- REPORT报告见文件
- **项目链接**：[魔搭社区项目](https://modelscope.cn/models/xing0518/AI4_LLM/summary)/[GitHub 代码仓库](https://github.com/xing05188/xing/tree/main/AI4_LLM)  

### **一、平台搭建**
#### **1. 注册&登录 ModelScope**
- **操作步骤**：
  1. 访问官网 [ModelScope](https://www.modelscope.cn/home)。
  2. 点击右上角“注册”，完成新用户注册（需绑定手机号）。
  3. 注册后登录，进入个人主页。

#### **2. 获取计算资源**
- **前提条件**：
  - 需绑定阿里云账号（在 ModelScope 个人主页 > 账号设置中操作）。
  - 确保已获得免费云计算资源（通常新用户有默认配额）。
- **启动 CPU 服务器**：
  1. 在 ModelScope 首页，进入“创空间”或“我的 Notebooks”。
  2. 选择“启动实例”，配置 CPU 环境（如 Ubuntu 系统），等待实例启动完成。

#### **3. 启动 Notebook 并配置环境**
- **打开终端**：
  - 在实例页面点击“Terminal”图标，打开命令行界面。
- **环境配置**：
- 
##### **直接在 root 环境操作**
- **基础依赖安装**：
  1. **检查网络连通性**：
```bash
    pip install -U pip setuptools wheel  # 更新 pip 工具
```
  2. **安装核心库**（兼容 Transformers 4.33.3 和推理需求）：
```bash
     pip install \
     "intel-extension-for-transformers==1.4.2" \
     "neural-compressor==2.5" \
     "transformers==4.33.3" \
     "modelscope==1.9.5" \
     "pydantic==1.10.13" \
     "sentencepiece" \
     "tiktoken" \
     "einops" \
     "transformers_stream_generator" \
     "uvicorn" \
     "fastapi" \
     "yacs" \
     "setuptools_scm"
```
  3. **安装对话工具 fschat**（需启用 PEP517 构建）：
```bash
     pip install fschat --use-pep517
```
- **可选增强工具**（非必需，提升体验）：
```bash
  pip install tqdm huggingface-hub  # 进度条和模型仓库工具
```


### **二、大模型实践：以 Qwen-7B-Chat 为例**
#### **1. 下载大模型到本地**
- **切换到数据目录**（避免占用系统盘空间）：
```bash
  cd /mnt/data  # ModelScope 实例默认数据存储路径
```
- **下载模型**（一次仅下载一个模型，避免存储不足）：
```bash
git clone https://www.modelscope.cn/qwen/Qwen-7B-Chat.git                 # 下载 Qwen-7B-Chat
# 其他模型示例：
# git clone https://www.modelscope.cn/ZhipuAI/chatglm3-6b.git             # ChatGLM3-6B
# git clone https://www.modelscope.cn/baichuan-inc/Baichuan2-7B-Chat.git  #百川
```
- **模型文件结构**（下载后检查）：
```
Qwen-7B-Chat/
├── config.json          # 模型配置
├── pytorch_model-*.bin  # 权重文件
├── tokenizer.model      # 分词器
└── ...                  # 其他元数据文件
```

#### **2. 编写推理脚本**
- **切换工作目录**：
```bash
  cd /mnt/workspace  # 建议在独立目录编写代码
```
- **创建脚本 `run_qwen_cpu.py`**：
```python
from transformers import TextStreamer, AutoTokenizer, AutoModelForCausalLM
import torch
model_name = "/mnt/data/Qwen-7B-Chat"
prompt = [
    "请说出以下两句话区别在哪里? 1､冬天:能穿多少穿多少 2､夏天:能穿多少穿多少",
    "请说出以下两句话区别在哪里？单身狗产生的原因有两个，一是谁都看不上，二是谁都看不上",
    "他知道我知道你知道他不知道吗？ 这句话里，到底谁不知道",
    "明明明明明白白白喜欢他，可她就是不说。 这句话里，明明和白白谁喜欢谁？",
    "领导：你这是什么意思？ 小明：没什么意思。意思意思。 领导：你这就不够意思了。 小明：小意思，小意思。领导：你这人真有意思。 小明：其实也没有别的意思。领导：那我就不好意思了。 小明：是我不好意思。请问：以上“意思”分别是什么意思。",
    "“下雨天留客天留我不留”，请用不同断句方式解释这句话的含义，并说明哪种断句更符合中文表达习惯。",
    "甲说：“乙在说谎。”乙说：“丙在说谎。”丙说：“甲和乙都在说谎。”请问：谁在说谎？谁在说真话？"
]
tokenizer = AutoTokenizer.from_pretrained(
    model_name,
    trust_remote_code=True  # 允许加载远程代码（部分模型需此参数）
)
model = AutoModelForCausalLM.from_pretrained(
    model_name,
    trust_remote_code=True,
    torch_dtype=torch.float32
).eval()  # 切换为推理模式
for i in range(7):
    print(f"\n===== 问题 {i+1} =====\n")
    inputs = tokenizer(prompt[i], return_tensors="pt").input_ids
    streamer = TextStreamer(tokenizer)
    outputs = model.generate(inputs, streamer=streamer, max_new_tokens=300) 
```

#### **3. 运行推理脚本**
- **执行命令**：
```bash
  python run_qwen_cpu.py  # 直接运行脚本
```
- **预期输出**：
  - 模型会逐字输出回答7个问题。  

### **三、注意事项与常见问题**
1. **资源释放问题**：
   - ModelScope 的 CPU 资源若 1 小时未使用会自动释放，环境清空。建议中途保持终端活跃或及时保存数据。
2. **存储限制**：
   - 每次仅下载一个大模型，避免 `/mnt/data` 目录爆满。
3. **依赖冲突**：
   - 若安装时报错“版本不兼容”，尝试使用 `pip uninstall` 移除旧版本后重新安装指定版本（如 `transformers==4.33.3`）。
4. **模型加载失败**：
   - 确保 `trust_remote_code=True` 参数已添加，部分国产模型（如 Qwen、ChatGLM）需加载自定义代码。
   - 检查模型路径是否正确，避免空格或中文路径。


通过以上步骤，可在 ModelScope 平台完成 CPU 环境下的大模型本地推理实践。