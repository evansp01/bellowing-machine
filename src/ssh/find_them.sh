for h in {00..79}; do
 	hostname="ghc$h.ghc.andrew.cmu.edu"
	if ping -q -c 3 $hostname >/dev/null; then 
		echo "${hostname} worked"; 
	else  
		echo "${hostname} failed"; 
	fi
done
 
