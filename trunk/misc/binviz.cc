/*
 * Experimental auto-aligning interactive binary visualizer.
 *
 * Micah Dowty <micah@navi.cx>
 *
 * g++ -O3 -o binviz binviz.cc `sdl-config --cflags --libs`
 */

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

SDL_Event mouse;

int
evaluatePitch(uint8_t *bytes, uint8_t *limit, int pitch)
{
   const int minPitch = 16;

   uint8_t *nextLine = bytes + pitch;
   int score = 0;
   int x;

   for (x = 0; x < pitch; x++) {
      if (nextLine + x >= limit) {
         break;
      }

      int diff = nextLine[x] - bytes[x];
      diff = diff * diff;
      score -= diff;
   }

   return score;
}

int
findBestPitch(uint8_t *bytes, uint8_t *limit, int target)
{
   int bestPitch = 0;
   int bestScore = 100;
   int i;

   for (i = target - 32; i < target + 32; i++) {
      int score = evaluatePitch(bytes, limit, i);

      if (score < bestScore) {
         bestScore = score;
         bestPitch = i;
      }
   }

   return bestPitch;
}

void
draw(void *data, size_t dataSize, uint32_t *pixels, int pitch, int width, int height)
{
   int x, y, c;
   uint8_t *bytes = (uint8_t*) data;
   uint8_t *limit = bytes + dataSize;
   const int columns = 4;
   int colWidth = width / columns;

   int maxPitch = mouse.motion.x;
   bytes += mouse.motion.y * 4096 * 32;

   for (c = 0; c < columns; c++) {
      for (y = 0; y < height; y++) {
         uint32_t *line = (uint32_t *) (y * pitch + 4*colWidth*c + (uint8_t*)pixels);
         int wLimit = findBestPitch(bytes, limit, maxPitch);

         for (x = 0; x < colWidth; x++) {
            uint8_t r, g, b;

            if (x < wLimit && bytes < limit) {
               r = g = b = *(bytes++);
            } else {
               r = 0x80;
               g = b = 0;
            }

            *(line++) = (r << 16) | (g << 8) | b;
         }
      }
   }
}

void *
mapfile(const char *name, size_t *dataSize)
{
   size_t length;
   int fd;
   struct stat st;
   void *mapped;

   fd = open(name, O_RDONLY);
   if (fd < 0) {
      perror("open");
      exit(1);
   }

   if (fstat(fd, &st)) {
      perror("stat");
      exit(1);
   }

   mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
   if (mapped == MAP_FAILED) {
      perror("mmap");
      exit(1);
   }

   *dataSize = st.st_size;
   return mapped;
}

bool
handleEvent(SDL_Event *event)
{
   switch (event->type) {

   case SDL_QUIT:
      return false;

   case SDL_MOUSEMOTION:
      if (event->motion.state)
         mouse = *event;
      break;
   }
   return true;
}

int
main(int argc, char **argv)
{
   SDL_Surface *screen;
   bool running = true;
   const int width = 1024;
   const int height = 768;
   size_t dataSize;
   void *mapped = mapfile(argv[1], &dataSize);

   if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      fprintf(stderr, "SDL initialization error: %s\n", SDL_GetError());
   }

   screen = SDL_SetVideoMode(width, height, 32, 0);
   if (!screen) {
      fprintf(stderr, "Error setting video mode: %s\n", SDL_GetError());
   }

   while (running) {
      SDL_Event event;

      SDL_LockSurface(screen);
      draw(mapped, dataSize, (uint32_t*) screen->pixels,
           screen->pitch, width, height);
      SDL_UnlockSurface(screen);
      SDL_UpdateRect(screen, 0, 0, width, height);

      //      if (SDL_WaitEvent(&event)) {
      //         running = handleEvent(&event);
         while (running && SDL_PollEvent(&event)) {
            running = handleEvent(&event);
         }
      //      }
   }

   SDL_Quit();
   return 0;
}
