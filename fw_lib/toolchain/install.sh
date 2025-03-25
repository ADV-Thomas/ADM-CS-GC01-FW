#!/bin/bash

aCOLOUR=(
			'\e[93m'	# Yellow		| Update notifications
			'\e[92m'	# Green		| good
            
)
COLOUR_RESET='\e[0m'

#FILE="ti_cgt_c2000_22.6.0.LTS_linux-x64_installer.bin"
content=$(wget --timeout=1 https://www.ti.com/tool/download/C2000-CGT -q -O -)
#echo ${content}
content2=$( echo "$content" | grep -A 10 'Latest version')
#echo ${content2}
lastest_version=$( echo "$content2" | grep -A 1 'Version:' | grep -o '>[^<]*<' | grep -o '[^<>]*'  | awk '{print $2}' )
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
echo  "DIR=" ${DIR}
FILE=$(ls ${DIR} | grep 'c2000' | sort -r | head -1 )
#check if file is the lastest version 
echo "FILE=" ${FILE}

if [[ "$lastest_version" == "" ]]; then 
 echo -e "${aCOLOUR[0]}[Warning] Can't check last version of compiler available $COLOUR_RESET"
elif [[ "$FILE" == *"$lastest_version"* ]]; then
  echo -e "${aCOLOUR[1]}[OK] Seems to be the lastest version of Compiler :  $lastest_version $COLOUR_RESET"
else
  echo -e "${aCOLOUR[0]}[Warning] Newer version of compiler available :  $lastest_version $COLOUR_RESET"
fi 

chmod +x ${DIR}/${FILE}
#echo ${DIR}/${FILE} --prefix /opt/ti --mode unattended
${DIR}/${FILE} --prefix /opt/ti --mode unattended
echo -e "${aCOLOUR[1]}[Installed] ${DIR}/${FILE} $COLOUR_RESET"


