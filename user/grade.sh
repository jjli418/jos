#!/bin/sh

verbose=false

if [ "x$1" = "x-v" ]
then
	verbose=true
	out=/dev/stdout
	err=/dev/stderr
else
	out=/dev/null
	err=/dev/null
fi

pts=5
timeout=30

runbochs() {
	# Find the address of the kernel readline function,
	# which the kernel monitor uses to read commands interactively.
	brkaddr=`grep readline obj/kern/kernel.sym | sed -e's/ .*$//g'`
	#echo "brkaddr $brkaddr"

	# Run Bochs, setting a breakpoint at readline(),
	# and feeding in appropriate commands to run, then quit.
	(
		echo vbreak 0x8:0x$brkaddr
		echo c
		echo die
		echo quit
	) | (
		ulimit -t $timeout
		bochs -q 'display_library: nogui' \
			'parport1: enabled=1, file="bochs.out"'
	) >$out 2>$err
}


# Usage: runtest <tagname> <defs> <strings...>
runtest() {
	perl -e "print '$1: '"
	rm -f obj/kern/init.o obj/kern/kernel obj/kern/bochs.img
	if $verbose
	then
		echo "gmake $2... "
	fi
	if ! gmake $2 >$out
	then
		echo gmake failed 
		exit 1
	fi
	runbochs
	if [ ! -s bochs.out ]
	then
		echo 'no bochs.out'
	else
		shift
		shift
		continuetest "$@"
	fi
}

quicktest() {
	perl -e "print '$1: '"
	shift
	continuetest "$@"
}

continuetest() {
	okay=yes

	not=false
	for i
	do
		if [ "x$i" = "x!" ]
		then
			not=true
		elif $not
		then
			if egrep "^$i\$" bochs.out >/dev/null
			then
				echo "got unexpected line '$i'"
				if $verbose
				then
					exit 1
				fi
				okay=no
			fi
			not=false
		else
			if ! egrep "^$i\$" bochs.out >/dev/null
			then
				echo "missing '$i'"
				if $verbose
				then
					exit 1
				fi
				okay=no
			fi
			not=false
		fi
	done
	if [ "$okay" = "yes" ]
	then
		score=`echo $pts+$score | bc`
		echo OK
	else
		echo WRONG
	fi
}

# Usage: runtest1 [-tag <tagname>] <progname>
runtest1() {
	if [ $1 = -tag ]
	then
		shift
		tag=$1
		prog=$2
		shift
		shift
	else
		tag=$1
		prog=$1
		shift
	fi
	runtest "$tag" "DEFS=-DTEST=_binary_obj_user_${prog}_start DEFS+=-DTESTSIZE=_binary_obj_user_${prog}_size" "$@"
}



score=0
timeout=10

runtest1 dumbfork \
	'.00000000. new env 00000800' \
	'.00000000. new env 00001001' \
	'0: I am the parent!' \
	'9: I am the parent!' \
	'0: I am the child!' \
	'9: I am the child!' \
	'19: I am the child!' \
	'.00001001. exiting gracefully' \
	'.00001001. free env 00001001' \
	'.00001802. exiting gracefully' \
	'.00001802. free env 00001802'

echo PART A SCORE: $score/5

runtest1 faultread \
	! 'I read ........ from location 0!' \
	'.00001001. user fault va 00000000 ip 008.....' \
	'TRAP frame at 0xefbfffbc' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000004' \
	'.00001001. free env 00001001'

runtest1 faultwrite \
	'.00001001. user fault va 00000000 ip 008.....' \
	'TRAP frame at 0xefbfffbc' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000006' \
	'.00001001. free env 00001001'

runtest1 faultdie \
	'i faulted at va deadbeef, err 6' \
	'.00001001. exiting gracefully' \
	'.00001001. free env 00001001' 

runtest1 faultalloc \
	'fault deadbeef' \
	'this string was faulted in at deadbeef' \
	'fault cafebffe' \
	'fault cafec000' \
	'this string was faulted in at cafebffe' \
	'.00001001. exiting gracefully' \
	'.00001001. free env 00001001'

runtest1 faultallocbad \
	'.00001001. PFM_KILL va deadbeef ip f01.....' \
	'.00001001. free env 00001001' 

runtest1 faultnostack \
	'.00001001. PFM_KILL va eebfff.. ip f01.....' \
	'.00001001. free env 00001001'

runtest1 faultbadhandler \
	'.00001001. PFM_KILL va eebfef.. ip f01.....' \
	'.00001001. free env 00001001'

runtest1 faultevilhandler \
	'.00001001. PFM_KILL va eebfef.. ip f01.....' \
	'.00001001. free env 00001001'

runtest1 forktree \
	'....: I am .0.' \
	'....: I am .1.' \
	'....: I am .000.' \
	'....: I am .100.' \
	'....: I am .110.' \
	'....: I am .111.' \
	'....: I am .011.' \
	'....: I am .001.' \
	'.000028... exiting gracefully' \
	'.000048... exiting gracefully' \
	'.000058... exiting gracefully' \
	'.000078... exiting gracefully' \
	'.000078... free env 000078..'

echo PART B SCORE: $score/50

runtest1 spin \
	'.00000000. new env 00000800' \
	'.00000000. new env 00001001' \
	'I am the parent.  Forking the child...' \
	'.00001001. new env 00001802' \
	'I am the parent.  Running the child...' \
	'I am the child.  Spinning...' \
	'I am the parent.  Killing the child...' \
	'.00001001. destroying 00001802' \
	'.00001001. free env 00001802' \
	'.00001001. exiting gracefully' \
	'.00001001. free env 00001001'

runtest1 pingpong \
	'.00000000. new env 00000800' \
	'.00000000. new env 00001001' \
	'.00001001. new env 00001802' \
	'send 0 from 1001 to 1802' \
	'1802 got 0 from 1001' \
	'1001 got 1 from 1802' \
	'1802 got 8 from 1001' \
	'1001 got 9 from 1802' \
	'1802 got 10 from 1001' \
	'.00001001. exiting gracefully' \
	'.00001001. free env 00001001' \
	'.00001802. exiting gracefully' \
	'.00001802. free env 00001802' \

runtest1 primes \
	'.00000000. new env 00000800' \
	'.00000000. new env 00001001' \
	'.00001001. new env 00001802' \
	'2 .00001802. new env 00002003' \
	'3 .00002003. new env 00002804' \
	'5 .00002804. new env 00003005' \
	'7 .00003005. new env 00003806' \
	'11 .00003806. new env 00004007' 

echo PART C SCORE: $score/65



