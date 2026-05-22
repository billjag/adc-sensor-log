#ifndef IO_H
#define IO_H
#include "adc.h"

int read_adc_file(const char *filename, ADCHeader *header, ADCSample **samples);

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
    );

#endif //ADC_SENSOR_LOG_IO_H