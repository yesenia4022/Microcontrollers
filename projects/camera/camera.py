import gc
import math
import uasyncio
import micropython
from array import array
from uasyncio import Event
from ucollections import namedtuple
import urequests
from utils import array_filled
from pinmap import I2C_CAMERA
from config import Config
import mlx90640
from mlx90640.calibration import NUM_ROWS, NUM_COLS, IMAGE_SIZE, TEMP_K
from mlx90640.image import ChessPattern, InterleavedPattern

class CameraLoop:
    def __init__(self):
        self.camera = mlx90640.detect_camera(I2C_CAMERA)
        self.camera.set_pattern(ChessPattern)

        self.update_event = Event()
        self.state = None
        self.image = None
        self.default = Config()

        try: 
            config = Config()
            config.load('config.json')
            self.reload_config(config)
            print("config loaded.")
        except Exception as err:
            print(f'Failed to load config: {err}')
            self.reload_config(self.default)

    def reload_config(self, config):
        self.set_refresh_rate(config.refresh_rate)
        self.bad_pix = config.bad_pixels
        self.debug = config.debug

    def set_refresh_rate(self, value):
        self.camera.refresh_rate = value
        self._refresh_period = math.ceil(1000/self.camera.refresh_rate)

    async def run(self):
        await uasyncio.sleep_ms(80 + 2 * int(self._refresh_period))
        print("setup camera...")
        self.camera.setup()
        self.image = self.camera.image

        tasks = [
            self.stream_images()
            #self.print_temp()
        ]
        #if self.debug:
         #   tasks.append(self.print_mem_usage())
        await uasyncio.gather(*tasks)
    async def print_temp(self):
        print(self.image.buf)
        print(self.image.v_ir)

    def _calc_temp(self, idx):
        return self.image.calc_temperature(idx, self.state)

    def _calc_temp_ext(self, idx):
        return self.image.calc_temperature_ext(idx, self.state)

    def calc_reticle_temperature(self):
        reticle = (367, 368, 399, 400)
        temp = sum(self._calc_temp_ext(idx) for idx in reticle)
        return temp/4

    async def wait_for_data(self):
        await uasyncio.wait_for_ms(self._wait_inner(), int(self._refresh_period))

    async def _wait_inner(self):
        while not self.camera.has_data:
            await uasyncio.sleep_ms(50)

    async def post_to_firebase(self, temp_data):
        url = 'https://asar-cd8e5-default-rtdb.firebaseio.com/temperatures.json'
        headers = {'Content-Type': 'application/json'}
        gc.collect()
        response = urequests.post(url, json=temp_data, headers=headers)
        # Check response and print result
        if response.status_code == 200:
            print("Temperature values uploaded successfully!")
        else:
            print(f"Failed to upload temperatures: {response.text}")
        response.close()
        gc.collect()

    async def stream_images(self):
        print("start image read loop...")
        sp = 0
        await self.wait_for_data()
            
        self.camera.read_image(sp)

        self.state = self.camera.read_state()
        self.image = self.camera.process_image(sp, self.state)
        self.image.interpolate_bad_pixels(self.bad_pix)
        chunk_size = 128
        total_pixels = len(self.image.buf)  # Assuming this is 768 as per MLX90640 full resolution
        num_chunks = (total_pixels + chunk_size - 1) // chunk_size  # Calculate number of chunks needed
        print(num_chunks)
        for i in range(num_chunks):
            print(i)
            start_index = i * chunk_size
            end_index = min(start_index + chunk_size, total_pixels)
            current_temperatures = []

            for idx in range(start_index, end_index):
                temperature = self.image.calc_temperature(idx, self.state)
                current_temperatures.append(temperature)

            # Send current chunk of temperatures to Firebase
            await self.post_to_firebase(current_temperatures)
        #current_temperatures = []
        #for idx in range(len(self.image.buf)):
        #    temperature = self.image.calc_temperature(idx, self.state)
            #print(f'pixel {idx} Temperature: {temperature:.2f} C')
        #    if idx < 128:
        #        current_temperatures.append(temperature)
        #sp = int(not sp)
        #await self.post_to_firebase(current_temperatures)
        #print(current_temperatures)

        self.update_event.set()

        await uasyncio.sleep_ms(int(self._refresh_period * 0.8))

    async def print_mem_usage(self):
        while True:
            await uasyncio.sleep(5)
            micropython.mem_info()
