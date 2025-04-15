import gradio as gr
import openai
import json
import os
from typing import Dict, List

#调用API
client = openai.OpenAI(
    api_key=os.getenv("DASHSCOPE_API_KEY"),
    base_url="https://dashscope.aliyuncs.com/compatible-mode/v1",  # 使用阿里云大模型 API
)
# 检查是否正确设置了 API
try:
    response = client.chat.completions.create(
        model="qwen-turbo",  # 使用阿里云 DashScope 的模型
        messages=[{'role': 'user', 'content': "测试"}],  # 设置一个简单的测试消息
        max_tokens=1,
    )
    print("API 设置成功！！")  # 输出成功信息
except Exception as e:
    print(f"API 可能有问题，请检查：{e}")  # 输出详细的错误信息


# 定义保存历史记录的 JSON 文件路径
HISTORY_FILE = "chat_history.json"

# 原初始数据（包含系统消息）
role_setting = [{"role": "system", "content": "你是一个甜甜的女友，在和男朋友聊天时语气要温柔、甜蜜。"}]
initial_history = role_setting + [
    {"role": "assistant", "content": "🌸 宝宝，今天过得怎么样呀～"}
]
# 显示用的初始化数据（仅包含assistant消息）
initial_display = [initial_history[-1]]  # 只取最后一条助理消息

# 自定义CSS样式
custom_css = """
/* 整体背景 */
.gradio-container {
    background: linear-gradient(135deg, #ffe6f0 0%, #ffd9eb 100%) !important;
    min-height: 100vh;
}
/* 标题样式 */
#title {
    color: #ff6699 !important;
    text-align: center;
    font-family: 'Microsoft YaHei';
    font-size: 2.2em !important;
    text-shadow: 2px 2px 4px rgba(255,102,153,0.2);
}

/* 聊天框样式 */
#chatbot_display {
    background: rgba(255,255,255,0.9);
    border-radius: 10px !important;
    box-shadow: 0 4px 6px rgba(0,0,0,0.05) !important;
    min-height: 500px;
    padding: 20px !important;
}

/* 用户消息气泡 */
.user-message {
    background: #90ee90 !important;
    border: 1px solid #adff2f !important;
    border-radius: 15px 15px 3px 15px !important;
    color: #333;
    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.1);
}

/* AI消息气泡 */
.assistant-message {
    background: #ffffff !important;
    border: 1px solid #dcdfe6 !important;
    border-radius: 15px 15px 15px 3px !important;
    color: #333;
    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.1);
}
/* 输入框样式 */
#input_box {
    background: rgba(255,255,255,0.9) !important;
    border-radius: 12px !important;
    border: 2px solid #ff99cc !important;
    padding: 15px !important;
}

/* 按钮样式 */
button#send_btn {
    background: #ff6699 !important;
    color: white !important;
    border-radius: 50% !important;
    padding: 12px 20px !important;
    transition: all 0.3s !important;
}
button#send_btn:hover {
    transform: scale(1.1);
    box-shadow: 0 4px 12px rgba(255,102,153,0.4) !important;
}

button#new_dialog_btn {
    background: #ff99cc !important;
    border-radius: 10px !important;
    color: white !important;
    padding: 12px 25px !important;
    margin-bottom: 20px !important;
}

/* 历史记录列表 */
#conversation_list {
    background: rgba(255,255,255,0.9) !important;
    border-radius: 12px !important;
    padding: 10px !important;
    min-height: 400px;
}

/* 历史记录标题 */
#history_title {
    color: #ff6699 !important;
    font-family: 'Microsoft YaHei';
    font-size: 1.2em !important;
    margin-bottom: 10px !important;
}
#label {
    position: absolute;
    top: 10px;
    right: 10px;
    background-color: #FF69B4;
    color: white;
    padding: 5px 10px;
    border-radius: 5px;
    font-size: 16px;
}
"""

#加载存档
def load_history() -> Dict[str, List[dict]]:
    try:
        if os.path.exists(HISTORY_FILE):
            with open(HISTORY_FILE, "r", encoding="utf-8") as f:
                data = json.load(f)
                if isinstance(data, dict):
                    return data
    except Exception as e:
        print(f"发生未知错误: {e}")
    return {}


