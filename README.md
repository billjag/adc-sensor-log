ADC sensor data processor

This program reads a binary ADC sensor log file that processes the data and writes the analysis results to 'results.txt'

What the program does:
- reads and validates the ADC file header
- checks the magic number, version, channel count and sample rate
- uses malloc to allocate memory for the ADC samples
- reads all ADC records using fread
- converts raw ADC values into voltages
- calculates mean, minimum, maximum and standard deviation for each channel
- detects overvoltage, undervoltage and sensor faults
- checks for sequence number gaps
- writes the final results to 'results.txt'

Project Files:
- 'main.c' controls the main program flow
- 'adc.c' and 'adc.h' contain adc related code, including raw value to voltage conversion
- 'stats.c' and 'stats.h' contain statistics functions
- 'io.c' and 'io.h' contain file reading and file writing functions
- 'CMakeLists.txt' used by CLion to build the project
- 'adc_sensor_log.bin' is the binary input file
- 'results.txt' is the output file created by the program

How to build and run in CLion:
- open the project folder in CLion
- make sure the project is using C99
- make sure 'adc_sensor_log.bin' is in the main project folder
- go to run then edit configuration
- set the program argument to 'adc_sensor_log.bin'
- set the working dictionary to the main project folder 
- click apply and okay 

GitHub link:
https://github.com/billjag/adc-sensor-log
