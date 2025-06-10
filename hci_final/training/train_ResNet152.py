import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms, models
from torch.utils.data import DataLoader


# 运行前的准备
# 1. 安装依赖库
#    pip install torch torchvision

# 2. 准备数据集（https://github.com/hukenovs/hagrid.git），请自行下载并解压到同目录dataset/下
#    数据集目录结构应如下：
#    dataset/
#      train/
#        class1/
#          img1.jpg
#          img2.jpg
#          ...
#        class2/
#          ...
#      val/
#        class1/
#          ...
#        class2/
#          ...

# 3. 确认类别数num_classes与实际类别一致
# 4. 建议使用GPU运行，确保CUDA可用（否则自动使用CPU）


# 参数
data_dir = 'dataset'  # 数据集路径
batch_size = 32
num_epochs = 20
num_classes = 34  # 按你实际的类别数修改
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

# 数据增强与标准化
train_transforms = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.RandomHorizontalFlip(),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.54, 0.499, 0.474], std=[0.234, 0.235, 0.231])
])
val_transforms = transforms.Compose([
    transforms.Resize((224, 224)), 
    transforms.ToTensor(),  
    transforms.Normalize(mean=[0.54, 0.499, 0.474], std=[0.234, 0.235, 0.231]) 
])
# 加载数据
train_dataset = datasets.ImageFolder(root=f'{data_dir}/train', transform=train_transforms) 
val_dataset = datasets.ImageFolder(root=f'{data_dir}/val', transform=val_transforms)
train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True, num_workers=4)
val_loader = DataLoader(val_dataset, batch_size=batch_size, shuffle=False, num_workers=4)

# 构建ResNet152模型
model = models.resnet152(weights=None)  # 不加载预训练权重
#model = models.resnet152(pretrained=True)  # 加载预训练权重
model.fc = nn.Linear(model.fc.in_features, num_classes)
model = model.to(device)

# 损失函数与优化器
criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=1e-4)

# 训练与验证循环
for epoch in range(num_epochs):
    model.train()
    running_loss = 0.0
    correct = 0
    total = 0
    for imgs, labels in train_loader:
        imgs, labels = imgs.to(device), labels.to(device)
        optimizer.zero_grad()
        outputs = model(imgs)
        loss = criterion(outputs, labels)
        loss.backward()
        optimizer.step()
        running_loss += loss.item() * imgs.size(0)
        _, predicted = torch.max(outputs, 1)
        correct += (predicted == labels).sum().item()
        total += labels.size(0)
    train_acc = correct / total
    train_loss = running_loss / total

    # 验证
    model.eval()
    val_loss = 0.0
    val_correct = 0
    val_total = 0
    with torch.no_grad():
        for imgs, labels in val_loader:
            imgs, labels = imgs.to(device), labels.to(device)
            outputs = model(imgs)
            loss = criterion(outputs, labels)
            val_loss += loss.item() * imgs.size(0)
            _, predicted = torch.max(outputs, 1)
            val_correct += (predicted == labels).sum().item()
            val_total += labels.size(0)
    val_acc = val_correct / val_total
    val_loss = val_loss / val_total

    print(f"Epoch [{epoch+1}/{num_epochs}] Train Loss: {train_loss:.4f} Acc: {train_acc:.4f} | Val Loss: {val_loss:.4f} Acc: {val_acc:.4f}")

# 保存模型
torch.save(model.state_dict(), 'ResNet152.pth')