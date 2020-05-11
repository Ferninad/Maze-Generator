#include "common.h"
#include "cmath"
#include "vector"

bool Init();
void CleanUp();
void Run();
void Draw();
void Generate(int x, int y);
bool VisitedNeighbors(int x, int y); //checks if visited all cells neighbors
vector<char> UnvisitedNeighbors(int x, int y); //returns cells unvisited neighbors

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

double screenWidth = 501;
double screenHeight = 501;
double gridSize = 20;
int px = 0;
int py = 0;

vector<vector<vector<double>>> grid;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));

    int vectorWidth = screenWidth/gridSize; //X
    int vectorHeight = screenHeight/gridSize; //Y

    vector<vector<double>> y(vectorHeight, {1, 1, 1, 1, 0}); //U D L R walls present, 0 for not visited
    for(int i = 0; i < vectorWidth; i++){
        grid.push_back(y);
    }
    Generate(0, 0);
    
    while (gameLoop)
    {   
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        Draw();
        SDL_RenderPresent(renderer);
        pos.x = 0;
        pos.y = 0;
        pos.w = screenWidth;
        pos.h = screenHeight;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);
        
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    case SDLK_w:
                        if(py > 0)
                            py--;
                        break;
                    case SDLK_a:
                        if(px > 0)
                            px--;
                        break;
                    case SDLK_s:
                        if(py < grid[0].size()-1)
                            py++;
                        break;
                    case SDLK_d:
                        if(px < grid.size()-1)
                            px++;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    default:
                        break;
                }
            }
        }
    }
}

void Draw(){
    for(int x = 0; x < grid.size(); x++){
        for(int y = 0; y < grid[x].size(); y++){
            if(grid[x][y][0] == 1)
                SDL_RenderDrawLine(renderer, x*gridSize, y*gridSize, x*gridSize+gridSize, y*gridSize);
            if(grid[x][y][1] == 1)
                SDL_RenderDrawLine(renderer, x*gridSize, y*gridSize+gridSize, x*gridSize+gridSize, y*gridSize+gridSize);
            if(grid[x][y][2] == 1)
                SDL_RenderDrawLine(renderer, x*gridSize, y*gridSize, x*gridSize, y*gridSize+gridSize);
            if(grid[x][y][3] == 1)
                SDL_RenderDrawLine(renderer, x*gridSize+gridSize, y*gridSize, x*gridSize+gridSize, y*gridSize+gridSize);
        }
    }
}

void Generate(int x, int y){ //recursive algorithum
    grid[x][y][4] = 1;
    while(VisitedNeighbors(x, y) == false){
        vector<char> UVN = UnvisitedNeighbors(x, y);
        int index = rand() % UVN.size(); //chooses cell to do next
        char cell = UVN[index];

        if(cell == 'U'){
            grid[x][y][0] = 0;
            grid[x][y-1][1] = 0;
            Generate(x, y-1);
        }
        else if(cell == 'D'){
            grid[x][y][1] = 0;
            grid[x][y+1][0] = 0;
            Generate(x, y+1);
        }
        else if(cell == 'L'){
            grid[x][y][2] = 0;
            grid[x-1][y][3] = 0;
            Generate(x-1, y);
        }
        else if(cell == 'R'){
            grid[x][y][3] = 0;
            grid[x+1][y][2] = 0;
            Generate(x+1, y);
        }
    }
}

bool VisitedNeighbors(int x, int y){ //checks adjacent cells, returns false if a cell is unvisited
    int offset = y-1;
    if(y-1 >= 0){
        if(grid[x][y-1][4] == 0)
            return false;
    }
    offset = y+1;
    if(offset < grid[x].size()){
        if(grid[x][y+1][4] == 0)
            return false;
    }

    offset = x-1;
    if(offset >= 0){
        if(grid[x-1][y][4] == 0)
            return false;
    }
    offset = x+1;
    if(x+1 < grid.size()){
        if(grid[x+1][y][4] == 0)
            return false;
    }

    return true;
}

vector<char> UnvisitedNeighbors(int x, int y){ //returns vector holding direction of unvisited cells
    vector<char> unvisited; //holds the direction of the unvisited cells, U D L R
    int offset = y-1;
    
    if(y-1 >= 0){
        if(grid[x][y-1][4] == 0)
            unvisited.push_back('U');
    }
    offset = y+1;
    if(offset < grid[x].size()){
        if(grid[x][y+1][4] == 0)
            unvisited.push_back('D');
    }

    offset = x-1;
    if(offset >= 0){
        if(grid[x-1][y][4] == 0)
            unvisited.push_back('L');
    }
    offset = x+1;
    if(x+1 < grid.size()){
        if(grid[x+1][y][4] == 0)
            unvisited.push_back('R');
    }

    return unvisited;
}