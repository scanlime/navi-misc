#
# Makefile for training the field sensor.
# An efs-1.log file produced by the 'tracker' app in navi-misc's
# vision module comes in, gets processed by libsvm and included
# scripts, and an SVM model comes out.
#

# Amount of the data to reserve as a test set, between 0 and 1
TEST_RATIO = 0.2

# Input file, in the tracker log format
INPUT_FILE = efs-1.log

#######################################################################

# Shuffle the input into test set and training set
shuffle_log: $(INPUT_FILE)
	./shuffle-efs-log.py $(INPUT_FILE) $(TEST_RATIO) tmp/test-set.log tmp/training-set.log

tmp/test-set.log: shuffle_log
tmp/training-set.log: shuffle_log

# Split the test and training sets into files for each output variable
split_test_set: tmp/test-set.log
	./split-efs-log.py tmp/test-set.log tmp/x.test tmp/y.test tmp/z.test

tmp/x.test: split_test_set
tmp/y.test: split_test_set
tmp/z.test: split_test_set

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

# Scale the test sets, using the saved scale parameters
tmp/x.test.scaled: tmp/x.test tmp/x.training.scaled
	svm-scale -r tmp/x.scale tmp/x.test > tmp/x.test.scaled
tmp/y.test.scaled: tmp/y.test tmp/y.training.scaled
	svm-scale -r tmp/y.scale tmp/y.test > tmp/y.test.scaled
tmp/z.test.scaled: tmp/z.test tmp/z.training.scaled
	svm-scale -r tmp/z.scale tmp/z.test > tmp/z.test.scaled

