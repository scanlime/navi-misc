#include <annie.h>

using namespace annie;

void main(int argc, char *argv[])
{
  try
	{
	  TwoLayerNetwork net(argv[1]);
	  TrainingSet T(argv[2],(argv[3][0]=='b')?BINARY_FILE:TEXT_FILE);

	  T.initialize();
	  annie::VECTOR in,out;
	  while (!T.epochOver())
		{
		  T.getNextPair(in,out);
		  cout<<net.getOutput(in)[0]<<endl;
		}
	}
  catch (Exception &e)
	{
	  cout<<e.what()<<endl;
	}
}
