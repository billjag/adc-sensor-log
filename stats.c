#include <math.h>
#include "stats.h"

double calculate_mean(double total, int count) {
    double mean;
    mean = total / count;
    return mean;
}

double calculate_standard_deviation(double sum_squared_diff, int count) {
    double standard_deviation;
    standard_deviation = sqrt(sum_squared_diff / count);
    return standard_deviation;
}