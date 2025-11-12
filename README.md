# Audio Recorder
[HCI Engineering] Assignment 3

This project demonstrates analog audio recording on the Raspberry Pi Pico using a microphone connected to its ADC input. It includes two implementations:
- Polling-based sampling: Simple, CPU-driven analog reads.
- DMA-based sampling: Efficient, interrupt-driven ADC sampling using the RP2040's DMA controller.

A Python script is included to read data over USB serial and save it as a 16-bit signed PCM file, which can be played or converted with tools like Audacity.

## Hardware
- Raspberry Pi Pico (RP2040)
- Electret microphone breakout board (or analog mic with amp)
- USB connection to host computer
- Breadboard + jumper wires

## Set up
1. **Connect the Microphone**
   - Connect the analog output of your microphone breakout board to **GPIO 26 (ADC0)** on the Pico.
   - Power it using the **3.3V** and **GND** pins on the Pico.

2. **Upload Code**
   - Open either `no_dma/adc_polling.ino` or `with_dma/adc_dma.ino` in the Arduino IDE.
   - Select **Raspberry Pi Pico** as the board and upload the sketch.

3. **Run the Python Recorder**
   - Edit script to set the correct serial port (e.g. `COM4`, `/dev/ttyUSB0`, etc.).
   - Run the script:
     ```bash
     python withdma.py
     ```

     or

     ```bash
     python nodma.py
     ```

4. **Play Back the Audio**

   **Use Audacity**
   - Open **Audacity**.
   - Go to **File > Import > Raw Data...**
   - Select your `.pcm` file (e.g., `recording.pcm`)
   - In the import dialog:
     - **Encoding**: Signed 16-bit PCM
     - **Byte order**: Little-endian
     - **Channels**: 1 (Mono)
     - **Sample rate**: `12000 Hz` (or whatever your `SAMPLE_RATE` is)
   - Click **Import**. You should now see and hear the waveform.