#保存存档
def save_history(history_id: str, history_data: List[dict]):
    all_history = load_history()
    all_history[history_id] = history_data
    with open(HISTORY_FILE, "w", encoding="utf-8") as f:
        json.dump(all_history, f, ensure_ascii=False, indent=2)

#获取存档的数量，并排序
def get_history_list() -> List[str]:
    history = load_history()
    return sorted(history.keys(), reverse=True)

#生成新的存档编号
def generate_history_id() -> str:
    sorted_list = get_history_list()
    first_element = int(sorted_list[0][2:]) if sorted_list else 0
    return "对话"+str(first_element + 1)

#加载选择的对话
def load_selected_history(history_id: str):
    all_history = load_history()
    if history_id in all_history:
       history_data = all_history[history_id]
       display_data = [msg for msg in history_data if msg["role"] != "system"]
       return history_data, display_data,history_id
    return initial_history, initial_display,generate_history_id()

#创建新的对话
def create_new_chat():
    new_id = generate_history_id()
    save_history(new_id, initial_history)
    return initial_history,initial_display,new_id,gr.update(choices=get_history_list())


# 定义聊天机器人的核心函数，用于处理用户输入并生成回复
def chatbot(input_text: str, history: List[dict], history_id: str):
    if not history_id:
        history_id = generate_history_id()
    history = history or []
    new_message = {"role": "user", "content": input_text}
    history.append(new_message)
    try:
        response = client.chat.completions.create(
            model="qwen-turbo",
            messages=history,
            max_tokens=1024
        )
        reply = response.choices[0].message.content
        history.append({"role": "assistant", "content": reply})
    except Exception as e:
        history.append({"role": "assistant", "content": f"发生错误：{e}"})

    save_history(history_id,history)
    return history, history,""


# 使用 Gradio 的 Blocks 接口创建一个交互式界面
with gr.Blocks(css=custom_css) as demo:
    gr.Markdown("""
           <div id="title">💖 甜心女友聊天室</div>
           <div id="label">2351289周慧星</div>
           """)
    # 整体行布局，划分左右 区域
    with gr.Row():
        with gr.Column(scale=1, min_width=250):
            new_dialog_btn = gr.Button("✨ 新的聊天", variant="primary", elem_id="new_dialog_btn")
            gr.Markdown("📚 聊天记录", elem_id="history_title")
            history_dropdown = gr.Dropdown(
                choices=get_history_list(),
                label="选择历史对话",
                interactive=True
            )
        # 右侧区域：聊天显示 + 输入交互区
        with gr.Column(scale=4):
            chatbot_messages = gr.Chatbot(
                value=initial_display,
                elem_id="chatbot_display",
                type="messages",
                avatar_images=("user.png", "girl.png")
            )
            chat_history = gr.State(initial_history)
            history_id = gr.State("对话1")
            with gr.Row(elem_id="input_row"):
                input_text = gr.Textbox(
                    placeholder="给 AI 女友 发送消息",
                    show_label=False,
                    container=False,
                    elem_id="input_box",
                )
                send_btn = gr.Button("➤", elem_id="send_btn")

    # 绑定发送按钮点击事件
    send_btn.click(
        fn=chatbot,
        inputs=[input_text, chat_history,history_id],
        outputs=[chatbot_messages, chat_history, input_text]
    )
    # 绑定输入框回车事件
    input_text.submit(
        fn=chatbot,
        inputs=[input_text, chat_history,history_id],
        outputs=[chatbot_messages, chat_history, input_text]
    )
    # 简单示例：点击新对话按钮清空聊天记录（实际需更完善的对话管理逻辑）
    new_dialog_btn.click(
        fn=create_new_chat,
        outputs=[chat_history,chatbot_messages,history_id,history_dropdown]
    )

    history_dropdown.change(
        fn=load_selected_history,
        inputs=[history_dropdown],
        outputs=[chat_history, chatbot_messages, history_id]
    )


# 启动 Gradio 应用程序，使其可以在浏览器中访问
demo.launch(share=True)