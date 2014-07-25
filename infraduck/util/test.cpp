#include<SDL/SDL.h>
#include<stdlib.h>

int main() {
SDL_Init(SDL_INIT_VIDEO);
SDL_Surface *s = SDL_SetVideoMode(640, 480, 0, SDL_FULLSCREEN | SDL_DOUBLEBUF);
SDL_Rect r; r.x = 200; r.y = 150; r.w = 80; r.h = 60;
int white = SDL_MapRGB(s->format, 255, 255, 255);
SDL_ShowCursor(SDL_DISABLE);
SDL_FillRect(s, &r, white);
SDL_Flip(s);
SDL_Event e;
while(1) {
while(SDL_PollEvent(&e)) {
if(e.type == SDL_KEYDOWN) {
SDL_Quit();
exit(0);
}
}
}
}
