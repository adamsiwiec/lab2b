import time
import json
import serial
from pprint import pprint
import random
import time
import requests

def write(ser, data):
    ser.write(data.encode('ascii'))
    ser.flush()

def read(ser):
    return ser.readline().decode("utf-8")



def waitForSetup(ser):
    if ser.isOpen():
        while True:
            try:
                incoming = ser.readline().decode("utf-8").rstrip()
                print(incoming)
                print("Setup completed")
                print(incoming == "Setup completed")
                if incoming == "Setup completed":
                    print("returning")
                    return True
            except Exception as e:
                print(e)
                return False
    else:
        print("opening error")
        return False


if __name__ == "__main__":
    print("Ready...")
    ser = serial.Serial("COM8", baudrate=115200,
                        timeout=2.5,
                        parity=serial.PARITY_NONE,
                        bytesize=serial.EIGHTBITS,
                        stopbits=serial.STOPBITS_ONE
                        )

    print("waiting for setup")
    waitForSetup(ser)
    print("setup")
    while True:
        data = {}
        data["op"] = random.randint(0,98)

        print("writing " + str(data["op"]))
        data = json.dumps(data)
        r = requests.get("https://api.openweathermap.org/data/2.5/weather",
                     params={"q": "Rogers", "appid": "75efdcce6cdbeb47416583ae730fd24b", "units": "imperial"})
        res = r.json()['main']
        res['temp'] = round(res['temp'])
        write(ser, json.dumps(res))
        time.sleep(5)
        # print("reading")
        # print(read(ser))






