from motmot.FlyMovieFormat.FlyMovieFormat import *
import cv2

def main():
  cv2.namedWindow("input")
  vidcap = cv2.VideoCapture()
  vidcap.open(0)
  while True:
    f, img = vidcap.read()
    cv2.imshow("input", img)
    cv2.waitKey(1)

if __name__ == "__main__":
  main()
