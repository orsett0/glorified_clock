dd if=$1 bs=1024 | ssh -p$3 "$2" '
dd of=/tmp/program.hex

/opt/pickle/p14 program /tmp/program.hex
/opt/pickle/p14 verify /tmp/program.hex
'

