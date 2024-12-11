#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string>

using std::string;
using std::cout;

class Initializations {
private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    int width;
    int height;

public:
    // Constructor
    Initializations(int Width, int Height) : width(Width), height(Height) {
        if (InitializeImage() != 0 || InitializeWindow() != 0 || InitializeRenderer() != 0) {
            std::cerr << "Initialization failed!" << std::endl;
        }
    }

    // Setters
    void setWindow(SDL_Window *Window) {
        window = Window;
    }
    void setRenderer(SDL_Renderer *Renderer) {
        renderer = Renderer;
    }

    // Getters
    SDL_Window* getWindow() {
        return window;
    }
    SDL_Renderer* getRenderer() {
        return renderer;
    }

    // SDL Initialization
    int InitializeImage() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            std::cerr << "SDL failed to load: " << SDL_GetError() << std::endl;
            return -1;
        }
        return 0;
    }

    // Audio Initialization
    int InitializeAudio() {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cerr << "SDL_mixer could not initialize: " << Mix_GetError() << std::endl;
            return -1;
        }
        return 0;
    }

    // Font Initialization
    int InitializeFont() {
        if (TTF_Init() == -1) {
            std::cerr << "TTF failed to initialize: " << TTF_GetError() << std::endl;
            return -1;
        }
        return 0;
    }

    // Window Initialization
    int InitializeWindow() {
        window = SDL_CreateWindow("New Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return -1;
        }
        return 0;
    }

    // Renderer Initialization
    int InitializeRenderer() {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return -1;
        }
        return 0;
    }

    // Cleanup Function
    void QuitAll() {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
    }
};

class KeyofKeyboard {
private:
    SDL_Color Color;

public:
    void setColor(SDL_Color color) {
        Color = color;
    }
    SDL_Color getColor() {
        return Color;
    }

    KeyofKeyboard(SDL_Color color) {
        Color = color;
    }

    // Draw A Rectangle Function
    void makeKey(SDL_Renderer *renderer, TTF_Font *font, const char *text, int X, int Y) {
        // Draw the rectangle
        SDL_Rect KeyType = {X, Y, 125, 125};                                  // X Y W H
        SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);
        SDL_RenderFillRect(renderer, &KeyType);

        // Render the text
        SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, {255, 255, 255, 255}); // White text
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Center the text inside the rectangle
        int textW = textSurface->w;
        int textH = textSurface->h;
        SDL_Rect textRect = {
            X + (125 - textW) / 2, // Center horizontally
            Y + (125 - textH) / 2, // Center vertically
            textW, textH};

        // Render the text texture
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Cleanup
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
};

// Function Declaration
TTF_Font* LoadFont(int size);
Mix_Music* loadmusic(const std::string& Path);
Mix_Chunk* loadaudioeffect(const std::string& Path);
void Jump(SDL_Keycode Key1, int &Pos, int &velocity, bool &jumping);
void PressedKey(SDL_Event event, SDL_Keycode Key, KeyofKeyboard &KeyType);


