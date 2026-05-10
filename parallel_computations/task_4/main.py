# classes.py

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

from distro import name

logging.basicConfig(filename="log/app.log", level=logging.INFO)
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
            return
        cv2.imshow(self.window_name, image)
        wait_time = max(1, int(1000 / self.display_fps))
        key = cv2.waitKey(wait_time) & 0xFF
        return key

    def __del__(self):
        cv2.destroyWindow(self.window_name)
        logging.info(f"Window '{self.window_name}' destroyed")


def get_latest_from_queue(q: queue.Queue, last_value: Any) -> Any:
    try:
        while True:  # Опустошаем очередь, оставляя только последний элемент
            last_value = q.get_nowait()
    except queue.Empty:
        pass
    return last_value


def sensor_worker(sensor: Sensor, out_queue: queue.Queue, stop_event: threading.Event):
    """
    Универсальная функция-обёртка для работы сенсора в отдельном потоке.
    Постоянно опрашивает sensor.get() и кладёт результат в очередь.
    """
    logger.info(f"Worker started for {type(sensor).__name__}")
    while not stop_event.is_set():
        data = sensor.get()
        if data is not None:
            # Удаляем устаревшие данные из очереди, чтобы хранить только актуальные
            try:
                while not out_queue.empty():
                    out_queue.get_nowait()
            except queue.Empty:
                pass
            try:
                out_queue.put(data, block=False)
            except queue.Full:
                pass  # Пропускаем кадр, если очередь переполнена
        # Для SensorX задержка уже внутри get(), для SensorCam — нет
    logger.info(f"Worker stopped for {type(sensor).__name__}")


sensor0 = SensorX(0.01)
sensor1 = SensorX(0.1)
sensor2 = SensorX(1)

parser = argparse.ArgumentParser()
parser.add_argument("--camera", type=str, default="0", help="Camera device name/index")
parser.add_argument(
    "--resolution", type=str, default="1280x720", help="Resolution like 1280x720"
)
parser.add_argument("--fps", type=float, default=30.0, help="Display frequency in Hz")
args = parser.parse_args()
# Парсинг разрешения:
width, height = map(int, args.resolution.split("x"))
try:
    cam_input = int(args.camera) if args.camera.isdigit() else args.camera
    sensor_cam = SensorCam(cam_input, height, width)
except RuntimeError as e:
    logging.error(str(e))
    sensor_cam = None

try:
    window = WindowImage("Camera Feed", args.fps)
except Exception as e:
    logging.error(f"Failed to create window: {str(e)}")
    window = None

q_cam = queue.Queue(maxsize=1)
q_0 = queue.Queue(maxsize=1)  # SensorX(0.01) ~ 100 Hz
q_1 = queue.Queue(maxsize=1)  # SensorX(0.1)  ~ 10 Hz
q_2 = queue.Queue(maxsize=1)  # SensorX(1.0)  ~ 1 Hz

stop_event = threading.Event()

threads = []

if sensor_cam is not None:
    t_cam = threading.Thread(
        target=sensor_worker,
        args=(sensor_cam, q_cam, stop_event),
        daemon=True,
        name="CameraWorker",
    )
    t_cam.start()
    threads.append(t_cam)

# Создаём симулируемые датчики
sensors = [SensorX(0.01), SensorX(0.1), SensorX(1.0)]
queues = [q_0, q_1, q_2]

for i, (sensor, q) in enumerate(zip(sensors, queues)):
    t = threading.Thread(
        target=sensor_worker,
        args=(sensor, q, stop_event),
        daemon=True,
        name=f"SensorX{i}_Worker",
    )
    t.start()
    threads.append(t)
    logger.info("All worker threads started")

    last_frame = None
    last_values = [0, 0, 0]  # Для трёх датчиков SensorX

    logger.info("Starting main display loop")
    try:
        while not stop_event.is_set():
            # Получаем самые свежие данные из очередей
            if sensor_cam is not None:
                last_frame = get_latest_from_queue(q_cam, last_frame)

            for i, q in enumerate(queues):
                last_values[i] = get_latest_from_queue(q, last_values[i])

            # aормируем изображение для отображения
            if last_frame is not None:
                frame = last_frame.copy()
                y_offset = 30
                labels = ["100 Hz", "10 Hz", "1 Hz"]
                for i, (val, label) in enumerate(zip(last_values, labels)):
                    text = f"Sensor {i} ({label}): {val}"
                    cv2.putText(
                        frame,
                        text,
                        (10, y_offset),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.7,
                        (0, 255, 0),
                        2,
                    )
                    y_offset += 30
            else:
                # Заглушка, если камера не работает
                frame = cv2.imread("log/no_signal.png")  # можно создать заглушку
                if frame is None:
                    frame = np.zeros((480, 640, 3), dtype=np.uint8)
                cv2.putText(
                    frame,
                    "NO CAMERA SIGNAL",
                    (50, 240),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    1,
                    (0, 0, 255),
                    2,
                )

            # oтображаем и проверяем ввод
            key = window.show(frame)
            if key == ord("q"):
                logger.info("Key 'q' pressed — initiating graceful shutdown")
                stop_event.set()

    except KeyboardInterrupt:
        logger.warning("Interrupted by Ctrl+C")
        stop_event.set()

    finally:
        logger.info("Shutdown sequence started")
        stop_event.set()

        # Ждём завершения потоков с таймаутом
        for t in threads:
            t.join(timeout=1.0)
            if t.is_alive():
                logger.warning(f"Thread {t.name} did not terminate in time")

        if sensor_cam is not None and hasattr(sensor_cam, "_cap"):
            sensor_cam._cap.release()

        cv2.destroyAllWindows()
        logger.info("All resources released. Program exited cleanly.")
