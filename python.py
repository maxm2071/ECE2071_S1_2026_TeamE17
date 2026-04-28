import serial
import serial.tools.list_ports
import numpy as np
import wave
samplerate = 5000
i = 0
data = []


def clean_message(message):
    cleaned_message = ""
    for char in message:
        if char != "\0":
            cleaned_message += char
    return cleaned_message




def send_message(message):
    message = '>' + message + '\0'
    message = message.encode()
    ser.write(message)




# poll serial devices
devices = serial.tools.list_ports.comports()
for device in devices:
    print(device)




ser = serial.Serial("COM10", 115200)




while i < 5*samplerate:
    value = ser.read()          
    data.append(ord(value))    
    i += 1




# convert list to numpy array
data = np.array(data)


# normalise safely
data = (data - data.min()) / (data.max() - data.min())
data = data * 255
