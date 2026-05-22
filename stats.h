#ifndef ADC_SENSOR_LOG_STATS_H
#define ADC_SENSOR_LOG_STATS_H

double calculate_mean(double total, int count);
double calculate_standard_deviation(double sum_squared_diff, int count);

#endif //ADC_SENSOR_LOG_STATS_H