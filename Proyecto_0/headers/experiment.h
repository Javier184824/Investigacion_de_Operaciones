#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "../headers/structs.h"

void do_experiment(const int iterations, time_reports* times, optimal_values_reports* optimal_values, collisions_reports* collisions);

#endif