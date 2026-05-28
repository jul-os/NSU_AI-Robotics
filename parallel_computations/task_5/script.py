# n потока
# python script.py --mode multi --video walk.mp4 --output output_multi.mp4 --workers n
# 1 поток
# python script.py --mode single --video walk.mp4 --output output_single.mp4

import argparse
import time
import threading
import queue
import cv2
from ultralytics import YOLO


class Resource:
    def __init__(self, resource):
        self.resource = resource

    def __enter__(self):
        return self.resource

    def __exit__(self, *args):
        if hasattr(self.resource, "release"):
            self.resource.release()


class FrameProcessor:
    def __init__(self, model_path):
        self.model = YOLO(model_path)

    def process(self, frame):
        results = self.model(frame, device="cpu", verbose=False)
        return results[0].plot()


class Worker(threading.Thread):
    def __init__(self, in_q, out_q, model_path):
        super().__init__(daemon=True)
        self.in_q = in_q
        self.out_q = out_q
        self.processor = FrameProcessor(model_path)

    def run(self):
        while True:
            try:
                item = self.in_q.get(timeout=0.05)
                if item is None:
                    self.in_q.task_done()
                    break
                idx, frame = item
                result = self.processor.process(frame)
                self.out_q.put((idx, result))
            except queue.Empty:
                continue


def get_video_info(path):
    with Resource(cv2.VideoCapture(path)) as cap:
        fps = cap.get(cv2.CAP_PROP_FPS)
        w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        total = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    return fps, w, h, total


def process_single(video_path, output_path, model_path):
    model = YOLO(model_path)
    fps, w, h, _ = get_video_info(video_path)

    with Resource(cv2.VideoCapture(video_path)) as cap, Resource(
        cv2.VideoWriter(output_path, cv2.VideoWriter_fourcc(*"mp4v"), fps, (w, h))
    ) as out:
        start = time.time()
        while True:
            ret, frame = cap.read()
            if not ret:
                break
            results = model(frame, device="cpu", verbose=False)
            out.write(results[0].plot())
    return time.time() - start


def process_multi(video_path, output_path, model_path, n_workers):
    fps, w, h, total = get_video_info(video_path)
    in_q = queue.Queue(maxsize=16)
    out_q = queue.Queue()

    workers = [Worker(in_q, out_q, model_path) for _ in range(n_workers)]
    for t in workers:
        t.start()

    def producer():
        with Resource(cv2.VideoCapture(video_path)) as cap:
            idx = 0
            while True:
                ret, frame = cap.read()
                if not ret:
                    break
                in_q.put((idx, frame))
                idx += 1
        for _ in workers:
            in_q.put(None)

    def consumer():
        buffer = {}
        next_idx = 0
        written = 0
        with Resource(
            cv2.VideoWriter(output_path, cv2.VideoWriter_fourcc(*"mp4v"), fps, (w, h))
        ) as out:
            while written < total:
                if next_idx in buffer:
                    out.write(buffer.pop(next_idx))
                    next_idx += 1
                    written += 1
                else:
                    try:
                        idx, frame = out_q.get(timeout=0.05)
                        buffer[idx] = frame
                    except queue.Empty:
                        continue

    start = time.time()
    prod_t = threading.Thread(target=producer, daemon=True)
    cons_t = threading.Thread(target=consumer, daemon=True)
    prod_t.start()
    cons_t.start()

    prod_t.join()
    for t in workers:
        t.join()
    cons_t.join()
    return time.time() - start


def process_camera_realtime(model_path, cam_id=0):
    model = YOLO(model_path)
    with Resource(cv2.VideoCapture(cam_id)) as cap:
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        while True:
            ret, frame = cap.read()
            if not ret:
                break
            t0 = time.time()
            results = model(frame, device="cpu", verbose=False)
            annotated = results[0].plot()
            fps = 1.0 / (time.time() - t0)
            cv2.putText(
                annotated,
                f"FPS: {fps:.1f}",
                (10, 30),
                cv2.FONT_HERSHEY_SIMPLEX,
                1,
                (0, 255, 0),
                2,
            )
            cv2.imshow("Pose", annotated)
            if cv2.waitKey(1) & 0xFF == ord("q"):
                break
    cv2.destroyAllWindows()


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--video", help="Path to input video")
    p.add_argument("--mode", choices=["single", "multi", "camera"], required=True)
    p.add_argument("--output", help="Output video path")
    p.add_argument("--workers", type=int, default=4)
    p.add_argument("--model", default="yolov8s-pose.pt")
    p.add_argument("--cam", type=int, default=0, help="Camera device ID")
    args = p.parse_args()

    if args.mode == "camera":
        process_camera_realtime(args.model, args.cam)
        return

    if not args.video or not args.output:
        p.error("--video and --output required for video processing")

    if args.mode == "single":
        t = process_single(args.video, args.output, args.model)
    else:
        t = process_multi(args.video, args.output, args.model, args.workers)
    print(f"Time: {t:.2f}s")


if __name__ == "__main__":
    main()
