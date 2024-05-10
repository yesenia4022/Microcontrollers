import network

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
if not wlan.isconnected():
    print("Connecting to network...")
    wlan.connect("Kickster", "MuayThai4022!")
    while not wlan.isconnected():
        pass
print("Connected:", wlan.ifconfig())
