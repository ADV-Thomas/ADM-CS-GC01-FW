#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################
#  Standalone script to validate all .kcd CAN database files
#   against the KCD definition XSD schema, and against cantools
#  Copyright Advantics 2020
#  07-02-2020 : initial script
################################################################

# System imports
import os
import sys
import argparse

# Third-party imports
import xmlschema
import cantools


# helper class to format output in the console
class formatting:
    # colors
    PURPLE = '\033[95m'
    CYAN = '\033[96m'
    DARKCYAN = '\033[36m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    # styles
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    BLINK = '\033[5m'
    REVERSE = '\033[7m'
    CONCEALED = '\033[8m'

    # all attributes off
    END = '\033[0m'

    # clear screen, and move cursor to home position
    CLEARSCREEN = '\033[2J'


def validate_CAN_databases(args):
    DEBUG = args.debug
    # get all .kcd files in the current directory (by default, root of the repo)
    for filename in os.listdir(os.getcwd()):
        if filename.endswith(".kcd"):
            try:
                with open(filename, 'r') as f:
                    kcd_file = f.read()
                if DEBUG:
                    print('Validating {0} against XSD schema...'.format(filename))
                schema = xmlschema.XMLSchema(args.xsdschema)
                schema.validate(kcd_file)
                if DEBUG:
                    print('XML validation against XSD schema successful for {0}'.format(filename))
                    print('Validating {0} against cantools...'.format(filename))
                cantools.db.load_file(filename)
                print(formatting.GREEN + '{0} successfully validated'.format(
                    filename) + formatting.END)
            except Exception:
                print(formatting.RED + 'Validation failed for {0}'.format(
                    filename) + formatting.END)
                if DEBUG:
                    for info in sys.exc_info():
                        print(info)


def _main():
    parser = argparse.ArgumentParser(
        description='Standalone utility to validate .kcd CAN DBs against an XSD schema and cantools'
    )
    parser.add_argument(
        '-x', '--xsdschema', type=str, help='XSD schema to validate against',
        default='KCD_Definition.xsd'
    )
    parser.add_argument(
        '-d', '--debug', help='activate full feedback for debugging', action='store_true')
    args = parser.parse_args()
    validate_CAN_databases(args)
    print(formatting.YELLOW + 'Done!')
    print(formatting.END)


if __name__ == "__main__":
    _main()
