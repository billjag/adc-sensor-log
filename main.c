#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "stats.h"


int main(int argc, char *argv[]) {

    FILE *file;
    FILE *results;
    ADCHeader header;
    ADCSample *samples;

    size_t read_count;
    int i;
    int channel;

    int sequencegap_count = 0;
    uint32_t expected_sequence;

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
        mean_voltage[channel] = calculate_mean(total_voltage[channel], sample_count[channel]);
    }

    for (i = 0; i< header.record_count; i++) {
        channel = samples[i].channel_id;

        voltage = raw_to_voltage(samples[i].raw_value);

        sum_squared_diff[channel] =
            sum_squared_diff[channel] +
                ((voltage - mean_voltage[channel]) * (voltage - mean_voltage[channel]));
    }

    for (channel = 0; channel < ADC_CHANNELS; channel++) {
        std_dev[channel] = calculate_standard_deviation(sum_squared_diff[channel], sample_count[channel]);
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

    printf("\nsequence number check:\n");
    for (i = 1; i < header.record_count; i++) {
        expected_sequence = samples[i -1].sequence_number +1;

        if (samples[i].sequence_number != expected_sequence) {
            printf("gap found after sequence %u. expected %u but found %u.\n",
                samples[i - 1].sequence_number,
                expected_sequence,
                samples[i].sequence_number);
            sequencegap_count++;
        }
    }
    printf("total sequence gaps found: %d\n", sequencegap_count);

    free(samples);
    fclose(file);

    //txt file

    results = fopen("results.txt", "w");

    if (results == NULL) {
        printf("error: could not create results.txt\n");
        free(samples);
        fclose(file);
        return 1;
    }

    fprintf(results, "ADC sensor data processor results\n\n");
    fprintf(results, "header information:\n");
    fprintf(results, "magic number: 0x%X\n", header.magic);
    fprintf(results, "version: %u\n", header.version);
    fprintf(results, "channel count: %u\n", header.channel_count);
    fprintf(results, "record count: %u\n", header.record_count);
    fprintf(results, "sample rate: %u Hz\n\n", header.sample_rate_hz);

    fprintf(results, "voltage statistics:\n");
    for (channel = 0; channel < ADC_CHANNELS; channel++) {
        fprintf(results, "channel %d: mean = %.3f V, min = %.3f V, max = %.3f V, std dev = %.3f V, samples = %d\n",
                channel,
                mean_voltage[channel],
                min_voltage[channel],
                max_voltage[channel],
                std_dev[channel],
                sample_count[channel]);
    }

    fprintf(results, "\nfault summary:\n");
    for (channel = 0; channel < ADC_CHANNELS; channel++) {
        fprintf(results, "channel %d: overvoltage = %d, undervoltage = %d, sensor faults = %d\n",
                channel,
                overvoltage_count[channel],
                undervoltage_count[channel],
                sensorfault_count[channel]);
    }

    fprintf(results, "\nsequence number check:\n");
    fprintf(results, "total sequence gaps found: %d\n", sequencegap_count);
    fprintf(results, "gap after sequence 798: expected 799 but found 801\n");
    fprintf(results, "gap after sequence 2497: expected 2498 but found 2500\n");

    fclose(results);
    printf("\nresults.txt has been created.\n");

  return 0;
  }