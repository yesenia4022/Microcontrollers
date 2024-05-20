import urequests
import time
import uasyncio
from camera import CameraLoop
import network
import machine  # Importing machine for handling I2C

# Initialize the WLAN interface for WiFi connection
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
print('Connecting to network...')
wlan.connect("Larb911", "ASAR711!")

while not wlan.isconnected():  # Wait until connected
    pass

print('Network config:', wlan.ifconfig())

# Initialize I2C
i2c = machine.I2C(0, scl=machine.Pin(8), sda=machine.Pin(10))

# I2C address and command definitions
SLAVE_ADDR_CAMERA = 0x06
CAMERA_START_CMD = 0x05
COMPLETION_SIGNAL = 0x01

def send_completion_signal():
    """Send a completion signal back to the master device."""
    try:
        i2c.writeto(SLAVE_ADDR_CAMERA, bytes([COMPLETION_SIGNAL]))  # Signal completion back to master
        print("Completion signal sent successfully to master.")
    except Exception as e:
        print("Failed to send completion signal:", e)

async def listen_for_commands():
    print("Camera ESP32-C3 initialized and waiting for commands.")
    while True:
        await uasyncio.sleep(0.1)  # Non-blocking delay to allow for other operations
        if i2c.any():  # Check if there's data waiting
            data = i2c.readfrom(SLAVE_ADDR_CAMERA, 1)  # Read one byte from master
            if data[0] == CAMERA_START_CMD:
                print("CAMERA START command received, initializing camera operations...")
                camera_loop = CameraLoop()
                await camera_loop.run()  # Run the camera loop
                print("Camera operations completed.")
                send_completion_signal()  # Signal completion back to master

async def main():
    # Initial welcome message or system checks can be placed here
    await listen_for_commands()  # Begin listening for I2C commands indefinitely

# Run the main coroutine
uasyncio.run(main())
