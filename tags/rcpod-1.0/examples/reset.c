/* Just opens the rcpod then exits, causing it to be reset.
 * Handy if the board is in a state it shouldn't be in.
 */

#include <rcpod.h>

int main() {
  rcpod_InitSimple();
  return 0;
}
