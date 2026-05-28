# python main.py --camera /dev/video0 --resolution 1280x720 --fps 1

import sys
import numpy as np
import time
from typing import Any
import cv2
import logging
import argparse
import threading
import queue
import os

os.makedirs("log", exist_ok=True)

logging.basicConfig(
    filename="log/app.log",
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    filemode="w",
)
logger = logging.getLogger(__name__)


class Sensor:
    def get(self):
        raise NotImplementedError("Subclasses must implement this method")


class SensorX(Sensor):
    # условие лабы: Класс SensorX не модифицировать!
    def __init__(self, delay: float):
        self._delay = delay
        self._data = 0

    def get(self) -> int:
        time.sleep(self._delay)
        self._data += 1
        return self._data


class SensorCam(Sensor):
    def __init__(self, name: str, height: int, width: int):
        self.name = name
        self.height = height
        self.width = width
        self.cap = cv2.VideoCapture(name)
        if not self.cap.isOpened():
            self.cap.release()
            logging.error(f"Camera '{name}' is not working properly.")
            raise RuntimeError(f"Camera '{name}' is not working properly.")

        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
        logging.info(f"Camera '{name}' initialized with resolution {width}x{height}")

    def get(self):
        ret, frame = self.cap.read()
        if not ret:
            logging.error("Failed to capture image from camera")
            return None
        return frame

    def __del__(self):
        if hasattr(self, "cap") and self.cap.isOpened():
            self.cap.release()
            logging.info(f"Camera '{self.name}' released")


class WindowImage:
    def __init__(self, window_name: str, display_fps: float):
        self.window_name = window_name
        self.display_fps = display_fps
        cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
        logging.info(f"Window '{window_name}' created with display FPS: {display_fps}")

    def show(self, image):
        if image is None:
            return None
        cv2.imshow(self.window_name, image)
        wait_time = max(1, int(1000 / self.display_fps))
        key = cv2.waitKey(wait_time) & 0xFF
        return key

    def __del__(self):
        cv2.destroyWindow(self.window_name)
        logging.info(f"Window '{self.window_name}' destroyed")


def get_latest_from_queue(q: queue.Queue, last_value: Any) -> Any:
    try:
        while True:
            last_value = q.get_nowait()  # Опустошить очередь, оставить только последнее
    except queue.Empty:
        pass
    return last_value


def sensor_worker(sensor: Sensor, out_queue: queue.Queue, stop_event: threading.Event):
    while not stop_event.is_set():
        data = sensor.get()
        if data is not None:
            try:
                out_queue.put(data, block=False)
            except queue.Full:
                pass  # Пропускаем, если очередь полна
        time.sleep(0)  # Yield GIL для кооперативной многозадачности


def main():
    parser = argparse.ArgumentParser(description="Multi-sensor camera display")
    parser.add_argument(
        "--camera", type=str, default="0", help="Camera device name/index"
    )
    parser.add_argument(
        "--resolution", type=str, default="1280x720", help="Resolution like 1280x720"
    )
    parser.add_argument(
        "--fps", type=float, default=30.0, help="Display frequency in Hz"
    )
    args = parser.parse_args()

    try:
        width, height = map(int, args.resolution.split("x"))
    except ValueError:
        logging.error(f"Invalid resolution: {args.resolution}")
        sys.exit(1)

    sensor_cam = None
    if not args.test:
        try:
            cam_input = int(args.camera) if args.camera.isdigit() else args.camera
            sensor_cam = SensorCam(cam_input, height, width)
        except RuntimeError as e:
            logging.warning(f"Camera init failed: {e}. Running in test mode.")
            args.test = True

    window = WindowImage("Camera Feed", args.fps)

    # Очереди
    q_cam = queue.Queue(maxsize=1)
    queues = [queue.Queue(maxsize=1) for _ in range(3)]

    stop_event = threading.Event()
    threads = []

    # Поток камеры
    if sensor_cam is not None:
        t = threading.Thread(
            target=sensor_worker, args=(sensor_cam, q_cam, stop_event), daemon=True
        )
        t.start()
        threads.append(t)

    sensors = [SensorX(0.01), SensorX(0.1), SensorX(1.0)]
    for sensor, q in zip(sensors, queues):
        t = threading.Thread(
            target=sensor_worker, args=(sensor, q, stop_event), daemon=True
        )
        t.start()
        threads.append(t)

    time.sleep(0.5)

    last_frame = None
    last_values = [0, 0, 0]
    labels = ["100 Hz", "10 Hz", "1 Hz"]

    try:
        while not stop_event.is_set():
            if sensor_cam is not None:
                last_frame = get_latest_from_queue(q_cam, last_frame)

            for i, q in enumerate(queues):
                last_values[i] = get_latest_from_queue(q, last_values[i])

            if last_frame is not None and not args.test:
                frame = last_frame.copy()
            else:
                frame = np.zeros((480, 640, 3), dtype=np.uint8)
                cv2.putText(
                    frame,
                    "TEST MODE / NO SIGNAL",
                    (30, 40),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.7,
                    (100, 100, 255),
                    2,
                )

            y_offset = 30 if args.test else 10
            for i, (val, label) in enumerate(zip(last_values, labels)):
                color = (0, 255, 0) if val > 0 else (0, 100, 255)
                text = f"Sensor {i} ({label}): {val}"
                cv2.putText(
                    frame, text, (10, y_offset), cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2
                )
                y_offset += 30

            key = window.show(frame)
            if key == ord("q"):
                stop_event.set()

    except KeyboardInterrupt:
        stop_event.set()  # устанавливает флаг stop_event, будит другие потоки

    finally:
        stop_event.set()
        for t in threads:
            t.join(timeout=1.0)  # Ждём завершения с таймаутом
        if sensor_cam and hasattr(sensor_cam, "cap"):
            sensor_cam.cap.release()
        cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
