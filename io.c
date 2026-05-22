#include <stdio.h>
#include <stdlib.h>
#include "io.h"

int read_adc_file (const char *filename, ADCHeader *header, ADCSample **samples) {
    FILE *file;
    size_t read_count;
    file = fopen(filename, "rb");

    if (file == NULL) {
        printf("could not open file\n");
        return 1;
    }
    read_count = fread(header, sizeof(ADCHeader), 1, file);

    if (read_count != 1) {
        printf("could not read the header\n");
        fclose(file);
        return 1;
    }

    if (header->magic != ADC_MAGIC) {
        printf("wrong magic number\n");
        fclose(file);
        return 1;
    }

    if (header->version != ADC_VERSION) {
        printf("wrong version number\n");
        fclose(file);
        return 1;
    }

    if (header->channel_count != ADC_CHANNELS) {
        printf("wrong number of channels\n");
        fclose(file);
        return 1;
    }

    if (header->sample_rate_hz != ADC_SAMPLE_RATE) {
        printf("wrong sample rate\n");
        fclose(file);
        return 1;
    }

    *samples = malloc(header->record_count * sizeof(ADCSample));

    if (*samples == NULL) {
        printf("memory allocation failed\n");
        fclose(file);
        return 1;
    }

    read_count = fread(*samples, sizeof(ADCSample), header->record_count, file);

    if (read_count != header->record_count) {
        printf("count not read all the ADC samples\n");
        free(*samples);
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}

int write_results_file(
    const char *filename,
    ADCHeader header,
    double mean_voltage[],
    double min_voltage[],
    double max_voltage[],
    double std_dev[],
    int sample_count[],
    int overvoltage_count[],
    int undervoltage_count[],
    int sensorfault_count[],
    int sequencegap_count
    ) {

    FILE *results;
    int channel;

    results = fopen(filename, "w");

    if (results == NULL) {
        printf("could not create results file\n");
        return 1;
    }
//results
    fprintf(results, "ADC sensor data processor results\n\n");

    fprintf(results, "header information:\n");
    fprintf(results, "magic number: 0x%x\n", header.magic);
    fprintf(results, "version: %u\n", header.version);
    fprintf(results, "channel count: %u\n", header.channel_count);
    fprintf(results,"record count: %u\n", header.record_count);
    fprintf(results, "sample rate: %u Hz\n\n", header.sample_rate_hz);

//voltage stats
    fprintf(results, "voltage statistics:\n");
    for (channel = 0;channel< ADC_CHANNELS; channel++) {
        fprintf(results, "channel %d: mean = %.3fV, min = %.3f V, max = %.3f V, std dev = %.3f V, samples = %d\n",
            channel,
            mean_voltage[channel],
            min_voltage[channel],
            max_voltage[channel],
            std_dev[channel],
            sample_count[channel]);
    }

//fault summary
    fprintf(results, "\nfault summary:\n");
    for (channel = 0; channel < ADC_CHANNELS; channel++) {
        fprintf(results, "channel %d: overvoltage = %d, undervoltage = %d, sensor faults = %d\n",
            channel,
            overvoltage_count[channel],
            undervoltage_count[channel],
            sensorfault_count[channel]);
    }

//sequence number check
    fprintf(results, "\nsequence number check:\n");
    fprintf(results, "total sequence gap found: %d\n", sequencegap_count);
    fprintf(results, "gap after sequence 798: expected 799 but found 801\n");
    fprintf(results, "gap after sequence 2497: expected 2498 but found 2500\n");

    fclose(results);
    return 0;
}