#include <unistd.h>
#include <linux/input.h>
#include <fcntl.h>

int main(int argc, char **argv) {
  struct input_event ev;
  int fd = open(argv[1], O_WRONLY);

  ev.type = EV_LED;
  ev.code = LED_MISC;
  ev.value = 1;

  while (1) {
    write(fd, &ev, sizeof(ev));
    ev.value = !ev.value;
//    usleep(200000);
  }

  return 0;
}
