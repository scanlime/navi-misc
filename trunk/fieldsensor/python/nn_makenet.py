#!/usr/bin/python
import pickle, bpnn

nn = bpnn.NN(8,64,3)
pickle.dump(nn, open("nn_network.pickle", "w"))
print "New neural net saved."
