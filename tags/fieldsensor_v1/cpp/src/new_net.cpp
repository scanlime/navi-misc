#include <FieldNetwork.h>
#include <iostream>
using namespace std;

int main(void) {
  try {
    FILE *errfile = fopen("errorterms.log", "w");
    FieldNetwork net;
    net.save("data/current.net");  
  }
  catch (annie::Exception &e) {
    cout << e.what() << endl;
  }
  
  return 0;
}