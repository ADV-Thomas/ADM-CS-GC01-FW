import sys, os, re

# Build path to header file
fpath = os.path.join('..', '..', 'c2000', 'driverlib', 'f2803x', 'driverlib', 'sysctl.h')

# This is specific for f2803x
PCLKCR = { 12:'PCLKCR0', 13:'PCLKCR1', 9:'PCLKCR2', 16:'PCLKCR3' }
baseReg = 0x7010

# Read file with pin map
f = open(fpath, 'r')
l = f.readlines()
f.close()

CLK = {}

for line in l:
    res = re.search(r'\s+SYSCTL_PERIPH_CLK_(.*?)U[,]?\s+', line)
    if res is None:
        continue
    t = re.split(r'\s*=\s*', res.group(1))
    if len(t) != 2:
        continue
    # Convert string to number, automatically choose base
    num = int(t[1], 0)
    # Get peripheral clock label
    lab = t[0]
    # Decode
    offset = num & 0x001F
    index = (num & 0x0F00) >> 8
    # Save to dictionary
    if offset not in CLK:
        CLK[offset] = [None] * 16
    if CLK[offset][index] is not None:
        sys.exit('Peripheral clock {0} already initialized'.format(lab))
    else:
        CLK[offset][index] = lab

for offset in CLK:
    print('{0} @ 0x{1:04X} (1)'.format(PCLKCR[offset], offset + baseReg))
    C = CLK[offset]
    i = len(C) - 1
    while i >= 0:
        j = i - 1
        while j >= 0:
            if C[i] != C[j]:
                break
            j = j - 1
        j = j + 1
        ran = '{0}'.format(i) if i == j else '{0}:{1}'.format(i, j)
        print('{0: <5} {1}'.format(ran, 'Reserved' if C[i] is None else C[i]))
        i = j - 1
    print()