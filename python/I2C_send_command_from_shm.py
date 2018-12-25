from ctypes import*
import smbus
import time

SHM_SIZE = 8
SHM_KEY = 8787
#now = 0x00
#before = 0x00

bus = smbus.SMBus(1)
address = 0x04

try:
    rt = CDLL('librt.so')
except:
    rt = CDLL('librt.so.1')

def get_I2C_data(get_data, I2C_data, display):
    
    I2C_data[1] = get_data[0:2]
    I2C_data[3] = get_data[2:4]
    I2C_data[5] = get_data[4:6]
    if(display):
        print(I2C_data[0] + I2C_data[1])
        print(I2C_data[2] + I2C_data[3])
        print(I2C_data[4] + I2C_data[5])
    return 0

def I2C_data_to_int(I2C_data, display):
    
    I2C_data[1] = int(I2C_data[1],16)
    I2C_data[3] = int(I2C_data[3],16)
    I2C_data[5] = int(I2C_data[5],16)
    if(display):
        print('%s%x' %(I2C_data[0], I2C_data[1]))
        print('%s%x' %(I2C_data[2], I2C_data[3]))
        print('%s%x' %(I2C_data[4], I2C_data[5]))
        print('now: %x' %(I2C_data[1]))

    return 0

def I2C_send_data(I2C_data, display):

    #print('before: %x' %(data_change[1]))
    #print('now: %x' %(data_change[3]))   
    #before != now
    if(display):
        I2C_send_data_display(I2C_data, display)
    I2C_write_data(I2C_data) 
    return 0

def I2C_send_data_display(I2C_data, switch):
    if(switch):
        print('%s%x %s%x %s%x' %(I2C_data[0], I2C_data[1]
         ,I2C_data[2], I2C_data[3]
         ,I2C_data[4], I2C_data[5]))
        print('--------------------------------')
    return 0 

def I2C_write_data(I2C_data):
    bus.write_byte(address, I2C_data[1])
    bus.write_byte(address, I2C_data[3])
    bus.write_byte(address, I2C_data[5])
    return 0

try:
    shmget = rt.shmget
    shmget.argtypes = [c_int, c_size_t, c_int]
    shmget.restype = c_int
    shmat = rt.shmat
    shmat.argtypes = [c_int, POINTER(c_void_p), c_int]
    shmat.restype = c_void_p

    shmid = shmget(SHM_KEY, SHM_SIZE, 0o666)
    
    if shmid < 0:
        print ("System not infected")
    else:
        addr = shmat(shmid, None, 0)

    data_change = [ 'before: ', 0x00, 'now:', 0x00]

    while True:
        get_data = string_at(addr, SHM_SIZE)
        #print(get_data)
        data_change[3] = get_data;
        if(data_change[1] != data_change[3] ):
            I2C_data = ['command: ', 0x00, 'valueH: ', 0x00, 'valueL: ', 0x00]
        
            get_I2C_data(get_data, I2C_data , 0)
        
            I2C_data_to_int(I2C_data, 0)
        
            I2C_send_data(I2C_data, 1)
        data_change[1] = get_data;
        #time.sleep(0.1)
finally:
    print("exit")



