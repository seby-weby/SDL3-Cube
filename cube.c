#include <SDL3/SDL.h>
// #include <SDL3/SDL_main.h>
#include <stdio.h>
#include <math.h>

#define vnum 8
#define vsize 10
#define FPS 50
#define speed SDL_PI_D*dt/2

struct vertex {
    double x, y, z;
};

struct cube {
    struct vertex plane1[vnum/2];
    struct vertex plane2[vnum/2];
    double angle;
};

struct cube Cube;
// Functions
bool init();
bool makeWindow();
bool makeRenderer();
void screen(double* x, double* y);
void project(double* x, double* y, double vz);
struct vertex rotateVertex(struct vertex v, double angle);
void drawLine(struct vertex v1, struct vertex v2);
void drawPlane(struct vertex v[vnum/2]);
void connectPlanes(struct cube c);
void update();
void loop();
void end();

// Variables
SDL_Window* gWindow = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* gScreenSurface = NULL;
Uint32* vertexColor = NULL;
Uint32* Black = NULL;
const int kScreenWidth = 1200;
const int kScreenHeight = 1200;
const double dt = 1.0/FPS;
double dz = 1.5;
// Structs

int main(int argc, char* argv[]) {
    bool success = true;

    // Set Cube angle
    Cube.angle = 0;
    
    // Set vertex positions for the cube
    // Plane 1
    Cube.plane1[0].x = -0.4;
    Cube.plane1[0].y = 0.4;
    Cube.plane1[0].z = 0.4;

    Cube.plane1[1].x = 0.4;
    Cube.plane1[1].y = 0.4;
    Cube.plane1[1].z = 0.4;

    Cube.plane1[2].x = 0.4;
    Cube.plane1[2].y = -0.4;
    Cube.plane1[2].z = 0.4;

    Cube.plane1[3].x = -0.4;
    Cube.plane1[3].y = -0.4;
    Cube.plane1[3].z = 0.4;

    // 2nd Plane
    Cube.plane2[0].x = -0.4;
    Cube.plane2[0].y = 0.4;
    Cube.plane2[0].z = -0.4;

    Cube.plane2[1].x = 0.4;
    Cube.plane2[1].y = 0.4;
    Cube.plane2[1].z = -0.4;

    Cube.plane2[2].x = 0.4;
    Cube.plane2[2].y = -0.4;
    Cube.plane2[2].z = -0.4;

    Cube.plane2[3].x = -0.4;
    Cube.plane2[3].y = -0.4;
    Cube.plane2[3].z = -0.4;
    
    // Initialize SDL
    if(!init()){
        return -1;
    }

    // Create Window
    if(!makeWindow()){
        return -2;
    }

    // Create Renderer
    if(!makeRenderer()){
        return -4;
    }
    
    loop();
    // Cleanup
    end();

    return 0;
}

bool init() {
    bool success = SDL_Init(SDL_INIT_VIDEO);
    if(!success) {
        SDL_Log("Initialization Failed! Cause: %s\n", SDL_GetError());
    }
    return success;
}

bool makeWindow() {

    gWindow = SDL_CreateWindow("Cube", kScreenWidth, kScreenHeight, 0);
    if(gWindow == NULL) {
        SDL_Log("Window creation failed! Cause: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

bool makeRenderer() {
    renderer = SDL_CreateRenderer(gWindow, NULL);
    if(renderer == NULL) {
        SDL_Log("Renderer creation failed! Cause: %s\n", SDL_GetError());
        return false;
    }
    return true;
}


void screen(double* x, double* y){ 
    *x = ((*x+1)/2)*kScreenWidth;
    *y = (1-(*y+1)/2)*kScreenHeight;
}

void project(double* x, double* y, double z){
    *x = *x/z; 
    *y = *y/z;
}

struct vertex rotateVertex(struct vertex v, double angle) {
    const double c = cos(angle);
    const double s = sin(angle);
    const double x = v.x;
    const double y = v.y;
    const double z = v.z;
    v.x = x*c-y*s;
    v.y = x*s+y*c;

    v.z = v.x*s+z*c;
    v.x = v.x*c-z*s;

    return v;
}

void drawVertex(struct vertex v) {
        SDL_FRect r = {
        w: vsize,
        h: vsize
        };
        double x, y, z;    
        x = v.x;
        y = v.y;
        z = v.z+dz;
        project(&x, &y, z);
        screen(&x, &y);
        x -= (vsize/2);
        y -= (vsize/2);
        r.x = x;
        r.y = y;
        SDL_SetRenderDrawColor(renderer, 8, 146, 208, 0);
        SDL_RenderRect(renderer, &r);
}

void drawLine(struct vertex v1, struct vertex v2) {
    double x1, y1, z1;    
    x1 = v1.x;
    y1 = v1.y;
    z1 = v1.z+dz;
    project(&x1, &y1, z1);
    screen(&x1, &y1);

    double x2, y2, z2;    
    x2 = v2.x;
    y2 = v2.y;
    z2 = v2.z+dz;
    project(&x2, &y2, z2);
    screen(&x2, &y2);

    SDL_SetRenderDrawColor(renderer, 8, 146, 208, 0);
    SDL_RenderLine(renderer, x1, y1, x2, y2);
}

void drawPlane(struct vertex v[vnum/2]) {
    for(int i = 0; i < vnum/2; ++i) {
        struct vertex v1 = rotateVertex(v[i], Cube.angle);
        struct vertex v2 = rotateVertex(v[0], Cube.angle);
        if(i != (vnum/2-1)){
            v2 = rotateVertex(v[i+1], Cube.angle);
        }
        drawLine(v1, v2);
        // drawVertex(v1);
    }
}

void connectPlanes(struct cube c) {
    for(int i = 0; i < vnum/2; ++i) {
        struct vertex v1 = rotateVertex(c.plane1[i], Cube.angle);
        struct vertex v2 = rotateVertex(c.plane2[i], Cube.angle);
        drawLine(v1, v2);
    }
}

void update() {
    Cube.angle += speed;
    drawPlane(Cube.plane1);
    drawPlane(Cube.plane2);
    connectPlanes(Cube);
}

void loop() {
    bool quit = false;
    SDL_Event e;
    SDL_zero(e);

    // Colors
    Uint32 b = SDL_MapSurfaceRGB(gScreenSurface, 0, 0, 0);
    Uint32 vc = SDL_MapSurfaceRGB(gScreenSurface, 8, 146, 208);
    Black = &b;
    vertexColor = &vc;
    while(!quit) {
        // Events
        while(SDL_PollEvent( &e )) {
            if(e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
        SDL_RenderClear(renderer);
        // Updates
        update();
        // Renderer
        SDL_RenderPresent(renderer);

        SDL_Delay(1000/FPS);
    }
}

void end () {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gScreenSurface = NULL;
    SDL_Quit();
}