
OBJ
  prox[2] : "OpticalProximity"
  vcm[2] : "VoiceCoilServo"
  vm : "VectorMachine"
  bt : "BluetoothConduit"
  
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  VECTOR_MEM_SIZE = 4096
  
VAR
  long  vector_mem[VECTOR_MEM_SIZE]

DAT

vector_desc   long      VECTOR_MEM_SIZE
              byte      "vector_mem", 0      

vm_cmd_desc   long      1
              byte      "vm_cmd", 0

vm_out_desc   long      2
              byte      "vm_output", 0

pos_x_desc    long      1
              byte      "pos_x", 0

pos_y_desc    long      1
              byte      "pos_y", 0

params_x_desc long      vcm#NUM_PARAMS
              byte      "params_x", 0                 

params_y_desc long      vcm#NUM_PARAMS
              byte      "params_y", 0                 
              
  
PUB main | okay, syncTime

  okay := bt.start(20, 21, 22, 23, string("Test Device"))
  if not okay
    repeat

  bt.defineRegion(@vector_desc, @vector_mem)
  bt.defineRegion(@vm_cmd_desc, vm.getCmdAddress)
  bt.defineRegion(@vm_out_desc, vm.getOutputAddress(0))
  bt.defineRegion(@pos_x_desc, prox[0].getOutputAddress)
  bt.defineRegion(@pos_y_desc, prox[1].getOutputAddress)
  bt.defineRegion(@params_x_desc, vcm[0].getParams)
  bt.defineRegion(@params_y_desc, vcm[1].getParams)

  syncTime := cnt + clkfreq / 100

  vm.start(syncTime, vcm#LOOP_HZ, 9, @vector_mem, VECTOR_MEM_SIZE)
  prox[0].start(4, 5)
  prox[1].start(16, 17)
  vcm[0].start(6, 8, syncTime, prox[0].getOutputAddress, vm.getOutputAddress(0))
  vcm[1].start(2, 0, syncTime, prox[1].getOutputAddress, vm.getOutputAddress(1))

  repeat
  