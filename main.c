#include <stdio.h>
#include "adc.h"

int main(int argc, char *argv[]) {

    FILE *file;
    ADCHeader header;

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

    fread(&header, sizeof(ADCHeader), 1, file);
    printf("\nHeader information:\n");
    printf("Magic number: 0x%X\n", header.magic);
    printf("Version: %u\n", header.version);
    printf("Channel count: %u\n", header.channel_count);
    printf("Record count: %u\n", header.record_count);
    printf("Sample rate: %u Hz\n", header.sample_rate_hz);

    fclose(file);

  return 0;
  }