//Real Program
int main(int argc, char *argv[]) {
    //Class Declaration
    KeyofKeyboard WKey = KeyofKeyboard({0, 0, 0, 255});
    KeyofKeyboard AKey = KeyofKeyboard({0, 0, 0, 255});
    KeyofKeyboard SKey = KeyofKeyboard({0, 0, 0, 255});
    KeyofKeyboard DKey = KeyofKeyboard({0, 0, 0, 255});
    KeyofKeyboard Score = KeyofKeyboard({0, 0, 0, 255});


    //Initial State of Variables
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    Mix_Chunk *soundEffect;
    Mix_Music *music;
    TTF_Font *font;
    TTF_Font *font2;

    //Setup
    Initializations InitialSetup = Initializations(1800, 1200);
    window = InitialSetup.getWindow();
    renderer = InitialSetup.getRenderer();
    InitialSetup.InitializeAudio();
    InitialSetup.InitializeFont();

    //Audio Setup
    music = loadmusic("assets/audio/Zombietsunami.mp3");
    soundEffect = loadaudioeffect("assets/audio/Poopsound.mp3");
    font = LoadFont(64);
    font2 = LoadFont(45);

    //Font Setup
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    TTF_SetFontStyle(font2, TTF_STYLE_BOLD);


    //Variable Declaration
    int ImgXpos = 100;
    int ImgYpos = 800;
    int velocity = 0;
    
    bool running = true;
    bool jumping = false;

    Uint32 lastTime = SDL_GetTicks();
    std::vector<SDL_Rect> Doodooseverywhere;
    SDL_Texture *PoopOocum = IMG_LoadTexture(renderer, "assets/images/Doodoo.png");
    Mix_PlayMusic(music, -1);

    // Play sound effect
    Mix_PlayChannel(-1, soundEffect, 0); // Channel -1 (auto-pick), play once

    int Scores = 0;



    while (running)
    {

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Delta time in seconds
        lastTime = currentTime;
        float moveSpeed = 500.0f;
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        std::string numberStr = std::to_string(Scores) + " Doodoos From Larva";
        const char *digit = numberStr.c_str();


        SDL_Surface *surface = IMG_Load("assets/images/Larvaman.png");
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dest = {ImgXpos, ImgYpos, 200, 300}; // X Y W H

        //Background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); 
        SDL_Rect sky = {0, 0, 1800, 1200};                    
        SDL_RenderFillRect(renderer, &sky);

        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_Rect ground = {0, 950, 1800, 250};             
        SDL_RenderFillRect(renderer, &ground);

        //(X = (0-1680) ; Y = (0-1080)
        WKey.makeKey(renderer, font, "W", 840, 540);
        AKey.makeKey(renderer, font, "A", 700, 680);
        SKey.makeKey(renderer, font, "S", 840, 680);
        DKey.makeKey(renderer, font, "D", 980, 680);

        //Event Actions
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            //Cursor Location
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                std::cout << "Mouse button pressed at ("
                          << event.button.x << ", " << event.button.y << ")" << std::endl;
            }

            //Quit Program
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                std::cout << "Escape key pressed!" << std::endl;
                running = false;
            }

            //Poop
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                SDL_Rect newInstance = {(ImgXpos + 50), (ImgYpos + 150), 100, 100};
                Doodooseverywhere.push_back(newInstance);
                Mix_PlayChannel(-1, soundEffect, 0);
                Scores++;
            }

            //Key Functions
            PressedKey(event, SDLK_w, WKey);
            PressedKey(event, SDLK_a, AKey);
            PressedKey(event, SDLK_s, SKey);
            PressedKey(event, SDLK_d, DKey);
        }

        //Gravity
        if (!jumping && ImgYpos < 700) {
            ImgYpos += 10;
        }

        //Movement
        Jump(SDLK_w, ImgYpos, velocity, jumping);

        if (keys[SDL_SCANCODE_A]) {
            ImgXpos -= moveSpeed * deltaTime;
        }
        if (keys[SDL_SCANCODE_D]) {
            ImgXpos += moveSpeed * deltaTime;
        }


        //Scoreboard
        Score.makeKey(renderer, font2, digit, 250, 0);
        Score.setColor({135, 206, 235, 255});

        SDL_FreeSurface(surface);

        SDL_RenderCopy(renderer, texture, NULL, &dest);
        SDL_DestroyTexture(texture);

        for (const auto &rect : Doodooseverywhere) {
            SDL_RenderCopy(renderer, PoopOocum, NULL, &rect);
        }

        //Render Everything
        SDL_RenderPresent(renderer);
    }
    // Cleanup
    InitialSetup.QuitAll();
    SDL_DestroyTexture(PoopOocum);
    Mix_FreeChunk(soundEffect);
    Mix_FreeMusic(music);

    return 0;
}

//Functions

//Sound Effects Function
Mix_Chunk* loadaudioeffect(const std::string& Path) {
    Mix_Chunk* soundEffect = Mix_LoadWAV(Path.c_str()); // Convert std::string to const char*
    if (!soundEffect) {
        std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
    }
    return soundEffect; // Return pointer
}

//Music Function
Mix_Music* loadmusic(const std::string& Path) {
    Mix_Music* music = Mix_LoadMUS(Path.c_str());
    if (!music) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
    }
    return music;
}

//Creat Font Function
TTF_Font* LoadFont(int size) {
    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/Arial.ttf", size); 
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    return font;
}



// Key Pressed Function
void PressedKey(SDL_Event event, SDL_Keycode Key, KeyofKeyboard &KeyType) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == Key) {
        std::cout << "Key pressed: " << SDL_GetKeyName(Key) << std::endl;
        KeyType.setColor({107, 107, 107, 0}); //Gray

    } else if (event.type == SDL_KEYUP && event.key.keysym.sym == Key) {
        KeyType.setColor({0, 0, 0, 255}); //Black
    }
}

//Gravity Jump Function
void Jump(SDL_Keycode Key1, int &PosY, int &velocity, bool &jumping) {

    if (SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(Key1)] && !jumping) {
        jumping = true; 
        velocity = -30;
        std::cout << "Jump started!" << std::endl;
    }

    if (jumping) {
        PosY += velocity; 
        velocity += 2;

        if (PosY >= 800) {                    
            PosY = 800;      
            jumping = false; 
            velocity = 0;    
            std::cout << "Landed!" << std::endl;
        }
    }
}




