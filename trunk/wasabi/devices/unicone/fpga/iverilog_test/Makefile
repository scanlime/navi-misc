#
# Makefile for synthesizing Verilog designs into Xilinx FPGA
# bitstreams, using Icarus Verilog and running the Xilinx
# backend tools under Wine.
#
# -- Micah Dowty <micah@navi.cx>
#


TOP_LEVEL = test.v
CONSTRAINTS = test.ucf
DESIGN_NAME = test
PART_NAME = 4010XLPC84-09

VFLAGS = -tfpga

# Xilinx directories. These are defaults appropriate for Xilinx
# Foundation installed within a default Wine setup.
XILINX = C:/Fndtn
XILINX_BIN = ~/.wine/fake_windows/Fndtn/bin/nt
WINE = XILINX=$(XILINX) wine --

all: $(DESIGN_NAME).ngd

# Synthesize the top-level verilog file into an EDIF netlist,
$(DESIGN_NAME).edf: $(TOP_LEVEL)
	iverilog $(VFLAGS) -o $@ $<

# Convert EDIF netlists to Xilinx's NGO binary netlist format
$(DESIGN_NAME).ngo: $(DESIGN_NAME).edf
	$(WINE) $(XILINX_BIN)/edif2ngd.exe $<

# Translate and merge the NGO netlist with a UCF constraints
# file to yield an NGD design database file.
$(DESIGN_NAME).ngd: $(DESIGN_NAME).ngo $(CONSTRAINTS)
	$(WINE) $(XILINX_BIN)/ngdbuild.exe $< -uc $(CONSTRAINTS) -p $(PART_NAME)


clean:
	rm -f *.edf *.ngd *.ngo *.lst *.bld