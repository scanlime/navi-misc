import trackd

def getPosition(tracker, device):
    result = trackd.new_floatArray(3)
    trackd.trackdGetPosition(tracker, device, result)
    pos = (trackd.floatArray_getitem(result, 0), trackd.floatArray_getitem(result, 1), trackd.floatArray_getitem(result, 2))
    trackd.delete_floatArray(result)
    return pos

def getEulerAngles(tracker, device):
    result = trackd.new_floatArray(3)
    trackd.trackdGetEulerAngles(tracker, device, result)
    orn = (trackd.floatArray_getitem(result, 0), trackd.floatArray_getitem(result, 1), trackd.floatArray_getitem(result, 2))
    trackd.delete_floatArray(result)
    return orn
