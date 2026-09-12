#!/usr/bin/env python3
"""Downloads official YOLOv11n weights and exports models/vision_yolo.onnx"""

import os
import shutil
from ultralytics import YOLO

def main():
    os.makedirs("models", exist_ok=True)
    print("[YOLO Export] Loading official YOLO11n weights (COCO dataset)...")
    model = YOLO("yolo11n.pt")

    exported = model.export(format="onnx", imgsz=640, opset=17, dynamic=False, simplify=True)
    dest = "models/vision_yolo.onnx"
    shutil.move(exported, dest)
    print(f"[SUCCESS] Exported: {dest} ({os.path.getsize(dest)} bytes)")

if __name__ == "__main__":
    main()