#include <stdio.h>
#include "io.h"

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