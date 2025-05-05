import gymnasium as gym
import numpy as np
from DQN import DQN_Agent
import pygame

env = gym.make('CartPole-v1', render_mode='human')
state_size = env.observation_space.shape[0]
action_size = env.action_space.n
agent = DQN_Agent(state_size, action_size)
agent.load("./save/cartpole-dqn.weights.h5")
agent.Epsilon = 0
pygame.init()
font = pygame.font.Font(None, 36)  # 使用36号字体
text_color = (255, 255, 255)  # 白色文字
background_color = (0, 0, 0)  # 黑色背景（用于覆盖旧文本）
state, _ = env.reset()
state = np.reshape(state, [1, state_size])
total_reward = 0
done = False
while not done:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            env.close()
            pygame.quit()
            exit()
    action = np.argmax(agent.model.predict(state, verbose=0)[0])
    next_state, reward, terminated, truncated, _ = env.step(action)
    done = terminated or truncated
    state = np.reshape(next_state, [1, state_size])
    total_reward += reward
    screen = pygame.display.get_surface()
    if screen is not None:
        text = font.render(f"Score: {total_reward}", True, text_color)
        text_rect = text.get_rect(topleft=(10, 10))
        pygame.draw.rect(screen, background_color, text_rect)  # 清空背景
        screen.blit(text, text_rect)
        pygame.display.update(text_rect)  # 局部更新，减少闪烁
final_text = font.render(f"Final Score: {total_reward}", True, text_color)
final_rect = final_text.get_rect(center=(200, 150))  # 居中显示
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            env.close()
            pygame.quit()
            exit()
    screen = pygame.display.get_surface()
    if screen is not None:
        screen.fill(background_color)  # 清空背景
        screen.blit(final_text, final_rect)
        pygame.display.flip()