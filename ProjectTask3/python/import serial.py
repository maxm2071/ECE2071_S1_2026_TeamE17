import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import numpy as np
import wave
import time
import keyboard

#devices = serial.tools.list_ports.comports()

#for i in devices:
    #print(i)

ser = serial.Serial("COM10", 921600)



my_list = []

SAMPLE_RATE = 22050
clip_length = 10
is_distanceTriggerMode = input("enter 1 for distance trigger mode: ")
#ser.read(1)
ser.write(is_distanceTriggerMode.encode())

if (is_distanceTriggerMode == '1'):
    ser.timeout = 100000
    byte = ser.read(1)
    timeOut = False
    ser.timeout = 1.5
    print("HOLD 'shift' KEY TO EXIT DISTANCE TRIGGER MODE")
    while (timeOut == False):
        if (byte[0] != -1):
            bit = byte[0]
            my_list.append(bit)
        start_time = time.time()
        byte = ser.read(1)
        if (time.time()-start_time > 1):
            byte = [-1]
        
        if (keyboard.is_pressed('shift')):
            #input()
            if timeOut == False:
                print('exiting distance trigger mode')
            timeOut = True
    
        
    time.sleep(1) 
    #input()
    

        

    clip_length = len(my_list)/SAMPLE_RATE

else:
    clip_length = int(input("how long do you want to record in seconds "))
    for i in range(clip_length * SAMPLE_RATE):
        byte = ser.read(1)
        bit = byte[0]
        my_list.append(bit)

data = np.array(my_list)
#print(data)

data = (data - data.min()) / (data.max() - data.min())
data = data * 255
data = data.astype(np.uint8)

save_file = input("enter 1 to save WAV file: ")
if (save_file == '1'):
    with wave.open('audio_transmission_project.wav', 'wb') as wf:
        wf.setnchannels(1)
        wf.setsampwidth(1)
        wf.setframerate(SAMPLE_RATE)
        wf.writeframes(data.tobytes())

save_file = input("enter 1 to save png file of amplitude over time: ")
array = np.arange(0,clip_length,1/SAMPLE_RATE)
plt.plot(array,data)
plt.ylabel('Amplitude')
plt.xlabel('Time (s)')
plt.title('amplitude vs time of audio')
if (save_file == '1'):
    
    plt.savefig('audio_transmission_project.png')
    plt.show()
save_file = input("enter 1 to save a csv file of the audio data: ")
if (save_file == '1'):
    csv = np.insert(array,0,SAMPLE_RATE)
    np.savetxt('audio_transmission_project.csv',csv,delimiter=',')

#plt.show()
print("done")