#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>  // For boolean data type (bool, true, false)




FILE* fptr;

const int Y_AXIS[] = {1, 0, 1};
const int X_AXIS[] = {0, 1, 1};
const int Z_AXIS[] = {1, 1, 0};

void generate_triangles_from_square(int points[4][3]){
    int op = 1; //opposite idx
    int left = 3;
    int right = 2;
    for(int i = 1; i < 4; i++){
        // printf("%d %d\r\n", (abs(points[0][0] - points[i][0]) + abs(points[0][1] - points[i][1]) + abs(points[0][2] - points[i][2])), (abs(points[0][0] - points[op][0]) + abs(points[0][1] - points[op][1]) + abs(points[0][2] - points[op][2])));
        if((abs(points[0][0] - points[i][0]) + abs(points[0][1] - points[i][1]) + abs(points[0][2] - points[i][2])) >
        (abs(points[0][0] - points[op][0]) + abs(points[0][1] - points[op][1]) + abs(points[0][2] - points[op][2]))){
            op = i;
            left =  1 + ((i - 1 - 1) % 3);
            right = 1 + ((i - 1 + 1) % 3);
        }
    }

    // printf("Close triange: %d %d %d\r\n", points[0][0], points[0][1], points[0][2]);
    // printf("Far triange: %d %d %d\r\n", points[op][0], points[op][1], points[op][2]);
    // printf("op: %d Left: %d Right:%d\r\n", op, left, right);
    fprintf(fptr,"     facet normal 0 0 0\r\n         outer loop\r\n");
    fprintf(fptr,"             vertex %d %d %d\r\n",points[0][0], points[0][1], points[0][2]);
    fprintf(fptr,"             vertex %d %d %d\r\n",points[left][0], points[left][1], points[left][2]);
    fprintf(fptr,"             vertex %d %d %d\r\n",points[right][0], points[right][1], points[right][2]);
    fprintf(fptr,"         endloop\r\n     endfacet\r\n");

    fprintf(fptr,"     facet normal 0 0 0\r\n         outer loop\r\n");
    fprintf(fptr,"             vertex %d %d %d\r\n",points[op][0], points[op][1], points[op][2]);
    fprintf(fptr,"             vertex %d %d %d\r\n",points[left][0], points[left][1], points[left][2]);
    fprintf(fptr,"             vertex %d %d %d\r\n",points[right][0], points[right][1], points[right][2]);
    fprintf(fptr,"         endloop\r\n     endfacet\r\n");

    return;
}

void generate_square(const int center[3], const int width, const int axis[3], int points[4][3], bool gen_triangle){
    int points_local[4][3] = {
        { center[0] - width / 2 * axis[0], center[1] - width / 2 * axis[1], center[2] + width / 2 * axis[2] },
        { center[0] - width / 2 * axis[0], center[1] + width / 2 * axis[1], center[2] - width / 2 * axis[2] },
        { center[0] + width / 2 * axis[0], center[1] - width / 2 * axis[1], center[2] - width / 2 * axis[2] },
        { center[0] + width / 2 * axis[0], center[1] + width / 2 * axis[1], center[2] + width / 2 * axis[2] }
    };

    memcpy(points, points_local, sizeof(points_local));

    // printf("point1: %d, %d, %d\r\n", points[0][0], points[0][1], points[0][2]);
    // printf("point2: %d, %d, %d\r\n", points[1][0], points[1][1], points[1][2]);
    // printf("point3: %d, %d, %d\r\n", points[2][0], points[2][1], points[2][2]);
    // printf("point4: %d, %d, %d\r\n", points[3][0], points[3][1], points[3][2]);


    if(gen_triangle) generate_triangles_from_square(points);
    return;
}

void generate_cube(int center[3], int width, bool gen_triangle, int points[8][3]){
    int local_points[4][3];
    int centerx[3] = {center[0]-width / 2,center[1],center[2]};
    int centery[3] = {center[0],center[1]-width / 2,center[2]};
    int centerz[3] = {center[0],center[1],center[2]-width / 2};
    int centerx2[3] = {center[0]+width / 2,center[1],center[2]};
    int centery2[3] = {center[0],center[1]+width / 2,center[2]};
    int centerz2[3] = {center[0],center[1],center[2]+width / 2};
    generate_square(centerx, width, X_AXIS, local_points, gen_triangle);
    memcpy(points, local_points, sizeof(local_points));// first four points
    if (gen_triangle) generate_square(centery, width, Y_AXIS, local_points, gen_triangle);
    if (gen_triangle) generate_square(centerz, width, Z_AXIS, local_points, gen_triangle);
    generate_square(centerx2, width, X_AXIS, local_points, gen_triangle);
    memcpy(&points[4], local_points, sizeof(local_points)); /// last 4 points
    if (gen_triangle) generate_square(centery2, width, Y_AXIS, local_points, gen_triangle);
    if (gen_triangle) generate_square(centerz2, width, Z_AXIS, local_points, gen_triangle);
    return;
}

