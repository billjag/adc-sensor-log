#include <stdio.h>
#include <stdlib.h>
#include "adc.h"

int main(int argc, char *argv[]) {

    FILE *file;
    ADCHeader header;
    ADCSample *samples;
    size_t read_count;
    int i;

    printf("ADC Sensor Data Processor\n");

    if (argc != 2) {
        printf("enter the binary file name:\n");
        printf("example: adc_sensor_log.bin\n");
        return 1;
    }
    printf("file selected: %s\n", argv[1]);

    file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("ERROR: could not open the file\n");
        return 1;
    }

    read_count = fread(&header, sizeof(ADCHeader), 1, file);
    if (read_count != 1) {
        printf("ERROR: could not read this file.\n");
        fclose(file);
        return 1;
    }

    printf("\nheader information:\n");
    printf("magic number: 0x%X\n", header.magic);
    printf("version: %u\n", header.version);
    printf("channel count: %u\n", header.channel_count);
    printf("record count: %u\n", header.record_count);
    printf("sample rate: %u Hz\n", header.sample_rate_hz);

    if (header.magic != ADC_MAGIC) {
        printf("wrong magic number.\n");
        fclose(file);
        return 1;
    }

    if (header.version != ADC_VERSION) {
        printf("wrong file version\n");
        fclose(file);
        return 1;
    }

    if (header.channel_count != ADC_CHANNELS) {
        printf("wrong number of channels\n");
        fclose(file);
        return 1;
    }

    if (header.sample_rate_hz != ADC_SAMPLE_RATE) {
        printf("wrong sample rate\n");
        fclose(file);
        return 1;
    }

    printf("\nheader is valid\n");
    samples = malloc(header.record_count * sizeof(ADCSample));

    if (samples == NULL) {
        printf("memory allocation failed\n");
        fclose(file);
        return 1;
    }

    read_count = fread(samples, sizeof(ADCSample), header.record_count, file);

    if (read_count != header.record_count) {
        printf("could not read all ADC samples\n");
        free(samples);
        fclose(file);
        return 1;
    }

    printf("\nsamples loaded successfully\n");
    printf("first 5 samples:\n");

    for (i = 0; i < 5; i++) {
        printf("sample %d: time = %.3f, channel = %u, raw = %u, flags = %u, sequence = %u\n",
            i,
            samples[i].timestamp,
            samples[i].channel_id,
            samples[i].raw_value,
            samples[i].status_flags,
            samples[i].sequence_number);

    }
    free(samples);
    fclose(file);

  return 0;
  }