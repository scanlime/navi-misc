#
# Defines the available background images.
# This always includes the default backgrounds,
# but also looks for backgrounds on navi.
#

# The default background, in our data directory
list.append(dataFile('brushed_metal.png'))

# Directory to look for backgrounds on navi in
bgDir = "/navi/media/images/backgrounds"
for pattern in ('*.jpg', '*.jpeg', '*.png'):
    list.extend(glob(os.path.join(bgDir, pattern)))