#define LAST_POINTS_SIZE 100000
#define ALL_POINTS_SIZE  100000

int last_points_idx = 0;
static int last_points_arr[LAST_POINTS_SIZE][4];//last idx is iteration

int all_points_idx = 0;
static int all_points_arr[ALL_POINTS_SIZE][4];

void print_last_arr(int arr[LAST_POINTS_SIZE][4], int length){
    for(int i = 0; i < length; i++){
        printf("Last%d: %d %d %d %d\r\n", i, arr[i][0], arr[i][1], arr[i][2], arr[i][3]);
    }
}

int my_pow(int base, int power){
    if (power == 0) return 1;

    int result = base;
    for(int i = 1; i < power; i++){
        result = result * base;
    }
    return result;
}

void populate_michael_sponge(int iterations){
    all_points_arr[all_points_idx][0] = 0;
    all_points_arr[all_points_idx][1] = 0;
    all_points_arr[all_points_idx][2] = 0;
    all_points_arr[all_points_idx][3] = 0;
    all_points_idx = 1;

    last_points_arr[last_points_idx][0] = 0;
    last_points_arr[last_points_idx][1] = 0;
    last_points_arr[last_points_idx][2] = 0;
    last_points_arr[last_points_idx][3] = 0;
    last_points_idx = 1;

    for(int i = 1; i < iterations; i++){
        int local_last_idx = 0;
        int local_last_arr[LAST_POINTS_SIZE][4];
        if(last_points_idx < 0) break;
        // printf("idx %d %d\n", last_points_idx, i);
        for(int j = 0; j < last_points_idx; j++){
            // printf("%d\n", j);
            int local_cube[8][3];
            int width = 2 * 3 * (my_pow(3, iterations - i));
            int center[3] = {last_points_arr[j][0],last_points_arr[j][1],last_points_arr[j][2]};
            // printf("Making cube at %d,%d,%d with size %d\r\n", center[0], center[1], center[2], width);
            generate_cube(center, width, false, local_cube);
            for(int k = 0; k < 8; k++){
                local_last_arr[local_last_idx][0] = local_cube[k][0];
                local_last_arr[local_last_idx][1] = local_cube[k][1];
                local_last_arr[local_last_idx][2] = local_cube[k][2];
                local_last_arr[local_last_idx][3] = i;
                local_last_idx++;
            }
        }
        memcpy(last_points_arr, local_last_arr, sizeof(last_points_arr));
        last_points_idx = local_last_idx;
        memcpy(&all_points_arr[all_points_idx], local_last_arr, local_last_idx * sizeof(local_last_arr[0]));
        all_points_idx += local_last_idx;
    }

    // print_last_arr(all_points_arr, all_points_idx);
    

    int cube_points[8][3];
    for (int i = 0; i < all_points_idx; i++) {
        int center[3] = {all_points_arr[i][0], all_points_arr[i][1], all_points_arr[i][2]};
        // int center[3] = {0,0,0};
        int michael_idx = iterations - all_points_arr[i][3] - 1;
        int width = 2 * (my_pow(3, michael_idx));
        printf("Generating cube at %d,%d,%d with size %d\r\n", center[0], center[1], center[2], width);
        generate_cube(center, width, true, cube_points);
        // generate_cube(center, 6);
    }
}

int main() {
    int iterations = 2;
    printf("How many iterations?: ");
    scanf("%d", &iterations);
    printf("\r\n%d iterations\r\n", iterations);
    if(my_pow(8, iterations) > LAST_POINTS_SIZE){
        printf("Too many iterations!");
        // return 1;
    }

    fptr = fopen("cube.stl", "w");
    if (fptr == NULL) {
        printf("Error opening file!\r\n");
        return 1;
    }

    fprintf(fptr,"solid name\r\n");
    populate_michael_sponge(iterations);
    fprintf(fptr,"endsolid name\r\n");
    printf("Done!\r\n");
    return 0;
}