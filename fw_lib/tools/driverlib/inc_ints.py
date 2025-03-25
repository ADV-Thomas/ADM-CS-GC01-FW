import sys, os, re

# Build path to directory with header files
fpath = os.path.join('..', '..', 'c2000', 'driverlib', 'f28003x', 'driverlib', 'inc', 'hw_ints.h')

##
## PROCESS HEADER FILE
##

# Read header file
f = open(fpath, 'r')
lines = f.readlines()
f.close()

INT = [ [None] * 8 for i in range(13) ]
INT[0] = [None] * 32

# Parse header file
for line in lines:
    res = re.search(r'#define\s+INT_(.*?)\s+(.*?)UL\s+', line)
    if res is None:
        continue
    fun = res.group(1)
    code = int(res.group(2), 0)
    # Decode
    x = (code >> 8) & 0xFF
    y = (code >> 0) & 0xFF
    vid = (code >> 16) & 0xFFFF
    # Populate table
    if x > 0 and y > 0:
        col = y-1
        if vid != (((x-1+4) * 8) + (y-1)):
            sys.exit('Invalid vector identifier:\n{0}'.format(line))
    elif x == 0 and y == 0:
        col = vid
    else:
        sys.exit('Invalid X and Y parameters:\n{0}'.format(line))
    if INT[x][col] is None:
        INT[x][col] = '{0:<10}'.format(fun)
    else:
        sys.exit('Interrupt signal is already initialized:\n{0}'.format(line))

# Replace None with Reserved
for x, l in enumerate(INT):
    if x == 0:
        continue
    for y, f in enumerate(INT[x]):
        if INT[x][y] is None:
            INT[x][y] = '{0:<10}'.format('Reserved')

##
## PRINT INTERRUPT SIGNALS
##

for x, l in enumerate(INT):
    if x == 0:
        for y, fun in enumerate(l):
            if fun is not None:
                print('{0:<3}{1}'.format(y, fun))
        print()
    else:
        print('{0:<3}{1}'.format(
            x, ''.join(reversed(INT[x]))
        ))
