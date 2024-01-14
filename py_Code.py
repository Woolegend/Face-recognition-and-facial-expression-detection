import cv2
import numpy as np
from keras_preprocessing.image import img_to_array
from keras.models import load_model
import serial

com = serial.Serial(port="/dev/cu.usbserial-D30FB1H9",
                    baudrate=9600,
                    bytesize=serial.EIGHTBITS,
                    parity=serial.PARITY_NONE,
                    timeout=1)

face_detection = cv2.CascadeClassifier('files/haarcascade_frontalface_alt2.xml')
emotion_classifier = load_model('files/emotion_model.hdf5')
EMOTIONS = ["Angry", "Fearful", "Happy", "Sad", "Surpring", "Neutral"]
highlight = [0, 0, 0, 0, 0, 0, 0]
processing = False

if __name__ == "__main__":
    capture = cv2.VideoCapture(0)
    face_detection = cv2.CascadeClassifier('files/haarcascade_frontalface_alt2.xml')
    while True:
        if capture.isOpened():
            ret, frame = capture.read()
            frame = cv2.resize(frame, (320, 240))
            cv2.imshow('frame', frame)
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            faces = face_detection.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))
            if processing:
                # EMOTION LOGIC
                canvas = np.zeros((250, 502, 3), dtype='uint8')
                if len(faces) > 0:
                    (fX, fY, fW, fH) = faces[0]
                    roi = gray[fY:fY + fH, fX:fX + fW]
                    roi = cv2.resize(roi, (64, 64))
                    roi = roi.astype('float') / 255.0
                    roi = img_to_array(roi)
                    roi = np.expand_dims(roi, axis=0)

                    preds = emotion_classifier.predict(roi)[0]
                    preds = [preds[0] + preds[1], preds[2], preds[3], preds[4], preds[5], preds[6]]
                    fvalue = []
                    for (i, (emotion, prob)) in enumerate(zip(EMOTIONS, preds)):
                        fvalue.append(round(prob * 100))
                        text = "{}: {:.2f}%".format(emotion, prob * 100)
                        w = int(prob * 300)
                        display_roi = frame[fY:fY+fH, fX:fX+fW]
                        display_roi = cv2.resize(display_roi, (172, 230))
                        canvas[10:240, 10:182] = display_roi
                        cv2.rectangle(canvas, (10 + 182, (i * 40) + 10), (w + 10 + 182, (i * 40) + 40), (54, 67, 244), -1)
                        cv2.putText(canvas, text, (15 + 182, (i * 40) + 30), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 255), 2)
                    print(fvalue)
                    com.write('s'.encode('utf-8'))
                    for i in fvalue:
                        com.write(bytes([i]))
                    com.write('e'.encode('utf-8'))
                else:
                    cv2.putText(canvas, "none", (0, 10), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 255), 2)
                cv2.imshow('target', canvas)
                processing = False
            key = cv2.waitKey(1) & 0xFF
            if key == ord(' '):
                processing = True