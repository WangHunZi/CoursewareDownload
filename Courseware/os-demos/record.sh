sleep 1
read -p 'Start record... ' _
P=$(realpath ./records)
$@
clear
asciinema rec -i 2.5 $P/$(date +%b%d-%H:%M:%S).cast
