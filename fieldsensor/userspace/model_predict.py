#!/usr/bin/env python
import svm, efs, time

models = [svm.svm_model("../training/x.model"),
          svm.svm_model("../training/y.model"),
          svm.svm_model("../training/z.model")]

sensor = efs.FieldSensor()
sensor.initScan()

while 1:
    reading = sensor.readAverages()
    vector = [model.predict(reading) for model in models]
    print vector
    time.sleep(0.1)
