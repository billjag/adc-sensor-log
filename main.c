#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include <math.h>

int main(int argc, char *argv[]) {

    FILE *file;
    ADCHeader header;
    ADCSample *samples;
    size_t read_count;
    int i;
    int channel;

    double voltage;
    double total_voltage[ADC_CHANNELS] = {0};
    double min_voltage[ADC_CHANNELS] = {0};
    double max_voltage[ADC_CHANNELS] = {0};
    int sample_count[ADC_CHANNELS] = {0};
    double mean_voltage[ADC_CHANNELS] = {0};
    double sum_squared_diff[ADC_CHANNELS] = {0};
    double std_dev[ADC_CHANNELS] = {0};
    int overvoltage_count[ADC_CHANNELS] = {0};
    int undervoltage_count[ADC_CHANNELS] = {0};
    int sensorfault_count[ADC_CHANNELS] = {0};



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
        voltage = (samples[i].raw_value / ADC_MAX_RAW) * ADC_VREF;
        printf("sample %d: time = %.3f, channel = %u, raw = %u, voltage = %.3f V, flags = %u, sequence = %u\n",
            i,
            samples[i].timestamp,
            samples[i].channel_id,
            samples[i].raw_value,
            voltage,
            samples[i].status_flags,
            samples[i].sequence_number);

    }
    for (i = 0; i < header.record_count; i++) {
        channel = samples[i].channel_id;

        voltage = (samples[i].raw_value / ADC_MAX_RAW) * ADC_VREF;

        total_voltage[channel] = total_voltage[channel] + voltage;
        sample_count[channel]++;

        if (sample_count[channel] == 1) {
            min_voltage[channel] = voltage;
            max_voltage[channel] = voltage;
        }
        if (voltage < min_voltage[channel]) {
            min_voltage[channel] = voltage;
        }
        if (voltage > max_voltage[channel]) {
            max_voltage[channel] = voltage;
        }

        if (voltage > 3.0) {
            overvoltage_count[channel]++;
        }
        if (voltage < 0.3) {
            undervoltage_count[channel]++;
        }
        if (samples[i].status_flags & 0x01) {
            sensorfault_count[channel]++;
        }

    }


    for (channel = 0; channel < ADC_CHANNELS; channel++) {
        mean_voltage[channel] = total_voltage[channel] / sample_count[channel];
    }

    for (i = 0; i< header.record_count; i++) {
        channel = samples[i].channel_id;

        voltage = (samples[i].raw_value / ADC_MAX_RAW) * ADC_VREF;

        sum_squared_diff[channel] =
            sum_squared_diff[channel] +
                ((voltage - mean_voltage[channel]) * (voltage - mean_voltage[channel]));
    }

    for (channel = 0; channel < ADC_CHANNELS; channel++) {
        std_dev[channel] = sqrt(sum_squared_diff[channel] / sample_count[channel]);
    }


    printf("\nbasic voltage statistic:\n");
    for (channel = 0; channel < ADC_CHANNELS; channel++) {
        printf("channel %d: mean = %.3f V, min = %.3f V, max = %.3f V, samples = %d\n",
            channel,
            total_voltage[channel] / sample_count[channel],
            min_voltage[channel],
            max_voltage[channel],
            sample_count[channel]);
    }
    printf("\nfault summary:\n");
    for (channel = 0; channel < ADC_CHANNELS; channel++) {
        printf("channel %d: overvoltage = %d, undervoltage = %d, sensor faults = %d\n",
            channel,
            overvoltage_count[channel],
            undervoltage_count[channel],
            sensorfault_count[channel]);
    }

    free(samples);
    fclose(file);

  return 0;
  }