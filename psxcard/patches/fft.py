#
# Random utilities for poking at Final Fantasy Tactics save files.
# This file can also be used as a stand-alone patch which fixes
# broken FFT saves.
#

FRAME_SIZE = 128
PARITY_BLOCK_OFFSET = 0x118
TOTAL_FRAME_COUNT = 60

def frameParity(savedata, frameNumber):
    if frameNumber >= TOTAL_FRAME_COUNT:
	# These frames aren't used. Their parity bit is always 1.
	return 1

    if frameNumber == PARITY_BLOCK_OFFSET // FRAME_SIZE:
        # This is the frame containing the parity data itself.
        # FFT itself doesn't seem to calculate this bit in any reliable
        # way that I've been able to figure out- it's probably just using
        # the previous parity data values. In any case, the game seems
        # to ignore this bit's value on load, so we can just return
        # anything...

	return 0

    # A normal frame... Calculate a giant parity bit by XOR'ing
    # zero with each bit in the frame.

    byte = 0
    for offset in range(FRAME_SIZE):
        byte ^= data[frameNumber * FRAME_SIZE + offset]
    parity = 0
    for bit in range(8):
        if byte & (1<<bit):
            parity ^= 1
    return parity

def calculateParityBlock(savedata):
    """FFT save files include a 64-bit block which includes 1-bit parity values
       for every 128-byte memory card frame. Note the few exceptions to this rule,
       handled by frameParity().
       """
    bytes = []
    for offset in range(8):
        byte = 0
        for bit in range(8):
            byte |= frameParity(savedata, offset * 8 + bit) << (7 - bit)
        bytes.append(chr(byte))
    return ''.join(bytes)

def updateParityBlock(savedata):
    savedata[PARITY_BLOCK_OFFSET] = calculateParityBlock(savedata)

if __name__ == "__main__":
    import patchlib
    data = patchlib.BinaryPatcher().load()
    updateParityBlock(data)

