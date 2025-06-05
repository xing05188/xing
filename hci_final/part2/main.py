import tkinter as tk
from tkinter import messagebox
from tkinter import ttk
import subprocess
import sys
import os

def show_loading_and_run(img_path, run_func):
    # 创建加载窗口
    loading = tk.Toplevel(root)
    loading.title("加载中...")
    loading.geometry("360x260")
    loading.configure(bg="#232931")
    loading.resizable(False, False)
    try:
        img = tk.PhotoImage(file=img_path)
        img_label = tk.Label(loading, image=img, bg="#232931")
        img_label.image = img
        img_label.pack(pady=20)
    except Exception as e:
        img_label = tk.Label(loading, text="加载图片失败", fg="red", bg="#232931", font=("微软雅黑", 14))
        img_label.pack(pady=40)
    # 加载条
    progress = ttk.Progressbar(loading, orient="horizontal", length=300, mode="determinate")
    progress.pack(pady=30)
    progress["maximum"] = 100
    # 动态加载条
    def update_bar(val=0):
        if val <= 100:
            progress["value"] = val
            loading.update_idletasks()
            loading.after(30, update_bar, val+2)
        else:
            loading.destroy()
            run_func()
    update_bar()
    

def run_app1():
    def start():
        try:
            subprocess.Popen([sys.executable, "app1.py"], cwd=os.path.dirname(__file__))
        except Exception as e:
            messagebox.showerror("启动失败", f"启动球球大作战失败：{e}")
    show_loading_and_run("hci_final/Assets/app/ball_game.png",start)

def run_app2():
    def start():
        try:
            subprocess.Popen([sys.executable, "app2.py"], cwd=os.path.dirname(__file__))
        except Exception as e:
            messagebox.showerror("启动失败", f"启动我的世界失败：{e}")
    show_loading_and_run("hci_final/Assets/app/minecraft_game.png",start)

root = tk.Tk()
root.title("游戏选择-2351289周慧星")
root.geometry("500x350")
root.configure(bg="#222831")

title = tk.Label(root, text="欢迎来到游戏世界", font=("微软雅黑", 24, "bold"), fg="#FFD369", bg="#222831")
title.pack(pady=25)

frame = tk.Frame(root, bg="#222831")
frame.pack(pady=20)


img1 = tk.PhotoImage(file="hci_final/Assets/app/ball.png")
btn1 = tk.Button(
    frame, text="球球大作战", font=("微软雅黑", 12, "bold"),
    fg="#222831", bg="#00adb5", width=110, height=100,
    image=img1, compound="top",
    activebackground="#393e46", activeforeground="#FFD369",
    relief="raised", bd=4, command=run_app1
)
btn1.grid(row=0, column=0, padx=30)

img2 = tk.PhotoImage(file="hci_final/Assets/app/minecraft.png") 
btn2 = tk.Button(
    frame, text="我的世界", font=("微软雅黑", 12, "bold"),
    fg="#222831", bg="#00adb5", width=110, height=100,
    image=img2, compound="top", 
    activebackground="#393e46", activeforeground="#FFD369",
    relief="raised", bd=4, command=run_app2
)
btn2.grid(row=0, column=1, padx=30)

# 退出按钮
exit_btn = tk.Button(
    root, text="退出", font=("微软雅黑", 12),
    fg="#fff", bg="#393e46", width=10, height=1,
    command=root.quit
)
exit_btn.pack(pady=30)

root.mainloop()