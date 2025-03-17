#include <stdio.h>

#define dX 0.0000001

double Function(double x);

double diff(double x) {
    if (x + dX <= 1)
        return (Function(x + dX) - Function(x)) / dX;
    return (Function(x) - Function(x - dX)) / dX;
}

int main() {
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("output.txt", "w");

    int numQuantity;

    if (fscanf(in, "%d", &numQuantity) != 1) {}

    for (int i = 0; i < numQuantity; i++) {
        double x;
        if (fscanf(in, "%lf", &x) != 1) {}

        fprintf(out, "%0.20lf\n", diff(x));
    }

    return 0;
}