#include <SDL2/SDL.h>
#include <iostream>

// Define the width and height of the bytemap
const int WIDTH = 640;
const int HEIGHT = 480;

typedef uint8_t BitmapRow;  // 8 bits per row segment
const int BITMAP_ROW_SIZE = (WIDTH + 7) / 8;  // Number of bytes per row in the bitmap

// Function prototype for updating the bitmap
void updateBitmap(BitmapRow* bitmap, int width, int height, int frame);

// Function to get a pixel from the bitmap
inline uint8_t getPixel(const BitmapRow* bitmap, int x, int y, int width) {
    int byteIndex = y * BITMAP_ROW_SIZE + x / 8;
    int bitIndex = x % 8;
    return (bitmap[byteIndex] >> (7 - bitIndex)) & 1;
}

// Function to set a pixel in the bitmap
inline void setPixel(BitmapRow* bitmap, int x, int y, int width, uint8_t value) {
    int byteIndex = y * BITMAP_ROW_SIZE + x / 8;
    int bitIndex = x % 8;
    if (value) {
        bitmap[byteIndex] |= (1 << (7 - bitIndex));  // Set bit to 1
    } else {
        bitmap[byteIndex] &= ~(1 << (7 - bitIndex));  // Set bit to 0
    }
}

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

    // Allocate memory for the bitmap
    BitmapRow* bitmap = new BitmapRow[BITMAP_ROW_SIZE * HEIGHT]();

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

        // Update the bitmap (fill with black and white pattern)
        updateBitmap(bitmap, WIDTH, HEIGHT, frame);

        // Prepare a buffer to hold the RGB pixel data
        uint32_t* pixels = new uint32_t[WIDTH * HEIGHT];

        // Convert bitmap to RGB format (black and white)
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                uint8_t color = getPixel(bitmap, x, y, WIDTH);
                pixels[y * WIDTH + x] = (color == 0) ? 0x000000 : 0xFFFFFF;  // Black or White
            }
        }

        // Copy the pixel buffer to the texture
        void* texturePixels;
        int pitch;
        SDL_LockTexture(texture, nullptr, &texturePixels, &pitch);
        memcpy(texturePixels, pixels, WIDTH * HEIGHT * sizeof(uint32_t));
        SDL_UnlockTexture(texture);

        // Free the pixel buffer
        delete[] pixels;

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
    delete[] bitmap;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Example function to update the bitmap
void updateBitmap(BitmapRow* bitmap, int width, int height, int frame) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Simple pattern: alternate black and white pixels, change pattern every 30 frames
            setPixel(bitmap, x, y, width, ((x + y + frame / 30) % 2 == 0) ? 0 : 1);  // 0 = Black, 1 = White
        }
    }
}
