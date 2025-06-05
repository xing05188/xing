import cv2
import torch
import torchvision.transforms as transforms
import torchvision.models as models
from PIL import Image

# 加载 ResNet152 模型
model = models.resnet152()
model.fc = torch.nn.Linear(model.fc.in_features, 34)
checkpoint = torch.load("hci_final/models/ResNet152.pth", map_location=torch.device('cpu'))
if "MODEL_STATE" in checkpoint:
    model_state = checkpoint["MODEL_STATE"]
else:
    model_state = checkpoint
model.load_state_dict(model_state)
model.eval()

# 手势类别列表
gesture_classes = [
    "grabbing", "grip", "holy", "point", "call", "three3", "timeout", "xsign",
    "hand_heart", "hand_heart2", "little_finger", "middle_finger", "take_picture",
    "dislike", "fist", "four", "like", "mute", "ok", "one", "palm", "peace",
    "peace_inverted", "rock", "stop", "stop_inverted", "three", "three2",
    "two_up", "two_up_inverted", "three_gun", "thumb_index", "thumb_index2",
    "no_gesture"
]

transform = transforms.Compose([
    transforms.Resize((224, 224)), 
    transforms.ToTensor(),  
    transforms.Normalize(mean=[0.54, 0.499, 0.474], std=[0.234, 0.235, 0.231]) 
])

# 推理函数
def predict(image):
    image = Image.fromarray(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
    input_tensor = transform(image).unsqueeze(0)  # 添加 batch 维度
    with torch.no_grad():
        outputs = model(input_tensor)
    predicted_class = torch.argmax(outputs, dim=1).item()
    return predicted_class

# 打开摄像头
cap = cv2.VideoCapture(0)
while True:
    ret, frame = cap.read()
    if not ret:
        break
    frame = cv2.flip(frame, 1)
    predicted_class = predict(frame)
    cv2.putText(frame, gesture_classes[predicted_class], (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
    cv2.imshow("Hand_Gesture", frame)
    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()