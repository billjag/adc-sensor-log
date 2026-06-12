#include <stdio.h>
#include "adc.h"

int main() {
    printf("ADC Sensor Data Processor\n");
    printf("project started successfully\n");

    printf("size of ADCHeader: %zu bytes\n", sizeof(ADCHeader));
    printf("size of ADCSamples: %zu bytes\n", sizeof(ADCSample));

  return 0;
  }