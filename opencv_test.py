import cv2

cap = cv2.VideoCapture(1)
cap.set(3, 1920)
cap.set(4, 1080)
# cap.set(cv2.CAP_PROP_FRAME_WIDTH, 3840)
# cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 2160)

ret,frame = cap.read()

while(True):
    cv2.imshow('img1', frame)
    if cv2.waitKey(1) & 0xFF == ord('y'):
        cv2.imwrite('/home/nvidia/Desktop/c2.png', frame)
        cv2.destroyAllWindows()
        break

print('Resolution: ' + str(frame.shape[1]) + ' x ' + str(frame.shape[0]))
cap.release()