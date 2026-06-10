#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W 60
#define H 25
#define MAX_SHAPES 100

// Define Shape Types
typedef enum {
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

// Define specific shape data structures
typedef struct {
    int x1, y1;
    int x2, y2;
} LineData;

typedef struct {
    int x, y; // Top-left corner
    int w, h; // Width and height
} RectData;

typedef struct {
    int cx, cy; // Center
    int r;      // Radius
} CircleData;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriData;

// Shape wrapper
typedef struct {
    int id;
    ShapeType type;
    union {
        LineData line;
        RectData rect;
        CircleData circle;
        TriData tri;
    } data;
    int active; // 1 if active, 0 if deleted
} Shape;

// Global array of shapes
Shape shapes[MAX_SHAPES];
int next_shape_id = 1;

// Function declarations
void clear_canvas(char canvas[H][W]);
void render_canvas(char canvas[H][W]);
void display_canvas(char canvas[H][W]);
void draw_line(char canvas[H][W], int x1, int y1, int x2, int y2);
void draw_rectangle(char canvas[H][W], int x, int y, int w, int h);
void draw_circle(char canvas[H][W], int cx, int cy, int r);
void draw_triangle(char canvas[H][W], int x1, int y1, int x2, int y2, int x3, int y3);

int read_int(const char *prompt, int min_val, int max_val);
void add_shape_menu();
void delete_shape_menu();
void modify_shape_menu();
void list_shapes();
const char* shape_type_to_string(ShapeType type);

int main() {
    // Initialize shape array
    for (int i = 0; i < MAX_SHAPES; i++) {
        shapes[i].active = 0;
    }

    char canvas[H][W];

    printf("=========================================\n");
    printf("     Welcome to 2D Graphics Editor!      \n");
    printf("=========================================\n");
    printf("Canvas size: %d x %d\n", W, H);
    printf("Draw shapes using '*' and background '_'\n\n");

    // Initial render
    render_canvas(canvas);
    display_canvas(canvas);

    while (1) {
        printf("\n--- Main Menu ---\n");
        printf("1. Display Canvas\n");
        printf("2. Add a Shape (Line, Rect, Circle, Tri)\n");
        printf("3. Delete a Shape\n");
        printf("4. Modify a Shape\n");
        printf("5. List All Shapes\n");
        printf("6. Clear All Shapes\n");
        printf("7. Exit\n");
        printf("-----------------\n");

        int choice = read_int("Enter your choice (1-7): ", 1, 7);

        switch (choice) {
            case 1:
                render_canvas(canvas);
                display_canvas(canvas);
                break;
            case 2:
                add_shape_menu();
                render_canvas(canvas);
                display_canvas(canvas);
                break;
            case 3:
                delete_shape_menu();
                render_canvas(canvas);
                display_canvas(canvas);
                break;
            case 4:
                modify_shape_menu();
                render_canvas(canvas);
                display_canvas(canvas);
                break;
            case 5:
                list_shapes();
                break;
            case 6:
                for (int i = 0; i < MAX_SHAPES; i++) {
                    shapes[i].active = 0;
                }
                printf("All shapes cleared.\n");
                render_canvas(canvas);
                display_canvas(canvas);
                break;
            case 7:
                printf("Exiting 2D Graphics Editor. Goodbye!\n");
                return 0;
            default:
                break;
        }
    }

    return 0;
}

// Robust integer input scanner to prevent loops on invalid entries
int read_int(const char *prompt, int min_val, int max_val) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        
        char *endptr;
        long parsed = strtol(buffer, &endptr, 10);
        if (endptr == buffer || *endptr != '\0') {
            printf("Invalid input. Please enter a valid integer.\n");
            continue;
        }
        if (parsed < min_val || parsed > max_val) {
            printf("Value out of range [%d, %d]. Please try again.\n", min_val, max_val);
            continue;
        }
        val = (int)parsed;
        break;
    }
    return val;
}

