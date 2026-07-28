import sys
from face_sdk_3divi import FacerecService, Error, Context
import cv2

# sdk_root is provided by the user
service = FacerecService.create_service(
    f"{sdk_root}/lib/libfacerec.so", f"{sdk_root}/conf/facerec", f"{sdk_root}/license")

try:
    tracker = service.create_processing_block({
        "unit_type": "FACE_TRACKING_MODULE",
        "modification": "fda",
    })

    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Cannot open camera")
        sys.exit(1)

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        height, width = frame.shape[:2]
        ioData = service.create_context_from_frame(
            frame.tobytes(), width, height, Context.Format.FORMAT_RGB)


        tracker(ioData)

        for obj in ioData["objects"]:
            track_id = obj["track_id"].get_long()
            conf = obj["confidence"].get_double()
            bbox = obj["bbox"]
            x1 = int(bbox[0].get_double() * width)
            y1 = int(bbox[1].get_double() * height)
            x2 = int(bbox[2].get_double() * width)
            y2 = int(bbox[3].get_double() * height)
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.putText(frame, f"ID:{track_id} {conf:.2f}", (x1, y1 - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)

        cv2.imshow("Face Tracking", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

except Error as e:
    print(f"SDK error: {e}")
