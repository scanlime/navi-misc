#include <annie.h>

int main(void) {
  annie::TwoLayerNetwork net(8,64,3);
  net.connectAll();
  net.save("data/current.net");
  
  return 0;
}
