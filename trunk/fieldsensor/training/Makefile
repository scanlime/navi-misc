#
# Makefile for training the field sensor.
# An efs-1.log file produced by the 'tracker' app in navi-misc's
# vision module comes in, gets processed by libsvm and included
# scripts, and an SVM model comes out.
#

# Input file, in the tracker log format
INPUT_FILE = efs-1.log

# Parameters for building the SVM models
SVMFLAGS = -s 3 -p 0.01

#######################################################################

default: x.model y.model z.model

# Split the test and training sets into files for each output variable
split_log: $(INPUT_FILE)
	./split-efs-log.py $(INPUT_FILE) tmp/x.training tmp/y.training tmp/z.training

tmp/x.training: split_training_set
tmp/y.training: split_training_set
tmp/z.training: split_training_set

split_training_set: tmp/training-set.log
	./split-efs-log.py tmp/training-set.log tmp/x.training tmp/y.training tmp/z.training

tmp/x.training: split_training_set
tmp/y.training: split_training_set
tmp/z.training: split_training_set

# Scale the training sets, saving the scaling parameters
tmp/x.training.scaled: tmp/x.training
	svm-scale -s tmp/x.scale tmp/x.training > tmp/x.training.scaled
tmp/y.training.scaled: tmp/y.training
	svm-scale -s tmp/y.scale tmp/y.training > tmp/y.training.scaled
tmp/z.training.scaled: tmp/z.training
	svm-scale -s tmp/z.scale tmp/z.training > tmp/z.training.scaled

# Train the SVMs
x.model: tmp/x.training
	svm-train $(SVMFLAGS) tmp/x.training x.model
y.model: tmp/y.training
	svm-train $(SVMFLAGS) tmp/y.training y.model
z.model: tmp/z.training
	svm-train $(SVMFLAGS) tmp/z.training z.model