const char* shape_type_to_string(ShapeType type) {
    switch (type) {
        case SHAPE_LINE: return "LINE";
        case SHAPE_RECTANGLE: return "RECTANGLE";
        case SHAPE_CIRCLE: return "CIRCLE";
        case SHAPE_TRIANGLE: return "TRIANGLE";
        default: return "UNKNOWN";
    }
}

// Display the shape details
void print_shape_details(Shape *s) {
    printf("[ID: %d] %s - ", s->id, shape_type_to_string(s->type));
    switch (s->type) {
        case SHAPE_LINE:
            printf("Start: (%d, %d), End: (%d, %d)\n",
                   s->data.line.x1, s->data.line.y1,
                   s->data.line.x2, s->data.line.y2);
            break;
        case SHAPE_RECTANGLE:
            printf("Top-left: (%d, %d), Width: %d, Height: %d\n",
                   s->data.rect.x, s->data.rect.y,
                   s->data.rect.w, s->data.rect.h);
            break;
        case SHAPE_CIRCLE:
            printf("Center: (%d, %d), Radius: %d\n",
                   s->data.circle.cx, s->data.circle.cy,
                   s->data.circle.r);
            break;
        case SHAPE_TRIANGLE:
            printf("V1: (%d, %d), V2: (%d, %d), V3: (%d, %d)\n",
                   s->data.tri.x1, s->data.tri.y1,
                   s->data.tri.x2, s->data.tri.y2,
                   s->data.tri.x3, s->data.tri.y3);
            break;
    }
}

// List all shapes
void list_shapes() {
    int count = 0;
    printf("\n--- Active Shapes ---\n");
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active) {
            print_shape_details(&shapes[i]);
            count++;
        }
    }
    if (count == 0) {
        printf("No active shapes on the canvas.\n");
    }
    printf("---------------------\n");
}

// Add shape flow
void add_shape_menu() {
    // Find an empty slot
    int slot = -1;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (!shapes[i].active) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        printf("Error: Maximum shape limit (%d) reached. Delete some shapes first.\n", MAX_SHAPES);
        return;
    }

    printf("\nChoose shape type to add:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    int type_choice = read_int("Enter choice (1-4): ", 1, 4);

    shapes[slot].id = next_shape_id++;
    shapes[slot].active = 1;

    switch (type_choice) {
        case 1:
            shapes[slot].type = SHAPE_LINE;
            shapes[slot].data.line.x1 = read_int("Start X (0-59): ", 0, W - 1);
            shapes[slot].data.line.y1 = read_int("Start Y (0-24): ", 0, H - 1);
            shapes[slot].data.line.x2 = read_int("End X (0-59): ", 0, W - 1);
            shapes[slot].data.line.y2 = read_int("End Y (0-24): ", 0, H - 1);
            printf("Line added successfully with ID %d.\n", shapes[slot].id);
            break;
        case 2:
            shapes[slot].type = SHAPE_RECTANGLE;
            shapes[slot].data.rect.x = read_int("Top-left X (0-59): ", 0, W - 1);
            shapes[slot].data.rect.y = read_int("Top-left Y (0-24): ", 0, H - 1);
            shapes[slot].data.rect.w = read_int("Width (1-60): ", 1, W);
            shapes[slot].data.rect.h = read_int("Height (1-25): ", 1, H);
            printf("Rectangle added successfully with ID %d.\n", shapes[slot].id);
            break;
        case 3:
            shapes[slot].type = SHAPE_CIRCLE;
            shapes[slot].data.circle.cx = read_int("Center X (0-59): ", 0, W - 1);
            shapes[slot].data.circle.cy = read_int("Center Y (0-24): ", 0, H - 1);
            shapes[slot].data.circle.r  = read_int("Radius (0-60): ", 0, W);
            printf("Circle added successfully with ID %d.\n", shapes[slot].id);
            break;
        case 4:
            shapes[slot].type = SHAPE_TRIANGLE;
            shapes[slot].data.tri.x1 = read_int("Vertex 1 X (0-59): ", 0, W - 1);
            shapes[slot].data.tri.y1 = read_int("Vertex 1 Y (0-24): ", 0, H - 1);
            shapes[slot].data.tri.x2 = read_int("Vertex 2 X (0-59): ", 0, W - 1);
            shapes[slot].data.tri.y2 = read_int("Vertex 2 Y (0-24): ", 0, H - 1);
            shapes[slot].data.tri.x3 = read_int("Vertex 3 X (0-59): ", 0, W - 1);
            shapes[slot].data.tri.y3 = read_int("Vertex 3 Y (0-24): ", 0, H - 1);
            printf("Triangle added successfully with ID %d.\n", shapes[slot].id);
            break;
    }
}

