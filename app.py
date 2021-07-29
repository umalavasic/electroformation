from serial import Serial
from serial.tools import list_ports

PROTOCOL_MAX_STAGES = 10

def connectDevice():
    comports = list_ports.comports()
    for port in comports:
        connection = Serial(port = port.device, baudrate = 115200, timeout = 2)
        message = b"READY" # Expected bytes
        data = connection.read(len(message))
        if data == message:
            return connection
    return None
    
def readProtocol(connection):
    data = connection.readline().decode("UTF-8")
    if "<" not in data or ">" not in data:
        return None
    data = data.translate(str.maketrans("", "", "<>"))
    if not data:
        return None
    protocol = []
    for stage in data.split(";"):
        parameters = stage.split(",")
        duration = int(parameters[0])
        frequency = float(parameters[1])
        amplitude = float(parameters[2])
        protocol.append((duration, frequency, amplitude))
    return protocol
    
def writeProtocol(protocol, connection):
    stages = []
    for stage in protocol:
        duration = stage[0]
        frequency = stage[1]
        amplitude = stage[2]
        data = str(duration) + "," + str(frequency) + "," + str(amplitude)
        stages.append(data)
    data = "<" + ";".join(stages) + ">"
    connection.write(bytes(data, "UTF-8"))
    
def displayProtocol(protocol):
    if not protocol:
        print("\t" + "Empty protocol.")
    else:
        for i in range(len(protocol)):
            stage = protocol[i]
            duration = stage[0]
            frequency = stage[1]
            amplitude = stage[2]
            print("\t" + str(i + 1) + ". t = " + str(duration) + " s, f = " + str(frequency) + " Hz, U = " + str(amplitude) + " V")
            
def takeProtocolFromUser():
    protocol = []
    print("[*] Enter new protocol:")
    for stage in range(PROTOCOL_MAX_STAGES):
        print("\t" + "Stage " + str(stage + 1) + ":")
        time = int(input(2 * "\t" + "Time [s]: "))
        frequency = float(input(2 * "\t" + "Frequency [Hz]: "))
        amplitude = float(input(2 * "\t" + "Amplitude [V]: "))
        protocol.append((time, round(frequency, 2), round(amplitude, 2)))
        if input("\t" + "Would you like to add another stage? [y/n] ") != "y":
            break
    return protocol
    
def end():
    input("[*] Press ENTER to exit ...")
    quit()
    
# 1. Connect to device.
print("[*] Searching for serial port ...")
connection = connectDevice()
if not connection:
    print("[!] ERROR: Device not connected.")
    quit()
print("[*] Device ready.")

# 2. Read protocol.
print("[*] Reading protocol on device ...")
protocolOnDevice = readProtocol(connection)
print("[*] Protocol on device:")
displayProtocol(protocolOnDevice)

# 3. Ask user to input new protocol.
response = input("[Q] Would you like to change the protocol? [y/n/sample] ")
if response == "y":
    protocol = takeProtocolFromUser()
elif response == "sample":
    protocol = [
        (3, 10.0, 5.0),
        (2, 5.0, 2.5),
        (5, 2.5, 2.5),
        (8, 1.0, 1.0)
    ]
else:
    end()

# 4. Write protocol.
print("[*] Will write protocol to device:")
displayProtocol(protocol)
writeProtocol(protocol, connection)
print("[*] Protocol sent to device.")

# 5. Read protocol.
print("[*] Reading protocol on device ...")
protocolOnDevice = readProtocol(connection)
if protocol == protocolOnDevice:
    print("[*] Protocol on device matches sent protocol.")
else:
    print("[!] ERROR: Protocol on device doesn't match sent protocol.")

end()
