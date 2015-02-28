#!/bin/bash

VSBUTIL=../../../utility/vsbutil/vsbutil.py

$VSBUTIL dfu > /dev/null
./serialize.py $1 && \
sleep 1 && \
make dfu && \
sleep 1 && \
$VSBUTIL wipeconfig && \
echo && echo && echo && \
$VSBUTIL setkeys shift+h e l l o space t h e r e shift+1 enter && \
echo -n "Button test> " && \
read vsbsays && \
if [ "$vsbsays" = "Hello there!" ]
	then
		echo "Success! Re-wiping config."
		$VSBUTIL eepwrite 0 ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff && \
		$VSBUTIL wipeconfig && \
		$VSBUTIL loadconfig && \
		$VSBUTIL getconfig && \
		echo && echo SERIAL: `$VSBUTIL getserial`
	else
		echo "Failure?"
	fi
