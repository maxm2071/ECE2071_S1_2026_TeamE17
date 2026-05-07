devices = serial.tools.list_ports.comports()

for i in devices:
    print(i)

ser = serial.Serial("COM10", 115200)