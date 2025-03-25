import sys, os, re

# Check number of input arguments
if len(sys.argv) != 2:
    sys.exit('Incorrect number if input arguments ({0})'.format(len(sys.argv)-1))

# Get input arguments
module = sys.argv[1].lower()

# Build path to directory with header files
fdir = os.path.join('..', '..', 'c2000', 'driverlib', 'f28p65x', 'driverlib', 'inc')

##
## PROCESS HEADER FILE WITH REGISTER BASE ADDRESS
##

# Read header file
f = open(os.path.join(fdir, 'hw_memmap.h'), 'r')
lines = f.readlines()
f.close()

BASE = {}

# Parse header file
for line in lines:
    res = re.search(r'#define\s+(.*?)\s+(.*?)UL\s+', line)
    if res is None or '_BASE' not in line:
        continue
    BASE[res.group(1)] = int(res.group(2), 0)

##
## PROCESS HEADER FILE WITH MODULE DATA
##

# Read header file
f = open(os.path.join(fdir, 'hw_' + module + '.h'), 'r')
lines = f.readlines()
f.close()

REGS = {}
base = None

for n, line in enumerate(lines):
    # Build line with line number
    lerr = '\n[{0}] {1}'.format(n+1, line)
    # Load register base address
    if 'BASE REGISTER' in line and '_BASE' in line:
        res = re.search(r'.*BASE REGISTER\s+(.*?)\s+', line)
        base = re.split(',', res.group(1))
        for i, b in enumerate(base):
            if b not in BASE:
                sys.exit('Unknown base register: {0}'.format(b))
            else:
                base[i] = BASE[b]
        continue
    # Parse line
    res = re.search(rf'#define[ \t]+{module.upper()}_(.*?)[ \t]+(.*?)[U \t]*[\/\n]', line)
    if res is None:
        continue
    # Split register string
    #   MOD_O_REG       -> [ O, REG ]       -> num is REG offset
    #   MOD_S_REG       -> [ S, REG ]       -> num is REG size
    #   MOD_REG_FLD_S   -> [ REG, FLD, S ]  -> num is REG[FLD] shift
    #   MOD_REG_FLD_M   -> [ REG, FLD, M ]  -> num is REG[FLD] mask
    #   MOD_REG_BIT     -> [ REG, BIT ]     -> num is REG[BIT] mask
    t = re.split(r'_', res.group(1))
    # Numeric value, auto choose base
    try:
        num = int(res.group(2), 0)
    except ValueError:
        sys.exit('Could not parse number:{0}'.format(lerr))
    # Process line
    if len(t) < 2 or len(t) > 3:
        sys.exit('Unexpected line format:{0}'.format(lerr))
    if t[0] == 'O':
        if t[1] in REGS:
            sys.exit('Register offset already defined:{0}'.format(lerr))
        elif base is None:
            sys.exit('Base register not initialized:{0}'.format(lerr))
        adr = base.copy()
        for i, a in enumerate(adr):
            adr[i] = adr[i] + num
        # Create new register entry
        REGS[t[1]] = {
            'adr':adr,
            'size':0,
            'bits':None,
            'lock':'*E* */' in line,
            'shift':{}
        }
    elif t[0] == 'S':
        if t[1] not in REGS:
            sys.exit('Register size specified before offset:{0}'.format(lerr))
        elif REGS[t[1]]['size'] > 0:
            sys.exit('Register size already specified:{0}'.format(lerr))
        # Initialize register size
        REGS[t[1]]['size'] = num
        REGS[t[1]]['bits'] = [None] * (num*16)
    elif t[0] not in REGS or REGS[t[0]]['size'] == 0:
        sys.exit('Register offset or size not initialized:{0}'.format(lerr))
    elif len(t) == 2:
        if t[1] in REGS[t[0]]['bits']:
            sys.exit('Bit name already exists for this register:{0}'.format(lerr))
        elif bin(num).count('1') != 1:
            sys.exit('Invalid bit mask:{0}'.format(lerr))
        ind = bin(num)[::-1].find('1')
        if REGS[t[0]]['bits'][ind] is not None:
            sys.exit('Bit field already initialized:{0}'.format(lerr))
        REGS[t[0]]['bits'][ind] = t[1]
    elif t[2] == 'S':
        if t[1] in REGS[t[0]]['shift']:
            sys.exit('Bit field shift already initialized:{0}'.format(lerr))
        REGS[t[0]]['shift'][t[1]] = num
    elif t[2] == 'M':
        if t[1] not in REGS[t[0]]['shift']:
            sys.exit('Bit field shift not initialized:{0}'.format(lerr))
        s = REGS[t[0]]['shift'][t[1]]
        c = bin(num).count('1')
        mask = (2**c - 1) * 2**s
        if mask == 0 or mask != num:
            sys.exit('Mask and shift combination do not match:{0}'.format(lerr))
        for i, b in enumerate(bin(num)[::-1]):
            if b == '1' and REGS[t[0]]['bits'][i] is not None:
                sys.exit('Bit field already initialized:{0}'.format(lerr))
            elif b == '1':
                REGS[t[0]]['bits'][i] = t[1]
    else:
        sys.exit('Unexpected line format:{0}'.format(lerr))

for reg in REGS.keys():
    print('{0} @ {2} ({1}) {3}'.format(
        reg, REGS[reg]['size'],
        ','.join('0x{:04X}'.format(x) for x in REGS[reg]['adr']),
        'EALLOW' if REGS[reg]['lock'] else ''
    ))
    bits = REGS[reg]['bits']
    i = len(bits) - 1
    while i >= 0:
        j = i - 1
        while j >= 0:
            if bits[i] != bits[j]:
                break
            j = j - 1
        j = j + 1
        ran = '{0}'.format(i) if i == j else '{0}:{1}'.format(i, j)
        print('{0: <5} {1}'.format(ran, 'Reserved' if bits[i] is None else bits[i]))
        i = j - 1
    print()