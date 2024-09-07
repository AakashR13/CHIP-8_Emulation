#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "chip8.h"

#include <iostream>
#include <map>
#include <string>
#include <fstream>

// Define the width and height of the bytemap
static const int WIDTH = 640;
static const int HEIGHT = 480;

typedef std::map<std::string, std::string> SETTINGS_MAP ;

void HandleInput(Chip8* cpu, SDL_Event* event);
void SDL_INIT();
void EMU_LOOP(Chip8* cpu, const SETTINGS_MAP& settings);
void Render_Frame(Chip8* cpu);
bool LoadGameSettings(SETTINGS_MAP& settings);
bool CreateSDLWindow();
bool LoadChip8Rom(Chip8* cpu, const SETTINGS_MAP& settings);

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("Bitmap Example",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WIDTH, HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create an SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create a texture where the bitmap will be rendered
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGB888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             WIDTH, HEIGHT);
    if (!texture) {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    // Main loop
    bool quit = false;
    SDL_Event e;
    int frame = 0;
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear the screen
        SDL_RenderClear(renderer);

        // Render the texture
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);

        // Present the screen
        SDL_RenderPresent(renderer);

        // Increment the frame counter
        ++frame;
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


void HandleInput(Chip8* cpu, SDL_Event* event)
{
    if(event->type == SDL_KEYDOWN)
    {
        int key = -1;
        switch(event->key.keysym.sym)
        {
            case SDLK_x: key = 0; break;
            case SDLK_1: key = 1; break;
            case SDLK_2: key = 2; break;
            case SDLK_3: key = 3; break;
            case SDLK_q: key = 4; break;
            case SDLK_w: key = 5; break;
            case SDLK_e: key = 6; break;
            case SDLK_a: key = 7 ; break;
			case SDLK_s: key = 8 ; break;
			case SDLK_d: key = 9 ; break;
			case SDLK_z: key = 10 ; break;
			case SDLK_c: key = 11 ; break;
			case SDLK_4: key = 12 ; break;
			case SDLK_r: key = 13 ; break;
			case SDLK_f: key = 14 ; break;
			case SDLK_v: key = 15 ; break;
			default: break ;
        }
        if(key!=-1)
            cpu->KeyPressed(key);
    }
    else if(event->type == SDL_KEYUP)
    {
        int key = -1;
        switch(event->key.keysym.sym)
        {
            case SDLK_x: key = 0; break;
            case SDLK_1: key = 1; break;
            case SDLK_2: key = 2; break;
            case SDLK_3: key = 3; break;
            case SDLK_q: key = 4; break;
            case SDLK_w: key = 5; break;
            case SDLK_e: key = 6; break;
            case SDLK_a: key = 7 ; break;
			case SDLK_s: key = 8 ; break;
			case SDLK_d: key = 9 ; break;
			case SDLK_z: key = 10 ; break;
			case SDLK_c: key = 11 ; break;
			case SDLK_4: key = 12 ; break;
			case SDLK_r: key = 13 ; break;
			case SDLK_f: key = 14 ; break;
			case SDLK_v: key = 15 ; break;
			default: break ;
        }
        if(key!=-1)
            cpu->KeyReleased(key);
    }
}

void SDL_INIT()
{
    glViewport(0,0,WIDTH,HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0,WIDTH,0,HEIGHT,-1.0,1.0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_FLAT);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DITHER);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

void EMU_LOOP(Chip8* cpu, const SETTINGS_MAP& settings)
{
    SETTINGS_MAP::const_iterator it = settings.find("OpcodesPerSecond") ;

	// make sure the RomName setting is in the setting map
	if (settings.end() == it)
	{
		printf("The OpcodesPerSecond setting cannot be found in game.ini") ;
		return  ;
	}

	int fps = 60 ;

	// number of opcodes to execute a second
	int numopcodes = atoi((*it).second.c_str()) ;

	// number of opcodes to execute a frame 
	int numframe = numopcodes / fps ;

	bool quit = false ;
	SDL_Event event;	
	float interval = 1000 ;
	interval /= fps ;

	unsigned int time2 = SDL_GetTicks( ) ;

	while (!quit)
	{
		while( SDL_PollEvent( &event ) ) 
		{ 
			HandleInput( cpu, &event ) ;

			if( event.type == SDL_QUIT ) 
			{ 
				quit = true; 
			} 
		}

		unsigned int current = SDL_GetTicks( ) ;

		if( (time2 + interval) < current )
		{
			cpu->DecreaseTimers( ) ;
			for (int i = 0 ; i < numframe; i++)
				cpu->ExecuteNextOpcode( ) ;

			time2 = current ;
			Render_Frame(cpu) ;
		} 
	}
}

void Render_Frame(Chip8* cpu)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRasterPos2i(-1, 1);
	glPixelZoom(1, -1);
	glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, cpu->m_ScreenData);
	SDL_GL_SwapBuffers( ) ;
	glFlush();
}

bool LoadGameSettings(SETTINGS_MAP& settings)
{
    const int MAXLINE = 256;
    std::ifstream file ;
    file.open("game.ini");

    if (false == file.is_open())
    {
        printf("could not open settings file game.ini");
        return false ;
    }

    char line[MAXLINE] ;
    std::string settingname ;
    std::string settingvalue ;

    while (false == file.eof())
    {
        memset(line, '\0', sizeof(line));
        file.getline(line,MAXLINE);

        //allow for comments and line breaks
        if ('*' == line[0] || '\r' == line[0] || '\n' == line[0] || '\0' == line[0])
            continue ;

        // get the setting name
        char* name = 0;
        name = strtok(line, ":") ;
        settingname = name ;

        // get the setting value
        char* value = 0;
        value = strtok(NULL, "*") ;
        settingvalue = value ;

        // check for errors
        if (value == 0 || name == 0 || settingname.empty() || settingvalue.empty())
        {
            printf("game.ini appears to be malformed") ;
            file.close();
            return false ;
        }

        // add to settings map
        settings.insert(std::make_pair(settingname,settingvalue)) ;
}

bool CreateSDLWindow()
{
    if( SDL_Init(SDL_INIT_EVERYTHING) < 0 )
    {
        return false;
    }
    if( SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE) == NULL )
    {
        return false;
    }

    InitGL();

    SDL_WM_SetCaption("Chip8 Emulator", NULL);
    return true;
}

bool LoadChip8Rom(Chip8* cpu, const SETTINGS_MAP& settings)
{
    SETTINGS_MAP::const_iterator it = settings.find("RomName") ;

    // make sure the RomName setting is in the setting map
    if (settings.end() == it)
    {
        printf("The RomName setting cannot be found in game.ini");
        return false ;
    }

    // load the rom file into memory
    bool res = cpu->LoadRom( (*it).second ) ;

    return res ;
}