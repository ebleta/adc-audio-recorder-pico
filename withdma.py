import serial
import time
import numpy as np

#in seconds
duration = 8
sample_rate = 12000
file = "foreverandalwaysyoung(dma).pcm"

#connecting
arduino = serial.Serial(port='/dev/cu.usbmodem101', baudrate=2000000, timeout=1)

def record_audio():
    try:
        with arduino as ser:
            print(f"Recording for {duration} seconds...")
            start_time = time.time()
            samples = []

            while time.time() - start_time < duration:
                line = arduino.readline().strip()
                if line:
                    #try block around ensures if non integer value is read it is ignored
                    try:
                        sample = int(line)

                        #expected range
                        if 0 <= sample <= 4095:
                        #audacity expects 16 bit signed integers in the range -32768 to 32767
                        # so we convert 12 bit ADC (0-4095) to 16 bit PCM and center at 0
                            pcm_sample = (sample - 2048) * 16
                            samples.append(pcm_sample)
                    except ValueError:
                        #ignore bad data (any non integer values)
                        continue

        #print(f"{len(samples)} samples collected.")

        #normalizing samples by putting them in a numpy array
        samples = np.array(samples).astype(np.uint16)

        #save PCM file
        with open(file, "wb") as f:
            samples.tofile(f)

    except serial.SerialException as e:
        print(f"Serial error: {e}")

if __name__ == "__main__":
    record_audio()
