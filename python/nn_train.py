#!/usr/bin/python
import bpnn, pickle

nn = pickle.load(open("nn_network.pickle"))
pattern = pickle.load(open("nn_pattern.pickle"))

try:
    nn.train(pattern)
    nn.test(pattern)
finally:
    pickle.dump(nn, open("nn_network.pickle", "w"))
