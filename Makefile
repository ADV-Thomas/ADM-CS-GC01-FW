CPU_FAMILY = f28p65x
#TODO F28P65X
#MAKE_CMD = $(MAKE) --directory=fw_lib/build CPU_FAMILY="$(CPU_FAMILY)"
MAKE_CMD = $(MAKE) --silent --directory=fw_lib/build CPU_FAMILY="$(CPU_FAMILY)"

.PHONY: all clean artifacts

all:
#TODO TODO F28P65X
	$(MAKE_CMD)  NO_BOOTLOADER=ON all 
clean:
	$(MAKE_CMD) clean

artifacts:
	$(MAKE_CMD) artifacts