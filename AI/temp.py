import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, WeightedRandomSampler
from torchvision import datasets, transforms, models
from sklearn.metrics import f1_score, classification_report
import os
import numpy as np
import time
from tqdm import tqdm
import gc


DATA_ROOT = "/mnt/data/archive/simpsons_split/"
MODEL_PATH = "/mnt/data/archive/best_resnet34_simpsons_v3.pth"

NUM_CLASSES = 18
BATCH_SIZE = 32
NUM_EPOCHS = 50
LEARNING_RATE = 0.001  # Фиксированный LR (без scheduler)
PATIENCE = 10

if torch.cuda.is_available():
    torch.cuda.empty_cache()
    gc.collect()

# ─────────────────────────────────────────────────────────────
# ТРАНСФОРМЫ
# ─────────────────────────────────────────────────────────────

train_transform = transforms.Compose(
    [
        transforms.Resize((256, 256)),
        transforms.RandomCrop((224, 224)),
        transforms.RandomHorizontalFlip(p=0.5),
        transforms.RandomRotation(20),
        transforms.ColorJitter(brightness=0.3, contrast=0.3, saturation=0.2, hue=0.1),
        transforms.RandomAffine(degrees=0, translate=(0.1, 0.1), scale=(0.9, 1.1)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
    ]
)

val_transform = transforms.Compose(
    [
        transforms.Resize((224, 224)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
    ]
)

# ─────────────────────────────────────────────────────────────
# ЗАГРУЗКА ДАННЫХ
# ─────────────────────────────────────────────────────────────

print("\n📂 Загрузка датасета...")
train_dataset = datasets.ImageFolder(
    os.path.join(DATA_ROOT, "train"), transform=train_transform
)
val_dataset = datasets.ImageFolder(
    os.path.join(DATA_ROOT, "val"), transform=val_transform
)
test_dataset = datasets.ImageFolder(
    os.path.join(DATA_ROOT, "test"), transform=val_transform
)

class_names = train_dataset.classes
print(f"📊 Классы ({len(class_names)}): {class_names}")

# ─────────────────────────────────────────────────────────────
# WEIGHTED RANDOM SAMPLER
# ─────────────────────────────────────────────────────────────

print("\n⚖️ Настройка WeightedRandomSampler...")

class_counts = np.zeros(NUM_CLASSES)
for _, label in train_dataset:
    class_counts[label] += 1

class_weights = 1.0 / class_counts
sample_weights = [class_weights[label] for _, label in train_dataset]
sample_weights = torch.DoubleTensor(sample_weights)

sampler = WeightedRandomSampler(
    weights=sample_weights, num_samples=len(sample_weights), replacement=True
)

train_loader = DataLoader(
    train_dataset,
    batch_size=BATCH_SIZE,
    sampler=sampler,
    num_workers=4,
    pin_memory=True,
)
val_loader = DataLoader(
    val_dataset, batch_size=BATCH_SIZE, shuffle=False, num_workers=4, pin_memory=True
)
test_loader = DataLoader(
    test_dataset, batch_size=BATCH_SIZE, shuffle=False, num_workers=4, pin_memory=True
)

print(
    f"📦 Train: {len(train_dataset)}, Val: {len(val_dataset)}, Test: {len(test_dataset)}"
)
print(
    f"📊 Распределение весов: мин={class_weights.min():.4f}, макс={class_weights.max():.4f}"
)

# ─────────────────────────────────────────────────────────────
# МОДЕЛЬ
# ─────────────────────────────────────────────────────────────

print("\n🏗️ Загрузка ResNet34...")
model = models.resnet34(weights=models.ResNet34_Weights.IMAGENET1K_V1)

# Замораживаем все слои
for param in model.parameters():
    param.requires_grad = False

# Размораживаем layer4 + FC
for param in model.layer4.parameters():
    param.requires_grad = True

num_features = model.fc.in_features
model.fc = nn.Linear(num_features, NUM_CLASSES)

model = model.to(DEVICE)

# ─────────────────────────────────────────────────────────────
# ОПТИМИЗАТОР И ФУНКЦИЯ ПОТЕРЬ (БЕЗ SCHEDULER!)
# ─────────────────────────────────────────────────────────────

criterion = nn.CrossEntropyLoss().to(DEVICE)

# Один learning rate для всех обучаемых параметров (проще для отчёта)
optimizer = optim.Adam(
    filter(lambda p: p.requires_grad, model.parameters()),
    lr=LEARNING_RATE,
    weight_decay=1e-4,
)

# ❌ Scheduler убран — LR фиксированный на всём обучении

# ─────────────────────────────────────────────────────────────
# МЕТРИКИ
# ─────────────────────────────────────────────────────────────


def calculate_macro_f1(predictions, labels):
    return f1_score(labels, predictions, average="macro")


def evaluate(model, loader, device):
    model.eval()
    all_preds = []
    all_labels = []

    with torch.no_grad():
        for inputs, labels in tqdm(loader, desc="Evaluating", leave=False):
            inputs = inputs.to(device, non_blocking=True)
            labels = labels.to(device, non_blocking=True)

            outputs = model(inputs)
            _, preds = torch.max(outputs, 1)

            all_preds.extend(preds.cpu().numpy())
            all_labels.extend(labels.cpu().numpy())

    return calculate_macro_f1(all_preds, all_labels)


# ─────────────────────────────────────────────────────────────
# ЦИКЛ ОБУЧЕНИЯ
# ─────────────────────────────────────────────────────────────

print("\n🚀 Начало обучения на CUDA...")
best_val_f1 = 0.0
patience_counter = 0
history = {"train_loss": [], "val_f1": []}

start_time = time.time()

for epoch in range(NUM_EPOCHS):
    print(f"\n{'='*60}")
    print(f"Эпоха {epoch+1}/{NUM_EPOCHS}")
    print(f"{'='*60}")

    # ─── TRAIN ───────────────────────────────────────────────
    model.train()
    running_loss = 0.0

    for inputs, labels in tqdm(train_loader, desc="Training", leave=False):
        inputs = inputs.to(DEVICE, non_blocking=True)
        labels = labels.to(DEVICE, non_blocking=True)

        optimizer.zero_grad()

        with torch.set_grad_enabled(True):
            outputs = model(inputs)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()

        running_loss += loss.item() * inputs.size(0)

    epoch_train_loss = running_loss / len(train_dataset)
    history["train_loss"].append(epoch_train_loss)

    # ─── VALIDATION ──────────────────────────────────────────
    val_f1 = evaluate(model, val_loader, DEVICE)
    history["val_f1"].append(val_f1)

    # ─── CHECKPOINT ──────────────────────────────────────────
    if val_f1 > best_val_f1:
        best_val_f1 = val_f1
        patience_counter = 0
        torch.save(
            {
                "epoch": epoch,
                "model_state_dict": model.state_dict(),
                "optimizer_state_dict": optimizer.state_dict(),
                "val_f1": val_f1,
                "class_names": class_names,
            },
            MODEL_PATH,
        )
        print(f"💾 Новая лучшая модель! (F1: {val_f1:.4f})")
    else:
        patience_counter += 1
        print(f"⏳ Patience: {patience_counter}/{PATIENCE}")

        if patience_counter >= PATIENCE:
            print(f"\n🛑 Early stopping на эпохе {epoch+1}")
            break

training_time = time.time() - start_time

print(f"\n{'='*60}")
print(f"✅ Обучение завершено!")
print(f"⏱️ Время: {training_time/60:.2f} мин")
print(f"🏆 Best Val Macro F1: {best_val_f1:.4f}")
print(f"{'='*60}")

# ─────────────────────────────────────────────────────────────
# ТЕСТ
# ─────────────────────────────────────────────────────────────

print("\n🧪 Оценка на TEST...")
checkpoint = torch.load(MODEL_PATH)
model.load_state_dict(checkpoint["model_state_dict"])

test_f1 = evaluate(model, test_loader, DEVICE)
print(f"🎯 Test Macro F1: {test_f1:.4f}")

# Classification Report
print("\n📋 Classification Report (Test):")
model.eval()
all_preds = []
all_labels = []
with torch.no_grad():
    for inputs, labels in test_loader:
        inputs = inputs.to(DEVICE)
        labels = labels.to(DEVICE)
        outputs = model(inputs)
        _, preds = torch.max(outputs, 1)
        all_preds.extend(preds.cpu().numpy())
        all_labels.extend(labels.cpu().numpy())

print(classification_report(all_labels, all_preds, target_names=class_names, digits=4))

# Сохранение
with open("/mnt/data/archive/results_v3.txt", "w", encoding="utf-8") as f:
    f.write(f"Training time: {training_time/60:.2f} minutes\n")
    f.write(f"Best Val Macro F1: {best_val_f1:.4f}\n")
    f.write(f"Test Macro F1: {test_f1:.4f}\n")
    f.write("\nClassification Report:\n")
    f.write(
        classification_report(all_labels, all_preds, target_names=class_names, digits=4)
    )

print("📄 Результаты: results_v3.txt")

if torch.cuda.is_available():
    torch.cuda.empty_cache()
    gc.collect()
