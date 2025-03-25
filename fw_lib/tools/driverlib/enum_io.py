import sys, os, re

# Check number of input arguments
if len(sys.argv) != 2:
    sys.exit('Incorrect number if input arguments ({0})'.format(len(sys.argv)-1))

# Get input arguments
cpu = sys.argv[1].lower()

# Check input argument
if cpu != 'f2803x' and cpu != 'f28004x' and cpu != 'f28p65x' :
    sys.exit('CPU type {0} is not supported'.format(cpu))

# Read file
f = open(os.path.join('..', '..', 'code', 'inc', 'drv', 'io.h'), 'r')
lines = f.readlines()
f.close()

FUN = {}

inEnum = False

for line in lines:
    if line.startswith('enum io_fun {'):
        inEnum = True
    elif line.startswith('};'):
        inEnum = False
    elif inEnum:
        res = re.search(r'\s+IO_(.+?)\s+=\s+0x(.+?)U', line)
        if res is None:
            sys.exit('Unexpected line format:\n{0}'.format(line))
        fun = res.group(1)
        code = int(res.group(2), 16)
        if fun in FUN:
            sys.exit('Function already defined:\n{0}'.format(line))
        FUN[fun] = code

# Read file
f = open(cpu + '.txt', 'r')
lines = f.readlines()
f.close()

mux = [ 0, 1, 2, 3 ]
an = [ 98, 100, 102, 106, 108, 110 ]
if cpu == 'f28004x':
    mux = [ 0, 1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15 ]
    an = range(224, 248)
if cpu == 'f28p65x':
    mux = [ 0, 1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15,0 ]
    an = range(225, 242)

MAP = {}

MAP['IOX'] = 0

for line in lines:
    if not line.startswith('GPIO'):
        sys.exit('Unexpected line format:\n{0}'.format(line))
    res = re.search(r'GPIO(\d+).', line)
    num = int(res.group(1), 10)
    code = 0xC0000000 | num
    io = 'IO{0}'.format(num)
    funs = line.split()
    if len(funs) != len(mux):
        sys.exit('Unexpected line format:\n{0}'.format(line))
    for i, fun in enumerate(funs):
        if 'AIN' in fun:
            MAP[io + '_AIN'] = code | (FUN['AIN'] << 12)
        for f in fun.split('#'):
            if f == 'RESERVED' or f == 'AIN':
                continue
            elif f.startswith('GPIO'):
                MAP[io + '_DIN'] = code | (FUN['DIN'] << 12)
                MAP[io + '_DOUT'] = code | (FUN['DOUT'] << 12)
            else:
                key = io + '_' + f
                if key in MAP:
                    continue
                    sys.exit('Function already defined: {0}\n{1}'.format(f, line))
                elif f not in FUN:
                    continue
                    sys.exit('Unknown function: {0}\n{1}'.format(f, line))
                MAP[key] = code | (FUN[f] << 12) | (mux[i] << 8)

for num in an:
    code = 0xC0000000 | num
    io = 'IO{0}'.format(num)
    if cpu == 'f2803x':
        MAP[io + '_AIN'] = code | (FUN['AIN'] << 12)
        MAP[io + '_AOUT'] = code | (FUN['AOUT'] << 12)
        MAP[io + '_DIN'] = code | (FUN['DIN'] << 12)
        MAP[io + '_DOUT'] = code | (FUN['DOUT'] << 12)
    elif cpu == 'f28004x':
        MAP[io + '_AIN'] = code | (FUN['AIN'] << 12)
        MAP[io + '_DIN'] = code | (FUN['DIN'] << 12)
    elif cpu == 'f28p65x':
        MAP[io + '_AIN'] = code | (FUN['AIN'] << 12)
        MAP[io + '_DIN'] = code | (FUN['DIN'] << 12)

fun0 = None

print('enum io {')
for fun in MAP:
    fun00 = fun.split('_')[0]
    if fun0 is not None and fun00 != fun0:
        print()
    fun0 = fun00
    c = ',' if fun != list(MAP)[-1] else ''
    print('\t{0:<20} = 0x{1:08X}U{2}'.format(fun, MAP[fun], c))
print('};')