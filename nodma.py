import serial
import time
import numpy as np


#connecting
arduino = serial.Serial(port='/dev/cu.usbmodem101', baudrate=2000000, timeout=1)
#in seconds
duration = 20
file_output = "foreverandalwaysyoung.pcm"

def record_audio():
    try:
        with arduino as ser:
            print(f"Recording for {duration} seconds...")
            start_time = time.time()
            samples = []

            while time.time() - start_time < duration:
                line = ser.readline().strip()
                if line:
                    try:
                        sample = int(line)
                        #audacity is reading your file as a 16 bit PCM file
                        #PCM audio typically expects 16-bit signed integers in the range −32768 to 32767
                        #So... we convert arduino's range (0-1023) to -32768 to 32767
                        #   and then to 16-bit signed integers in the range -32768 to 32767
                        #   audacity expects 16-bit signed integers in the range -32768 to 32767
                        sample = int(((sample / 1023.0) * 65535) - 32768)
                        samples.append(sample)
                    except ValueError:
                        continue

            print(f"{len(samples)} samples collected.")

            #normalizing samples by putting them in a numpy array
            samples = np.array(samples).astype(np.uint16)

            #save PCM file
            with open(file_output, "wb") as f:
                samples.tofile(f)

            print(f"Saved PCM audio to {file_output}")

    except serial.SerialException as e:
        print(f"Serial error: {e}")

if __name__ == "__main__":
    record_audio()
