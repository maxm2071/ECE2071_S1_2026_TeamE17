import serial
import serial.tools.list_ports
import numpy as np
import wave

devices = serial.tools.list_ports.comports()

for i in devices:
    print(i)

ser = serial.Serial("COM3", 115200)

my_list = []

SAMPLE_RATE = 5000

for i in range(10 * SAMPLE_RATE):
    byte = ser.read(1)
    bit = byte[0]
    my_list.append(bit)

data = np.array(my_list)
print(data)

data = (data - data.min()) / (data.max() - data.min())
data = data * 255
data = data.astype(np.uint8)

with wave.open('Project_Task_1.wav', 'wb') as wf:
    wf.setnchannels(1)
    wf.setsampwidth(1)
    wf.setframerate(SAMPLE_RATE)
    wf.writeframes(data.tobytes())