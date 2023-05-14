#include "engine.h"
#include "maze.h"

//Using SDL, SDL_image, standard IO, and strings
/*
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Walking animation
const int WALKING_ANIMATION_FRAMES = 4;
SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;


LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load sprite sheet texture
	if (!gSpriteSheetTexture.loadFromFile("assets/rat-charset.png"))
	{
		printf("Failed to load walking animation texture!\n");
		success = false;
	}
	else
	{
		//Set sprite clips
		gSpriteClips[0].x = 0;
		gSpriteClips[0].y = 0;
		gSpriteClips[0].w = 70;
		gSpriteClips[0].h = 105;

		gSpriteClips[1].x = 70;
		gSpriteClips[1].y = 0;
		gSpriteClips[1].w = 72;
		gSpriteClips[1].h = 105;

		gSpriteClips[2].x = 141;
		gSpriteClips[2].y = 0;
		gSpriteClips[2].w = 73;
		gSpriteClips[2].h = 105;

		gSpriteClips[3].x = 210;
		gSpriteClips[3].y = 0;
		gSpriteClips[3].w = 73;
		gSpriteClips[3].h = 105;
	}

	return success;
}

void close()
{
	//Free loaded images
	gSpriteSheetTexture.free();

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

*/

int main(int, char* [])
{
	// Start up the engine
	if (not engine::init(640, 480))
	{
		engine::close();
		return -1;
	}

	// Inicializa o jogo
	game::Maze maze(64, 48, 10);

	// Main loop
	while (true)
	{
		//       1. process_events();


		//       2. update_objects();

	   //        3. draw the frame
		engine::screen::clear();

		maze.draw();

		engine::screen::show();

		//       4. wait_until_next_frame();

		break;
	}

	/*
				//Main loop flag
				bool quit = false;

				//Event handler
				SDL_Event e;

				//Current animation frame
				int frame = 0;
				int w = 71, h = 58;
				SDL_Rect currentClip { 0, h, w, h };

				//While application is running
				while (!quit)
				{
					//Handle events on queue
					while (SDL_PollEvent(&e) != 0)
					{
						//User requests quit
						if (e.type == SDL_QUIT)
						{
							quit = true;
						}

						if (e.type = SDL_KEYDOWN)
						{
							switch (e.key.keysym.sym) {
							case SDLK_UP:
								currentClip.y = 3*h;
								break;

							 case SDLK_DOWN:
								currentClip.y = 0;
								break;

							 case SDLK_LEFT:
								currentClip.y = h;
								break;

							 case SDLK_RIGHT:
								currentClip.y = 2*h;
								break;
							}
						}
					}

					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					//Render current frame
					//SDL_Rect* currentClip = &gSpriteClips[frame];

					gSpriteSheetTexture.render((SCREEN_WIDTH - currentClip.w) / 2, (SCREEN_HEIGHT - currentClip.h) / 2, &currentClip);

					//Update screen
					SDL_RenderPresent(gRenderer);
					SDL_Delay(100);

					//Go to next frame
					++frame;
					currentClip.x += w;

					//Cycle animation
					if (frame == WALKING_ANIMATION_FRAMES)
					{
						frame = 0;
						currentClip.x = 0;
					}
				}
			}
		}
	*/
	engine::close();

	return 0;
}