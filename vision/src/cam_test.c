#include <cv_dc1394.h>
#include <SDL.h>

int main() {
  IplImage** images;
  SDL_Overlay *overlay;
  SDL_Surface *screen;
  SDL_Event event;
  SDL_Rect r;
  unsigned char *src, *dest;
  int i;

  cv_dc1394_init();

  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(640, 480, 0, 0);
  overlay = SDL_CreateYUVOverlay(640, 480, SDL_YUY2_OVERLAY, screen);

  while (1) {
    images = cv_dc1394_capture_yuv(1);

    CvPoint pt1, pt2;
    pt1.x = 100;
    pt1.y = 100;
    pt2.x = 200;
    pt2.y = 200;
    cvRectangle(images[0], pt1, pt2, CV_RGB(255, 128, 128), 3);

    SDL_LockYUVOverlay(overlay);
    src = (unsigned char *) images[0]->imageData;
    dest = overlay->pixels[0];
    for (i=0; i<640*480; i+=2) {
      int y0, y1, u0, u1, v0, v1, u, v;
      y0 = *(src++);
      u0 = *(src++);
      v0 = *(src++);
      y1 = *(src++);
      u1 = *(src++);
      v1 = *(src++);
      u = (u0 + u1)>>1;
      v = (v0 + v1)>>1;
      *(dest++) = y0;
      *(dest++) = u;
      *(dest++) = y1;
      *(dest++) = v;
    }
    SDL_UnlockYUVOverlay(overlay);

    r.x = 0;
    r.y = 0;
    r.w = 640;
    r.h = 480;
    SDL_DisplayYUVOverlay(overlay, &r);

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
      return 0;
  }
}
