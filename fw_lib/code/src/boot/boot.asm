;   CLEAN UP AND JUMP TO BOOT ENTRY POINT
;   For COFF ABI, the compiler prepends an underscore to the beginning of all C/C++
;   identifiers. Hence, the symbol _boot_exit is used as boot_exit in C program.
;   See pp.181 in SPRU513R for more information.
    .def _boot_exit ; Symbol defined in current file. May be used in other files.
    .sect ".TI.ramfunc" ; Assemble into the .TI.ramfunc section.

_boot_exit:

;   Deallocate stack
__stack:    .usect ".stack", 0
    MOV SP,#__stack ; Set SP to the beginning of .stack section.

;   Push two 16-bit zeros to stack.
;   This will end up in RPC after setting PC in LRETR.
    PUSH #0x0       ; ([SP]=0x0000, SP+=1)
    PUSH #0x0       ; ([SP]=0x0000, SP+=1)

;   Input to the function, ie boot entry point, is saved in the 32-bit ACC register.
;   Save boot entry point given at input to RPC.
    PUSH ACC        ; ([SP]=ACC, SP+=2)
    POP  RPC        ; RPC=ACC (SP-=2, RPC=[SP])

;   Reset CPU registers (ACC, P, OVC, XT, DP, XARn).
;   See pp.23-30 in SPRU430F for more information.
    ZAPA            ;  ACC=0, P=0, OVC=0
    MOVL XT,ACC     ;   XT=0
    MOVW DP,#0x0    ;   DP=0
    MOVZ AR0,AL     ; XAR0=0
    MOVZ AR1,AL     ; XAR1=0
    MOVZ AR2,AL     ; XAR2=0
    MOVZ AR3,AL     ; XAR3=0
    MOVZ AR4,AL     ; XAR4=0
    MOVZ AR5,AL     ; XAR5=0
    MOVZ AR6,AL     ; XAR6=0
    MOVZ AR7,AL     ; XAR7=0

;   Restore CPU status registers ST0 and ST1 to default values. OBJMODE in ST1 is the only one
;   not restored to default value for C28x object mode. See pp.23,30-44 in SPRU430F.
;   ST0 = 0x0000    ST1 = 0x0A0B instead of 0x080B!
;   15:10 OVC=0     15:13      ARP=0
;    9: 7  PM=0        12       XF=0
;       6   V=0        11  MOM1MAP=1
;       5   N=0        10 RESERVED=0
;       4   Z=0         9  OBJMODE=1 (C28x object mode!)
;       3   C=0         8    AMODE=0
;       2  TC=0         7 IDLESTAT=0
;       1 OVM=0         6   EALLOW=0
;       0 SXM=0         5     LOOP=0
;                       4      SPA=0
;                       3     VMAP=1
;                       2    PAGE0=0
;                       1     DBGM=1
;                       0     INTM=1
    PUSH #0x0000    ; ([SP]=0x0000, SP+=1)
    PUSH #0x0A0B    ; ([SP]=0x0A0B, SP+=1)
    POP  ST1        ; ST1=0x0A0B (SP-=1, ST1=[SP])
    POP  ST0        ; ST0=0x0000 (SP-=1, ST0=[SP])

;   Jump to boot entry point saved in RPC register.
;   After setting PC to RPC, RPC is loaded from stack.
    LRETR           ; PC=RPC, RPC=0 (PC=RPC, SP-=2, RPC=[SP])