// Delete shape flow
void delete_shape_menu() {
    list_shapes();
    int has_shapes = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active) {
            has_shapes = 1;
            break;
        }
    }
    if (!has_shapes) return;

    int id_to_delete = read_int("Enter the ID of the shape to delete: ", 1, next_shape_id - 1);
    int found = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active && shapes[i].id == id_to_delete) {
            shapes[i].active = 0;
            printf("Shape ID %d deleted successfully.\n", id_to_delete);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Shape with ID %d not found.\n", id_to_delete);
    }
}

// Modify shape flow
void modify_shape_menu() {
    list_shapes();
    int has_shapes = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active) {
            has_shapes = 1;
            break;
        }
    }
    if (!has_shapes) return;

    int id_to_modify = read_int("Enter the ID of the shape to modify: ", 1, next_shape_id - 1);
    int found_index = -1;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active && shapes[i].id == id_to_modify) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        printf("Shape with ID %d not found.\n", id_to_modify);
        return;
    }

    Shape *s = &shapes[found_index];
    printf("\nModifying shape:\n");
    print_shape_details(s);

    switch (s->type) {
        case SHAPE_LINE:
            s->data.line.x1 = read_int("New Start X (0-59): ", 0, W - 1);
            s->data.line.y1 = read_int("New Start Y (0-24): ", 0, H - 1);
            s->data.line.x2 = read_int("New End X (0-59): ", 0, W - 1);
            s->data.line.y2 = read_int("New End Y (0-24): ", 0, H - 1);
            break;
        case SHAPE_RECTANGLE:
            s->data.rect.x = read_int("New Top-left X (0-59): ", 0, W - 1);
            s->data.rect.y = read_int("New Top-left Y (0-24): ", 0, H - 1);
            s->data.rect.w = read_int("New Width (1-60): ", 1, W);
            s->data.rect.h = read_int("New Height (1-25): ", 1, H);
            break;
        case SHAPE_CIRCLE:
            s->data.circle.cx = read_int("New Center X (0-59): ", 0, W - 1);
            s->data.circle.cy = read_int("New Center Y (0-24): ", 0, H - 1);
            s->data.circle.r  = read_int("New Radius (0-60): ", 0, W);
            break;
        case SHAPE_TRIANGLE:
            s->data.tri.x1 = read_int("New Vertex 1 X (0-59): ", 0, W - 1);
            s->data.tri.y1 = read_int("New Vertex 1 Y (0-24): ", 0, H - 1);
            s->data.tri.x2 = read_int("New Vertex 2 X (0-59): ", 0, W - 1);
            s->data.tri.y2 = read_int("New Vertex 2 Y (0-24): ", 0, H - 1);
            s->data.tri.x3 = read_int("New Vertex 3 X (0-59): ", 0, W - 1);
            s->data.tri.y3 = read_int("New Vertex 3 Y (0-24): ", 0, H - 1);
            break;
    }

    printf("Shape ID %d modified successfully.\n", id_to_modify);
}

