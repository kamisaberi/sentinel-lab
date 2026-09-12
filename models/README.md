# Sentinel-Lab Model Directory

This directory stores compiled and pre-trained ONNX models evaluated by Sentinel-Lab.

## Default Models
- `network_threat.onnx`: Evaluates 32 NetFlow features (CIC-IDS-2017). Input tensor: `input`, Output tensor: `scores`.
- `vision_yolo.onnx`: Evaluates 640x640 video frames for physical perimeter breach detection.

## Auto-Download
Models referenced in `configs/models.json` will be fetched automatically via HTTPS by `xinfer::ModelHub` if they are not found locally on startup.