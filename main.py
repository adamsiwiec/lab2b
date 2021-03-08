import time
import json
import serial
from pprint import pprint
import random
import time
import requests

# city IDs: 
# Rogers: 4128894
# Stanford: 5398563
# Warsaw: 756135

def getTemps():
    r = requests.get("https://api.openweathermap.org/data/2.5/group",
                     params={"id": "4128894,5398563,756135", "appid": "75efdcce6cdbeb47416583ae730fd24b", "units": "imperial"})
    temps = list(map(lambda city: round(city['main']['temp']),r.json()['list']))
    return temps


def write(ser, data):
    ser.write(data.encode('ascii'))
    ser.flush()

def read(ser):
    data = ser.readline().decode("utf-8").rstrip()
    jsondata = json.loads(data)

    if jsondata['op'] == 'gettemps':
        res = {'op': 'sendtemps', 'temps': getTemps()}
        write(json.dumps(res))
    return 



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
        time.sleep(0.05)
        if ser.is_waiting:
            print("reading")
            print(read(ser))






