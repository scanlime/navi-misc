#include <FieldNetwork.h>
#include <iostream>
using namespace std;

int main(void) {
  try {
    FieldNetwork net("data/current.net");
    TrainingSet pattern("data/4x4cube.pattern");
    VECTOR input,desired,output;
    VECTOR::iterator i,j;
    float error;
    int numEpochs = 0;
    const int epochsPerRound = 50;
    
    FILE *errfile = fopen("errorterms.log", "a");

    while (1) {
      /* Train the net a bit... */
      net.train(pattern, epochsPerRound, 0.99, 0.0, false);
      numEpochs += epochsPerRound;

      /* Save our progress */
      net.save("data/current.net");

      /* Compute an error value */
      pattern.initialize();
      error = 0;
      while (!pattern.epochOver()) {
	pattern.getNextPair(input,desired);
	output = net.getOutput(input);
	for (i=output.begin(), j=desired.begin(); i!=output.end(); i++, j++)
	  error += (*i - *j) * (*i - *j);
      }
      printf(" Training... %10d epochs : error = %f\n", numEpochs, error);
      fprintf(errfile,"%f\n", error);
      fflush(errfile);
    }
  }
  catch (annie::Exception &e) {
    cout << e.what() << endl;
  }
  
  return 0;
}
