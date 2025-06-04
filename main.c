//***************************************
//compilacion 
//gcc scanner_to_stl.c -o scanner_to_stl -lm
//conexion con ESP32 en linux
//cat /dev/ttyUSB0 | ./scanner_to_stl
//***************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_POINTS 10000
#define THETA_STEPS 200

typedef struct {
    float x, y, z;
} Point;

Point points[MAX_POINTS];
int total_points = 0;

void write_ascii_stl(const char* filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Error opening STL file");
        exit(1);
    }

    fprintf(fp, "solid scan\n");

    int layers = total_points / THETA_STEPS;

    for (int i = 0; i < layers - 1; i++) {
        for (int j = 0; j < THETA_STEPS; j++) {
            int next_j = (j + 1) % THETA_STEPS;

            Point p1 = points[i * THETA_STEPS + j];
            Point p2 = points[(i + 1) * THETA_STEPS + j];
            Point p3 = points[(i + 1) * THETA_STEPS + next_j];
            Point p4 = points[i * THETA_STEPS + next_j];

            // Triángulo 1
            fprintf(fp, "  facet normal 0 0 0\n");
            fprintf(fp, "    outer loop\n");
            fprintf(fp, "      vertex %f %f %f\n", p1.x, p1.y, p1.z);
            fprintf(fp, "      vertex %f %f %f\n", p2.x, p2.y, p2.z);
            fprintf(fp, "      vertex %f %f %f\n", p3.x, p3.y, p3.z);
            fprintf(fp, "    endloop\n");
            fprintf(fp, "  endfacet\n");

            // Triángulo 2
            fprintf(fp, "  facet normal 0 0 0\n");
            fprintf(fp, "    outer loop\n");
            fprintf(fp, "      vertex %f %f %f\n", p1.x, p1.y, p1.z);
            fprintf(fp, "      vertex %f %f %f\n", p3.x, p3.y, p3.z);
            fprintf(fp, "      vertex %f %f %f\n", p4.x, p4.y, p4.z);
            fprintf(fp, "    endloop\n");
            fprintf(fp, "  endfacet\n");
        }
    }

    fprintf(fp, "endsolid scan\n");
    fclose(fp);
}

int main() {
    char line[128];

    while (fgets(line, sizeof(line), stdin)) {
        Point p;
        if (sscanf(line, "%f,%f,%f", &p.x, &p.y, &p.z) == 3) {
            if (total_points >= MAX_POINTS) {
                fprintf(stderr, "Error: too many points\n");
                return 1;
            }
            points[total_points++] = p;
        }
    }

    printf("Total points: %d\n", total_points);
    write_ascii_stl("scan.stl");

    return 0;
}
