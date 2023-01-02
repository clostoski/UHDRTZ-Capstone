#!/usr/bin/python
import requests

while True:
  r = requests.get('http://192.168.5.1/pitch')
  try:
    speed = float(r.text)
    speed *= -1
    speed = min(1, max(-5, speed))
    with open("/tmp/pitch", "w") as pitch_file:
      pitch_file.write(str(speed))
  except:
    print("Could not parse {}".format(r.text))

