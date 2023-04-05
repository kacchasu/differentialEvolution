#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_DIMENSIONS 2

float my_objective_function(float* x) {
    return pow(x[0], 2) + pow(x[1], 2);
}

void rand_1_bin(float** population, int n, int D, float F, float CR, float** v) {
    for (int i = 0; i < n; i++) {
        int idxs[3];
        for (int k = 0; k < 3; k++) {
            int index;
            do {
                index = rand() % n;
            } while (index == i || index == idxs[0] || index == idxs[1]);
            idxs[k] = index;
        }

        float* a = population[idxs[0]];
        float* b = population[idxs[1]];
        float* c = population[idxs[2]];

        float* mutant = (float*)malloc(D * sizeof(float));
        for (int j = 0; j < D; j++) {
            mutant[j] = a[j] + F * (b[j] - c[j]);
        }

        int jrand = rand() % D;
        float* trial = (float*)malloc(D * sizeof(float));
        for (int j = 0; j < D; j++) {
            if ((float)rand() / RAND_MAX < CR || j == jrand) {
                trial[j] = mutant[j];
            }
            else {
                trial[j] = population[i][j];
            }
        }

        for (int j = 0; j < D; j++) {
            v[i][j] = trial[j];
        }
    }
}

float* de_optimizer(float (*objective_function)(float*), float bounds[MAX_DIMENSIONS][2], int n_dims, int pop_size, int max_iter, float F, float CR) {
    float** population = (float**)malloc(pop_size * sizeof(float*));
    for (int i = 0; i < pop_size; i++) {
        population[i] = (float*)malloc(n_dims * sizeof(float));
        for (int j = 0; j < n_dims; j++) {
            population[i][j] = bounds[j][0] + (float)rand() / RAND_MAX * (bounds[j][1] - bounds[j][0]);
        }
    }

    float* scores = (float*)malloc(pop_size * sizeof(float));
    for (int i = 0; i < pop_size; i++) {
        scores[i] = objective_function(population[i]);
    }

    int best_idx = 0;
    for (int i = 1; i < pop_size; i++) {
        if (scores[i] < scores[best_idx]) {
            best_idx = i;
        }
    }
    float* best = population[best_idx];

    float** v = (float**)malloc(pop_size * sizeof(float*));
    for (int i = 0; i < pop_size; i++) {
        v[i] = (float*)malloc(n_dims * sizeof(float));
    }

    for (int i = 0; i < max_iter; i++) {
        rand_1_bin(population, pop_size, n_dims, F, CR, v);

        float* new_scores = (float*)malloc(pop_size * sizeof(float));
        for (int j = 0; j < pop_size; j++) {
            new_scores[j] = objective_function(v[j]);
        }

        for (int j = 0; j < pop_size; j++) {
            if (new_scores[j] < scores[j]) {
                for (int k = 0; k < n_dims; k++) {
                    population[j][k] = v[j][k];
                }
                scores[j] = new_scores[j];
            }
        }

        int new_best_idx = 0;
        for (int j = 1; j < pop_size; j++) {
            if (scores[j] < scores[new_best_idx]) {
                new_best_idx = j;
            }
        }
        if (scores[new_best_idx] < scores[best_idx]) {
            best_idx = new_best_idx;
            best = population[best_idx];
        }
    }

    float* result = (float*)malloc((n_dims + 1) * sizeof(float));
    for (int i = 0; i < n_dims; i++) {
        result[i] = best[i];
    }
    result[n_dims] = scores[best_idx];

    for (int i = 0; i < pop_size; i++) {
        free(population[i]);
        free(v[i]);
    }
    free(population);
    free(v);

    return result;
}

int main() {
    srand(time(NULL));

    float bounds[MAX_DIMENSIONS][2] = {{-5.0f, 5.0f}, {-5.0f, 5.0f}};

    float* result = de_optimizer(my_objective_function, bounds, 2, 50, 1000, 0.8, 0.9);

    printf("Best solution: (%f, %f)\n", result[0], result[1]);
    printf("Best score: %f\n", result[2]);

    free(result);

    return 0;
}
