check_host() {
	ping -q -c 1 $1 >/dev/null
	if [ $? -eq 0 ]; then
		echo $1 up
		echo $1 >> $2
	else
		echo $1 down
	fi
}

if [ -z "$1" ]; then
	echo "USAGE: $0 FILE"
	exit 1
fi

for h in {00..79}; do
 	hostname="ghc$h.ghc.andrew.cmu.edu"
	check_host $hostname $1 &
done
 
