#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "chip8.h"

#include <iostream>
#include <map>
#include <string>
#include <fstream>

// Define the width and height of the bytemap
static const int WIDTH = 640;
static const int HEIGHT = 320;

typedef std::map<std::string, std::string> SETTINGS_MAP ;

void HandleInput(Chip8* cpu, SDL_Event* event, bool &quit);
bool GL_INIT();
void EMU_LOOP(Chip8* cpu, const SETTINGS_MAP& settings);
void Render_Frame(Chip8* cpu);
bool LoadGameSettings(SETTINGS_MAP& settings);
bool CreateSDLWindow(SDL_Window** window, SDL_GLContext* glContext);
bool LoadChip8Rom(Chip8* cpu, const SETTINGS_MAP& settings, std::string* romName);
bool SaveScreenShot(const std::string& filename);


int main(int argc, char* argv[]) {
    // Create an SDL window and OpenGL context
    SDL_Window* window = nullptr;
    SDL_GLContext glContext;

    // Create a Chip8 CPU object
    Chip8* cpu = Chip8::CreateSingleton();

    // Create a map for storing settings
    SETTINGS_MAP settings;
    std::string romName = "";

    // Load game settings from "settings.ini"
    if (!LoadGameSettings(settings)) {
        std::cerr << "Failed to load game settings from settings.ini" << std::endl;
        return 1;
    }

    // Initialize SDL and OpenGL window
    if (!CreateSDLWindow(&window, &glContext)) {
        std::cerr << "Failed to create SDL Window or OpenGL context" << std::endl;
        return 1;
    }

    // Load the Chip8 ROM based on the settings
    if (!LoadChip8Rom(cpu, settings, &romName)) {
        std::cerr << "Failed to load Chip8 ROM" << std::endl;
        return 1;
    }

    // Run the emulator loop
    EMU_LOOP(cpu, settings);

    // Cleanup and shutdown
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}



void HandleInput(Chip8* cpu, SDL_Event* event, bool &quit)
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
            case SDLK_ESCAPE: quit = true; break;
            case SDLK_F12:
                SaveScreenShot("./images/screenshot_" + std::to_string(cpu->GetProgramCounter()) + ".bmp");
                // printf("Screenshot saved\n");
			default: break ;
        }
        if(key!=-1)
        {
            cpu->KeyPressed(key);
            // printf("Key pressed: %d\n", key);
        }
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
        {
            cpu->KeyReleased(key);
            // printf("Key released: %d\n", key);  
        }
    }
}

bool GL_INIT()
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
    return true;
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
			HandleInput( cpu, &event, quit) ;

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
	SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow()); ;
	glFlush();
}

bool LoadGameSettings(SETTINGS_MAP& settings)
{
    const int MAXLINE = 256;
    std::ifstream file ;
    file.open("./src/settings.ini");

    if (false == file.is_open())
    {
        printf("could not open settings file settings.ini");
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
            printf("settings.ini appears to be malformed") ;
            file.close();
            return false ;
        }

        // add to settings map
        settings.insert(std::make_pair(settingname,settingvalue)) ;
        
}
    return true;
}

bool CreateSDLWindow(SDL_Window** window, SDL_GLContext* glContext)
{
    // Initialize SDL with video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create an SDL window
    *window = SDL_CreateWindow("Chip8 Emulator",
                               SDL_WINDOWPOS_CENTERED, // Center the window on the screen
                               SDL_WINDOWPOS_CENTERED,
                               WIDTH, HEIGHT,
                               SDL_WINDOW_OPENGL);     // Enable OpenGL context for this window
    if (*window == nullptr)
    {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create an OpenGL context associated with the window
    *glContext = SDL_GL_CreateContext(*window);
    if (*glContext == nullptr)
    {
        std::cerr << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize OpenGL settings
    if (!GL_INIT())
    {
        std::cerr << "Unable to initialize OpenGL!" << std::endl;
        return false;
    }

    // Set the window title
    SDL_SetWindowTitle(*window, "Chip8 Emulator");

    return true;
}

bool LoadChip8Rom(Chip8* cpu, const SETTINGS_MAP& settings, std::string* romName)
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
    romName->assign((*it).second) ;
    return res ;
}

bool SaveScreenShot(const std::string& filename){
    int width, height;
    SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &width, &height);

    // Allocate a buffer to store the pixel data
    unsigned char* pixels = new unsigned char[3 * width * height];  // 3 bytes per pixel (RGB)

    // Read pixels from the framebuffer
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Create an SDL surface to save the screenshot
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        pixels, width, height, 24, 3 * width, 
        0x000000FF, 0x0000FF00, 0x00FF0000, 0
    );

    if (!surface) {
        std::cerr << "Failed to create SDL surface: " << SDL_GetError() << std::endl;
        delete[] pixels;
        return false;
    }

    // Save the surface as a BMP file
    if (SDL_SaveBMP(surface, filename.c_str()) != 0) {
        std::cerr << "Failed to save screenshot: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        delete[] pixels;
        return false;
    }

    // Free the surface and pixel buffer
    SDL_FreeSurface(surface);
    delete[] pixels;

    std::cout << "Screenshot saved to " << filename << std::endl;
    return true;
}