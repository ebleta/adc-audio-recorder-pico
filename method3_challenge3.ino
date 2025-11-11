#include "hardware/adc.h"
#include "hardware/dma.h"

#define mic_pin 26
#define sample_rate 12000
#define buffer_size 1024

uint16_t buffer_arr[buffer_size];
int dma_chan;

//indicate data readiness
volatile bool ready = false;

void interrupt_handler() {
    //check if DMA transfer is done
        //determine if a particular channel is a cause of DMA_IRQ_0
    if (dma_channel_get_irq0_status(dma_chan)) {
        //acknowledge a channel IRQ, resetting it as the cause of DMA_IRQ_0
        dma_channel_acknowledge_irq0(dma_chan);
        //data is ready
        ready = true;

        //set the DMA initial write address
        dma_channel_set_write_addr(dma_chan, buffer_arr, true);
    }
}

void setup() {
    Serial.begin(2000000);

    //initialize the ADC HW
    adc_init();
    //initialize the gpio for use as an ADC pin.
    adc_gpio_init(mic_pin);
    //ADC input select (in our case, 0)
    adc_select_input(0);
    
    //setup the ADC FIFO
    //takes (bool en, bool dreq_en, uint16_t dreq_thresh, bool err_in_fifo, bool byte_shift)
    /*Parameters: (taken from docs)
        en -> Enables write each conversion result to the FIFO
        dreq_en -> Enable DMA requests when FIFO contains data
        dreq_thresh	-> Threshold for DMA requests/FIFO IRQ if enabled.
        err_in_fifo -> If enabled, bit 15 of the FIFO contains error flag for each sample
        byte_shift -> Shift FIFO contents to be one byte in size (for byte DMA) - enables DMA to byte buffers.
    */
    adc_fifo_setup(true, true, 1, false, false);

    adc_set_clkdiv(48000000 / sample_rate);

    //enable free-running sampling mode
    adc_run(true);

    //claim a free dma channel.
    dma_chan = dma_claim_unused_channel(true);
    //get the default channel configuration for a given channel.
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    //set the size of each DMA bus transfer in a channel configuration object
    //Parameters: c -> Pointer to channel configuration object
    //            size -> See enum for possible values
    //16-bit transfer
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    //Set DMA channel read increment in a channel configuration object
    //we are not incrementing read address
    channel_config_set_read_increment(&c, false);
    //set DMA channel write increment in a channel configuration object (the memory buffer)
    channel_config_set_write_increment(&c, true);
    //select a transfer request signal in a channel configuration object
    //DREQ: a Data Request from the system
    channel_config_set_dreq(&c, DREQ_ADC);

    //takes: (uint channel, const dma_channel_config *config, volatile void *write_addr, 
    //        const volatile void *read_addr, uint transfer_count, bool trigger)
    /*Parameters:
        channel -> DMA channel
        config -> Pointer to DMA config structure
        write_addr -> Initial write address
        read_addr	-> Initial read address
        transfer_count -> Number of transfers to perform
        trigger	-> True to start the transfer immediately
    */
    dma_channel_configure(dma_chan, &c, buffer_arr, &adc_hw->fifo, buffer_size, false);

    //enable single DMA channel’s interrupt via DMA_IRQ_0
    dma_channel_set_irq0_enabled(dma_chan, true);
    //calling irq_set_exclusive_handler() at runtime to install a single handler for the interrupt on the current core
    irq_set_exclusive_handler(DMA_IRQ_0, interrupt_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    //start DMA
    dma_channel_start(dma_chan);
}

void loop() {
    //is data ready?
    if (ready) {
        ready = false;
        //send data
        for (int i = 0; i < buffer_size; i++) {
            Serial.println(buffer_arr[i]);
        }
    }
}
