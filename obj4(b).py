import numpy as np
import matplotlib.pyplot as plt
import time
import serial
#Declear serial object with 250000 baud
ser = serial.Serial('COM6', 250000)
#Declare two empty arrays to store time/EMG voltage read
timeData = np.zeros(0)
emgData = np.zeros(0)
#Get current system time as initial time
initTime = time.time()
def readData():
    global timeData, emgData, initTime
    timeArray = np.zeros(20)
    dataArray = np.zeros(20)
    for i in range(0,20):
        line = ser.readline().split()
        #Make sure the data has valid length
        while(not(len(line) == 8)):
            line = ser.readline().split()
        #Use current system time - initial time as time
        timeArray[i] = time.time() - initTime
        #Read the final filtered data from serial
        dataArray[i] = line[6]
    #Append 20 data read into time/data array
    timeData = np.append(timeData,timeArray)
    emgData = np.append(emgData, dataArray)
    if(len(timeData) > 400):
        timeData = np.delete(timeData, range(0,20))
        emgData = np.delete(emgData, range(0,20))
        
def play():
    readData()
    t = 0
    plt.ion()
    lineHandle = plt.plot(timeData, emgData)
    plt.hold(False) # updates should overwrite, not generate new signal lines
    while 1:
        try:  # we use a try to catch the Ctrl+C
            readData()
            plt.pause(0.01)
            lineHandle[0].set_ydata(emgData)
            lineHandle[0].set_xdata(timeData)
            print timeData
            plt.xlim(timeData[0], timeData[-1])
            plt.ylim(0, 0.2)
            plt.xlabel('Time(seconds)')
            plt.ylabel('EMG Data (V)')
            plt.title('Streaming EMG Data')
        except KeyboardInterrupt: # if the user interrupts end gracefully
            plt.close()
            return
    
play()