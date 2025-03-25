# Differences between f2803x and f28004x driverlibs

*Marko Gulin, 8 Mar 2021*

Drivers for f2803x are custom-designed following the same design principles from f28004x driverlib. The f2803x driverlib is not complete, *i.e.*, it includes only what is necessary for Advantics power converters. Texas Instruments does not plan to provide driverlib for their older microcontrollers. Follow [this link](https://e2e.ti.com/support/microcontrollers/c2000/f/171/t/978800 "Visit web site") for more information. This note gives differences between f2803x and f28004x driverlib implementations.

## CPU

There are no differences.

## CPUTIMER

There are no differences.

## DEBUG

There are no differences.

## FLASH

The entire implementation is different.

## GPIO

The following functions do not exist in f28004x driverlib:
- GPIO_isPinAnalog()

The following functions do not exist in f2803x driverlib:
- GPIO_setInterruptType()
- GPIO_getInterruptType()
- GPIO_enableInterrupt()
- GPIO_disableInterrupt()
- GPIO_getInterruptCounter()
- GPIO_lockPortConfig()
- GPIO_unlockPortConfig()
- GPIO_commitPortConfig()
- GPIO_setInterruptPin()
- GPIO_setQualificationPeriod()
- GPIO_setMasterCore()

## I2C

There are no differences.

## SYSCTL

The following functions do not exist in f28004x driverlib:
- SysCtl_setInternalOscMode()
- SysCtl_enablePLL()
- SysCtl_disablePLL()
- SysCtl_setPLLMultiplier()

The following functions do not exist in f2803x driverlib:
- SysCtl_resetPeripheral()
- SysCtl_getResetCause()
- SysCtl_clearResetCause()
- SysCtl_selectClockOutSource()
- SysCtl_getExternalOscCounterValue()
- SysCtl_clearExternalOscCounterValue()
- SysCtl_enterIdleMode()
- SysCtl_enterHaltMode()
- SysCtl_enableLPMWakeupPin()
- SysCtl_disableLPMWakeupPin()
- SysCtl_setWatchdogMode()
- SysCtl_isWatchdogInterruptActive()
- SysCtl_setWatchdogPredivider()
- SysCtl_setWatchdogWindowValue()
- SysCtl_enableNMIGlobalInterrupt()
- SysCtl_getNMIStatus()
- SysCtl_getNMIFlagStatus()
- SysCtl_isNMIFlagSet()
- SysCtl_clearNMIStatus()
- SysCtl_clearAllNMIFlags()
- SysCtl_forceNMIFlags()
- SysCtl_getNMIWatchdogCounter()
- SysCtl_setNMIWatchdogPeriod()
- SysCtl_getNMIWatchdogPeriod()
- SysCtl_getNMIShadowFlagStatus()
- SysCtl_isNMIShadowFlagSet()
- SysCtl_lockAccessControlRegs()
- SysCtl_setPeripheralAccessControl()
- SysCtl_getPeripheralAccessControl()
- SysCtl_setSyncInputConfig()
- SysCtl_setSyncOutputConfig()
- SysCtl_enableExtADCSOCSource()
- SysCtl_disableExtADCSOCSource()
- SysCtl_lockExtADCSOCSelect()
- SysCtl_lockSyncSelect()
- SysCtl_getEfuseError()
- SysCtl_getPIEVErrAddr()
- SysCtl_issPLLValid()
- SysCtl_selectXTAL()
- SysCtl_selectXTALSingleEnded()