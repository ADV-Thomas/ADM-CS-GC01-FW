#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright Advantics 2019
# Original author: Axel Voitier
# Part of ETKA - Engineering ToolKit for Advantics

# System imports

# Third-party imports
import begin
import cantools
import xmlschema

# Local imports


@begin.start
def main(kcd_file):
    print('Validating...')
    schema = xmlschema.XMLSchema('KCD_Definition.xsd')
    schema.validate(kcd_file)
    print('XML Validation successful')

    print()
    print('Trying it in cantools...')
    cantools.db.load_file(kcd_file)
    print('Sucess!')
