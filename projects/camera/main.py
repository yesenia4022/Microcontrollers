import urequests
import time
time.sleep(2)
import uasyncio
from camera import CameraLoop
import network
wlan = network.WLAN(network.STA_IF)  # Create a station interface
wlan.active(True)  # Activate the interface
wifis = wlan.scan()
#print(wifis)
#print("Scanning for WiFi networks, please wait...")
#authmodes = ['Open', 'WEP', 'WPA-PSK' 'WPA2-PSK4', 'WPA/WPA2-PSK']
#for (ssid, bssid, channel, RSSI, authmode, hidden) in wlan.scan():
#  print("* {:s}".format(ssid))
#  print("   - Channel: {}".format(channel))
#  print("   - RSSI: {}".format(RSSI))
#  print("   - BSSID: {:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}".format(*bssid))
#  print()
print('connecting to network...')
#wlan.connect("Larb911", "ASAR711!")  # Connect to the network
wlan.connect("Larb711", "Alejandro7")
print("After wlan connect")
print(wlan.isconnected())
#if (wlan.isconnected()):
#    url = 'https://asar-cd8e5-default-rtdb.firebaseio.com/.json'  # Change to your actual database URL
#    headers = {'Content-Type': 'application/json'}
#    data = {"message": "hello bro"}
#    response = urequests.post(url, json=data, headers=headers)
#    print(response.text)
#    response.close()
#    while not wlan.isconnected():  # Wait until connected
#        pass
#print('network config:', wlan.ifconfig())
main = CameraLoop()
print("made cameraloop object")
uasyncio.run(main.run())
