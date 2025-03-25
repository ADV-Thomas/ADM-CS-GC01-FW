[![](images/advantics_logo.png)](http://advantics.fr)

# CAN Databases

This repository groups all files related to CAN databases for Advantics projects.

- .KCD : license free, XML-formatted, [Kayak](https://github.com/dschanoeh/Kayak) CAN definition files (the actual CAN databases)
- .DBC : proprietary format that describes the data transmitted over CAN bus
- [.XSD](https://en.wikipedia.org/wiki/XML_Schema_(W3C)) : XML schema definition files, to to verify the validity of of the .KCD files

Next, the repository also contains code and tools to test the validity of the listed CAN databases, and to convert CAN databases.

:bangbang: *When adding a new database to this repository, please also update the test-section in the [continuous integration script](.appveyor/appveyor.yml)* :bangbang:


## Tools & source code

### convert.py

This script converts a CAN-database from one format to another. It takes two arguments (the database to convert, the name of the converted database). The extension of the second argument will define the output-format. The used [cantools](https://cantools.readthedocs.io/en/latest/) Python library currently supports `.arxml`, `.dbc`, `.kcd`, `.sym` and `.cdd` extensions/formats.

### validate.py

This script is called by the [continuous integration test-section](.appveyor/appveyor.yml) to validate all configured .KCD files against the [KCD_Definition.xsd XML-schema file](KCD_Definition.xsd), and against the [cantools-library](https://pypi.org/project/cantools/).

### validate_standalone.py

This standalone tool validates all .KCD files in the repository against the [KCD_Definition.xsd XML-schema file](KCD_Definition.xsd), and against the [cantools-library](https://pypi.org/project/cantools/).

Default usage :
```bash
$ python validate_standalone.py
```
By default, the output will only show if the validation was successful.  
For more info on the reason in case of a validation failure, the `--debug` options needs to be activated :
```bash
$ python validate_standalone.py --debug
```
An `--xsdschema` option is available to validate against another XML-schema then the one included in the repository :
```bash
$ python validate_standalone.py --xsdschema [path]/[yourxmlschemafile].xsd
```
The above information can also be obtained directly from the script :
```bash
$ python validate_standalone.py --help
```
```text
usage: validate_standalone.py [-h] [-x XSDSCHEMA] [-d]

Standalone utility to validate .kcd CAN-databases against an XSD schema and
cantools.

optional arguments:
  -h, --help                            show this help message and exit
  -x XSDSCHEMA, --xsdschema XSDSCHEMA   XSD schema to validate against
  -d, --debug                           activate full feedback for debugging
```

## CAN Databases

<!--do not make any manual modifications in the generated-list-section-->

<!--start_generated_list--> 

database name | status 
-|- 
AC_v1.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
ADM_PC_BC25.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
ADM_PC_BI25.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
ADM_PC_BP25.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
ADM_PC_LF45.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
ADM_PC_LL25.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
ADM_PC_UP25.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Advantics_Bootloader.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Advantics_DLOG.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Advantics_Generic_EVSE_protocol_v1.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Advantics_Generic_EVSE_protocol_v2.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Advantics_Generic_PEV_protocol_v1.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Advantics_LOGGING.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Advantics_SFRA_F.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Advantics_VCU.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Advantics_VCU_Testbench.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
CHAdeMO.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Maxwell_MXR100040.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
PYMCO_BMS.kcd | ![](https://img.shields.io/badge/valid-green.svg) 
Sendyne_SFP200.kcd | ![](https://img.shields.io/badge/valid-green.svg) 

_Table last updated :_ `2020-11-01 01:15`

<!--end_generated_list-->

_(The validation procedure runs daily. The table is only updated in case of changes)_
