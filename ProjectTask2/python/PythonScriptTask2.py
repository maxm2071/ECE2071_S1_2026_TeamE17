import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import numpy as np
import wave
import time

devices = serial.tools.list_ports.comports()

for i in devices:
    print(i)

ser = serial.Serial("COM3", 115200)



my_list = []

SAMPLE_RATE = 10000
clip_length = 10
is_distanceTriggerMode = input("enter 1 for distance trigger mode: ")
ser.write(is_distanceTriggerMode.encode())

if (is_distanceTriggerMode == '1'):
    ser.timeout = 100000
    byte = ser.read(1)
    timeOut = False
    ser.timeout = 1.5
    while (timeOut == False):
        bit = byte[0]
        my_list.append(bit)
        start_time = time.time()
        byte = ser.read(1)
        if (time.time()-start_time > 1):
            timeOut = True

    clip_length = len(my_list)/SAMPLE_RATE

else:
    clip_length = int(input("how long do you want to record in seconds "))
    for i in range(clip_length * SAMPLE_RATE):
        byte = ser.read(1)
        bit = byte[0]
        my_list.append(bit)

data = np.array(my_list)
print(data)

data = (data - data.min()) / (data.max() - data.min())
data = data * 255
data = data.astype(np.uint8)

save_file = input("enter 1 to save WAV file: ")
if (save_file == '1'):
    with wave.open('Project_Task_2.wav', 'wb') as wf:
        wf.setnchannels(1)
        wf.setsampwidth(1)
        wf.setframerate(SAMPLE_RATE)
        wf.writeframes(data.tobytes())

save_file = input("enter 1 to save png file of amplitude over time: ")
if (save_file == '1'):
    array = np.arange(0,clip_length,1/SAMPLE_RATE)
    plt.plot(array,data)
    plt.ylabel('Amplitude')
    plt.xlabel('Time (s)')
    plt.title('amplitude vs time of audio')
    plt.savefig('Project_Task_2.png')
    plt.show()
save_file = input("enter 1 to save a csv file of the audio data: ")
if (save_file == '1'):
    csv = np.insert(array,0,SAMPLE_RATE)
    np.savetxt('Project_Task_2.csv',csv,delimiter=',')

#plt.show()
print("done")