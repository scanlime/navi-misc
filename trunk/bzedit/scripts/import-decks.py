import bzflag

# FIXME!!!! make it find this in the right dir!
reader = bzflag.WorldReader('decks')
for object in reader.readObjects():
    print object
    if type(object) == bzflag.Box or type(object) == bzflag.Pyramid:
        object.toBlender()
