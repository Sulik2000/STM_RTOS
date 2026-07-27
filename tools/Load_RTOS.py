import sys
import serial

bytesReadySignal = 2 
sizeOfDataBlocks = 128

port = ""
baud = -1
file = ""

for i in sys.argv:
    if i == "--port":
        print("Port of board is: " + sys.argv[sys.argv.index(i) + 1])
        port = sys.argv[sys.argv.index(i) + 1]
    if i == "--baud":
        print("Baudrate of board is: " + sys.argv[sys.argv.index(i) + 1])
        baud = int(sys.argv[sys.argv.index(i) + 1])
    if i == "--file":
        print("File to load is: " + sys.argv[sys.argv.index(i) + 1])
        file = sys.argv[sys.argv.index(i) + 1]

if(baud == -1 or port == "" or file == ""):
    print("Please provide all required arguments: --port, --baud, --file")
    sys.exit(1)

with serial.Serial(port = port, baudrate = baud, timeout = 1) as ser:
    print("Triggered reset and bootloader state on board, waiting for board to be ready to receive data")
    # Send RTS signal to activate bootloader state
    ser.setRTS(True)
    # Send DTR signal to reset the board
    ser.setDTR(False)
    ser.setDTR(True)
    sendedBytes = 0
    
    with open(file, "rb") as f:
        file_size = f.seek(0, 2)
        f.seek(0)
        print('File opened successfully. Size of file: ' + str(file_size) + ' bytes')
        while f.readable() and ser.is_open:
            bytes = ser.read(size=bytesReadySignal) # Read 2 bytes to wait for the board to reset and ready to get data
            if bytes == b'\xDD\xDD': # Board is ready to receive data
                data = f.read(sizeOfDataBlocks)
                if not data: # End of file reached
                    break
                ser.write(data)
                sendedBytes += len(data)
                print(f"Sent {sendedBytes} bytes to board")
            if bytes == b'\xDD\x01': # Board sent error
                print("Board sent error signal, exiting")
                ser.setRTS(False)
                break

        if(ser.is_open):
            print(f"File loaded successfully, total {sendedBytes} bytes sent")
            ser.write(b'\xFF\xFF\xDD\xDD') # Send signal to board to exit bootloader state
            ser.read(2) # Wait for board to acknowledge the exit signal
            if(ser.read(2) == b'\xDD\xFF'):
                print("Board exited bootloader state successfully")
                ser.setRTS(False)
            else:
                print("Board failed to exit bootloader state")
                ser.setRTS(False)