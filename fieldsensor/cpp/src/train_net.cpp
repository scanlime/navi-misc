#include <FieldNetwork.h>
#include <iostream>
using namespace std;

int main(void) {
  try {
    FieldNetwork net("data/current.net");
    TrainingSet pattern("data/4x4cube.pattern");
    
    while (1) {
      net.train(pattern, 100, 0.8, 0.0, true);
      net.save("data/current.net");
      cout << "Saved." << endl;
    }
  }
  catch (annie::Exception &e) {
    cout << e.what() << endl;
  }
  
  return 0;
}
