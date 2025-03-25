#!/bin/bash

# Key for accessing other github repos
echo "-----BEGIN OPENSSH PRIVATE KEY-----" > $HOME/.ssh/id_ed25519
echo "${priv_key}" | tr " " "\n" >> $HOME/.ssh/id_ed25519
echo "-----END OPENSSH PRIVATE KEY-----" >> $HOME/.ssh/id_ed25519
chmod 600 $HOME/.ssh/id_ed25519
echo -e "Host=github.com\nHostname=github.com\nUser=git\nStrictHostKeyChecking no\n" >> $HOME/.ssh/config
eval `ssh-agent`
ssh-add $HOME/.ssh/id_ed25519

# Appveyor tools
cd $APPVEYOR_BUILD_FOLDER
git clone --recurse-submodules --branch appveyor-tools --depth 1 --single-branch --no-tags git@github.com:ADVANTICS/etka-template.git .appveyor/tools
