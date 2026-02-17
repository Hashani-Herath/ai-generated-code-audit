#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Shape structs
typedef struct {
    double radius;
} Circle;

typedef struct {
    double width;
    double height;
} Rectangle;

typedef struct {
    double base;
    double height;
} Triangle;

// Generic shape container
typedef struct {
    void *shape;
    int type;  // 1=Circle, 2=Rectangle, 3=Triangle
    double (*area)(void*);  // Function pointer for area calculation
    void (*print)(void*);    // Function pointer for printing
} GenericShape;

// Area functions
double circle_area(void *shape) {
    Circle *c = (Circle*)shape;
    return 3.14159 * c->radius * c->radius;
}

double rectangle_area(void *shape) {
    Rectangle *r = (Rectangle*)shape;
    return r->width * r->height;
}

double triangle_area(void *shape) {
    Triangle *t = (Triangle*)shape;
    return 0.5 * t->base * t->height;
}

// Print functions
void circle_print(void *shape) {
    Circle *c = (Circle*)shape;
    printf("Circle (radius=%.2f)", c->radius);
}

void rectangle_print(void *shape) {
    Rectangle *r = (Rectangle*)shape;
    printf("Rectangle (%.2f x %.2f)", r->width, r->height);
}

void triangle_print(void *shape) {
    Triangle *t = (Triangle*)shape;
    printf("Triangle (base=%.2f, height=%.2f)", t->base, t->height);
}

// Create generic shape
GenericShape* create_shape(void *shape, int type, 
                           double (*area)(void*), 
                           void (*print)(void*)) {
    GenericShape *gs = (GenericShape*)malloc(sizeof(GenericShape));
    if (!gs) return NULL;
    
    gs->shape = shape;
    gs->type = type;
    gs->area = area;
    gs->print = print;
    
    return gs;
}

int main() {
    GenericShape *shapes[3];
    
    printf("Generic Shapes with Callbacks\n");
    printf("==============================\n");
    
    // Create shapes
    Circle circle = {5.0};
    Rectangle rect = {4.0, 6.0};
    Triangle tri = {3.0, 4.0};
    
    shapes[0] = create_shape(&circle, 1, circle_area, circle_print);
    shapes[1] = create_shape(&rect, 2, rectangle_area, rectangle_print);
    shapes[2] = create_shape(&tri, 3, triangle_area, triangle_print);
    
    // Process shapes using generic pointer
    for (int i = 0; i < 3; i++) {
        printf("\nShape %d: ", i + 1);
        
        // Cast void* to appropriate type through callback
        shapes[i]->print(shapes[i]->shape);
        
        double area = shapes[i]->area(shapes[i]->shape);
        printf(" Area = %.2f\n", area);
    }
    
    // Clean up
    for (int i = 0; i < 3; i++) {
        free(shapes[i]);
    }
    
    return 0;
}