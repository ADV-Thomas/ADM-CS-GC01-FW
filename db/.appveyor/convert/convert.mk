CONVERT?=canconvert
MKDIR_P=mkdir -p
OUTDIR?=./

# This is the list of databases to convert
DATABASES = ADM_PC_BC25 ADM_PC_BI25 ADM_PC_BP25 ADM_PC_LF45 ADM_PC_LL25 ADM_PC_UP25 Advantics_Generic_EVSE_protocol_v1 Advantics_Generic_EVSE_protocol_v2 Advantics_Generic_EVSE_protocol_v3 Advantics_Generic_PEV_protocol_v1 Advantics_Generic_PEV_protocol_v2 Advantics_Bootloader AC_v1

# Input format (put them with leading .)
IN_FORMAT = .kcd

# This is the list of formats (put them with leading .)
OUT_FORMATS = .dbc .kcd

# This will build list of all targets (i.e. the permutations of DATABASES x OUT_FORMATS)
TARGETS = $(foreach db, $(DATABASES), $(addprefix $(OUTDIR)/, $(addprefix $(db), $(OUT_FORMATS))))

all: directory $(TARGETS)

$(TARGETS):
	$(CONVERT) $(realpath $(notdir $(basename $@))$(IN_FORMAT)) $@

.PHONY: directory

directory: $(OUTDIR)

$(OUTDIR):
	$(MKDIR_P) $(OUTDIR)
