import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import numpy as np
import wave

devices = serial.tools.list_ports.comports()

for i in devices:
    print(i)

ser = serial.Serial("COM8", 921600)

my_list = []

SAMPLE_RATE = 22050
clip_length = int(input("Enter the clip length: "))

for i in range(clip_length * SAMPLE_RATE):
    byte = ser.read(1)
    bit = byte[0]
    my_list.append(bit)

data = np.array(my_list)
print(len(data))

data = (data - data.min()) / (data.max() - data.min())
data = data * 255
data = data.astype(np.uint8)

with wave.open('Project_Task_1.wav', 'wb') as wf:
    wf.setnchannels(1)
    wf.setsampwidth(1)
    wf.setframerate(SAMPLE_RATE)
    wf.writeframes(data.tobytes())
array = np.arange(0,clip_length,1/SAMPLE_RATE)
plt.plot(array,data)
plt.ylabel('Amplitude')
plt.xlabel('Time (s)')
plt.title('amplitude vs time of audio')
plt.savefig('Project_Task_1.png')

csv = np.insert(array,0,SAMPLE_RATE)
np.savetxt('Project_Task_1.csv',csv,delimiter=',')

plt.show()
print("done")