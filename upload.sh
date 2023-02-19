dd if=$1 bs=1024 | ssh -p42000 -l pi "$1" '
dd of=/tmp/program.hex
/opt/pickle/p14 program /tmp/program.hex
/opt/pickle/p14 verify /tmp/program.hex
'
    