// Canvas Initialization
void clear_canvas(char canvas[H][W]) {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Render the active shape list onto canvas
void render_canvas(char canvas[H][W]) {
    clear_canvas(canvas);
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active) {
            ShapeType t = shapes[i].type;
            if (t == SHAPE_LINE) {
                draw_line(canvas, shapes[i].data.line.x1, shapes[i].data.line.y1,
                          shapes[i].data.line.x2, shapes[i].data.line.y2);
            } else if (t == SHAPE_RECTANGLE) {
                draw_rectangle(canvas, shapes[i].data.rect.x, shapes[i].data.rect.y,
                               shapes[i].data.rect.w, shapes[i].data.rect.h);
            } else if (t == SHAPE_CIRCLE) {
                draw_circle(canvas, shapes[i].data.circle.cx, shapes[i].data.circle.cy,
                            shapes[i].data.circle.r);
            } else if (t == SHAPE_TRIANGLE) {
                draw_triangle(canvas, shapes[i].data.tri.x1, shapes[i].data.tri.y1,
                              shapes[i].data.tri.x2, shapes[i].data.tri.y2,
                              shapes[i].data.tri.x3, shapes[i].data.tri.y3);
            }
        }
    }
}

// Render canvas to stdout with row/column indices and borders
void display_canvas(char canvas[H][W]) {
    // Print column coordinate headers
    printf("   ");
    for (int x = 0; x < W; x++) {
        if (x % 10 == 0) {
            printf("%d", x / 10);
        } else {
            printf(" ");
        }
    }
    printf("\n   ");
    for (int x = 0; x < W; x++) {
        printf("%d", x % 10);
    }
    printf("\n");

    // Top border
    printf("  +");
    for (int x = 0; x < W; x++) {
        printf("-");
    }
    printf("+\n");

    // Grid rows
    for (int y = 0; y < H; y++) {
        printf("%2d|", y);
        for (int x = 0; x < W; x++) {
            printf("%c", canvas[y][x]);
        }
        printf("|\n");
    }

    // Bottom border
    printf("  +");
    for (int x = 0; x < W; x++) {
        printf("-");
    }
    printf("+\n");
}

// Bresenham's Line Algorithm
void draw_line(char canvas[H][W], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        if (x1 >= 0 && x1 < W && y1 >= 0 && y1 < H) {
            canvas[y1][x1] = '*';
        }
        if (x1 == x2 && y1 == y2) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Outline Rectangle using draw_line boundaries
void draw_rectangle(char canvas[H][W], int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    draw_line(canvas, x, y, x + w - 1, y);
    draw_line(canvas, x, y + h - 1, x + w - 1, y + h - 1);
    draw_line(canvas, x, y, x, y + h - 1);
    draw_line(canvas, x + w - 1, y, x + w - 1, y + h - 1);
}

// Bresenham's Midpoint Circle Algorithm
void draw_circle(char canvas[H][W], int cx, int cy, int r) {
    if (r < 0) return;
    
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    // Helper macro to safely plot within grid bounds
    #define PLOT(px, py) do { \
        if ((px) >= 0 && (px) < W && (py) >= 0 && (py) < H) { \
            canvas[py][px] = '*'; \
        } \
    } while(0)

    while (y >= x) {
        PLOT(cx + x, cy + y);
        PLOT(cx - x, cy + y);
        PLOT(cx + x, cy - y);
        PLOT(cx - x, cy - y);
        PLOT(cx + y, cy + x);
        PLOT(cx - y, cy + x);
        PLOT(cx + y, cy - x);
        PLOT(cx - y, cy - x);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
    #undef PLOT
}

// Draw triangle by drawing lines between vertices
void draw_triangle(char canvas[H][W], int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(canvas, x1, y1, x2, y2);
    draw_line(canvas, x2, y2, x3, y3);
    draw_line(canvas, x3, y3, x1, y1);
}
