import csv
import os.path
import serial
from serial.tools import list_ports

def send(buf):
    data=''.join([x.decode() for x in buf[15:(len(buf)-2)] if x!=b'\x00'])
    value=data.split("+")
    value.append(str(int(buf[12].hex()+buf[13].hex(),16)))
    print(value)
    ##write to file
    with open('data.csv', 'a') as csvfile:
        fieldnames = ['Buoy','Date', 'Time', 'Temperatur 1', 'Temperature 2', 'Luminosity', 'Turbidity']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        temp=value[1].split(",")
        date=value[0].split(",")
        writer.writerow({fieldnames[0]: value[4], fieldnames[1]: date[1], fieldnames[2]: date[0],
                         fieldnames[3]: temp[0], fieldnames[4]: temp[1],
                         fieldnames[5]: value[2], fieldnames[6]: value[3]})

if __name__ == "__main__":
    ##open file
    if not os.path.isfile('data.csv'):
        with open('data.csv', 'w') as csvfile:
            fieldnames = ['Buoy','Date', 'Time', 'Temperatur 1', 'Temperature 2', 'Luminosity', 'Turbidity']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
    ##finding com port  
    prev_ports=[]
    com_port="0"
    for port_no, description, address in list(list_ports.comports()):
        prev_ports.append(port_no)
    print("Please connect receiver module to receive data.")
    print("If already connected please disconnect, restart program and reconnect")
    while com_port=="0":
        curr_ports=[port_no for port_no, description, address in list(list_ports.comports())]
        if len(curr_ports) > len(prev_ports):
            com_port=list(set(curr_ports)-set(prev_ports))[0]
    print(com_port)
    #opening and reading from com port
    ser = serial.Serial(com_port,9600)
    buff=[]
    while(ser.isOpen()):
        b=ser.read(1)
        if b==b'~':
            if len(buff)>0:
                send(buff)
                buff=[]
        else:
            buff.append(b)
    
