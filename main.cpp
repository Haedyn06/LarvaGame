#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string>

class KeyofKeyboard{
private:
    SDL_Color Color;
public:

    void setColor(SDL_Color color){
        Color = color;
    }
    SDL_Color getColor(){
        return Color;
    }

    KeyofKeyboard(SDL_Color color){
        Color = color;
    }

    // Draw A Rectangle Function
    void makeKey(SDL_Renderer* renderer, TTF_Font* font, const char* text, int X, int Y) {
        //Draw the rectangle
        SDL_Rect KeyType = {X, Y, 125, 125}; //X Y W H
        SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a); // Black rectangle
        SDL_RenderFillRect(renderer, &KeyType);

        //Render the text
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, {255, 255, 255, 255}); // White text
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        //Center the text inside the rectangle
        int textW = textSurface->w;
        int textH = textSurface->h;
        SDL_Rect textRect = {
            X + (125 - textW) / 2, // Center horizontally
            Y + (125 - textH) / 2, // Center vertically
            textW, textH
    };





    //Render the text texture
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Cleanup
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    }
};


// Function Declaration
void PressedKey(SDL_Event event, SDL_Keycode Key, KeyofKeyboard& KeyType);
void QuitAll(TTF_Font* &font, SDL_Renderer* &renderer, SDL_Window* &window);
void MoveOnce(SDL_Event event, SDL_Keycode Key1,  SDL_Keycode Key2, int &Pos, int speed);
void MoveTwice(SDL_Event event, SDL_Keycode Key, int &PosY, int &PosX, int Num);

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL failed to load: " << SDL_GetError() << std::endl;
        return -1;
    }

        // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { // Standard audio setup
        std::cerr << "SDL_mixer could not initialize: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Load sound effect (WAV file)
    Mix_Chunk* soundEffect = Mix_LoadWAV("assets/audio/Poopsound.mp3");
    if (!soundEffect) {
        std::cerr << "Failed to load sound: " << Mix_GetError() << std::endl;
    }

    // Load music file (MP3/OGG)
    Mix_Music* music = Mix_LoadMUS("assets/audio/Zombietsunami.mp3");
    if (!music) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF failed to initialize: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Load font
    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/Arial.ttf", 64); // Correct font path
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);

    // Create window
    SDL_Window* window = SDL_CreateWindow("New Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1800, 1200, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }


    //Declare Variables
    SDL_Event event;
    bool running = true;
    int ImgXpos = 0;
    int ImgYpos = 0;
    std::vector<SDL_Rect> Doodooseverywhere;
    SDL_Texture* PoopOocum = IMG_LoadTexture(renderer, "assets/images/Doodoo.png");
    // Play background music
    Mix_PlayMusic(music, -1); // -1 means loop forever

    // Play sound effect
    Mix_PlayChannel(-1, soundEffect, 0); // Channel -1 (auto-pick), play once

    int Scores = 0; 

    // Declare Classes
    KeyofKeyboard WKey = KeyofKeyboard({0, 0, 0, 255});
    KeyofKeyboard AKey = KeyofKeyboard({0, 0, 0, 255});
    KeyofKeyboard SKey = KeyofKeyboard({0, 0, 0, 255});
    KeyofKeyboard DKey = KeyofKeyboard({0, 0, 0, 255});
    KeyofKeyboard Score = KeyofKeyboard({0, 0, 0, 255});


    while (running) {

        // Set background color and clear screen
        SDL_SetRenderDrawColor(renderer, 207, 61, 61, 0); // White background
        SDL_RenderClear(renderer);

        // Draw rectangle with text (X = (0-1680) ; Y = (0-1080)) 140
        WKey.makeKey(renderer, font, "W", 840, 540);
        AKey.makeKey(renderer, font, "A", 700, 680);
        SKey.makeKey(renderer, font, "S", 840, 680);
        DKey.makeKey(renderer, font, "D", 980, 680);


        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                std::cout << "Escape key pressed!" << std::endl;
                running = false;
            }

            //Y-Axis
            MoveOnce(event, SDLK_s,  SDLK_w, ImgYpos, 30);
            MoveTwice(event, SDLK_d, ImgYpos, ImgXpos, -30);
            
            //X-Axis
            MoveOnce(event, SDLK_d,  SDLK_a, ImgXpos, 30);
            MoveTwice(event, SDLK_d, ImgYpos, ImgXpos, 30);

            PressedKey(event, SDLK_w, WKey);
            PressedKey(event, SDLK_a, AKey);
            PressedKey(event, SDLK_s, SKey);
            PressedKey(event, SDLK_d, DKey);

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                std::cout << "Mouse button pressed at (" 
                    << event.button.x << ", " << event.button.y << ")" << std::endl;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                SDL_Rect newInstance = { (ImgXpos + 50), (ImgYpos + 150), 100, 100 }; // Random position, 100x100 size
                Doodooseverywhere.push_back(newInstance);
                Mix_PlayChannel(-1, soundEffect, 0);
                Scores ++;
            }

        }  

        std::string numberStr = std::to_string(Scores);
        const char* digit = numberStr.c_str();
        Score.makeKey(renderer, font, digit, 0, 0);
        Score.setColor({207, 61, 61, 0});
        std::cout << digit << std::endl;

        SDL_Surface* surface = IMG_Load("assets/images/Larvaman.png");
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        SDL_Rect dest = {ImgXpos, ImgYpos, 200, 300}; //X Y W H
        SDL_RenderCopy(renderer, texture, NULL, &dest);
        SDL_DestroyTexture(texture);

        for (const auto& rect : Doodooseverywhere) {
            SDL_RenderCopy(renderer, PoopOocum, NULL, &rect);
        }


        SDL_RenderPresent(renderer);


    }
    // Cleanup
    QuitAll(font, renderer, window);
    SDL_DestroyTexture(PoopOocum);
    Mix_FreeChunk(soundEffect);
    Mix_FreeMusic(music);
    Mix_CloseAudio();

    return 0;
}



//Key Pressed Function
void PressedKey(SDL_Event event, SDL_Keycode Key, KeyofKeyboard& KeyType) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == Key) {
        std::cout << "Key pressed: " << SDL_GetKeyName(Key) << std::endl;
        KeyType.setColor({107, 107, 107, 0}); // Set to gray

    } else if (event.type == SDL_KEYUP && event.key.keysym.sym == Key)
    {
        KeyType.setColor({0, 0, 0, 255}); // Set to Black
    }
     
}



void MoveOnce(SDL_Event event, SDL_Keycode Key1,  SDL_Keycode Key2, int &Pos, int speed){
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == Key1) {
        Pos += speed;
        std::cout << "Pressed" << std::endl;
    } else{
        Pos += 0;
    }

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == Key2) {
        Pos -= speed;
    } else{
        Pos += 0;
    }
}

void MoveTwice(SDL_Event event, SDL_Keycode Key, int &PosY, int &PosX, int Num){
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_w && event.key.keysym.sym == Key) {
        PosX += Num;
        PosY += 15;
        std::cout << "Pressed" << std::endl;
    } else{

        PosY += 0;
        PosX += 0;
    }

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s && event.key.keysym.sym == Key) {
        PosX += Num;
        PosY -= 15;
    } else{
        PosY += 0;
        PosX += 0;
    }
}

void QuitAll(TTF_Font* &font, SDL_Renderer* &renderer, SDL_Window* &window){

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

}