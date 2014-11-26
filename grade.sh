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
			'parport1: enabled=1, file="bochs.out"' \
			>$out 2>$err
	)
}


# Usage: runtest <tagname> <defs> <strings...>
runtest() {
	perl -e "print '$1: '"
	rm -f obj/kern/init.o obj/kern/kernel obj/kern/bochs.img obj/fs/fs.img
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

runtest1 hello \
	'.00000000. new env 00000800' \
	'hello, world' \
	'i am environment 00000800' \
	'.00000800. destroying 00000800' \
	'.00000800. free env 00000800' \
	'Destroyed the only environment - nothing more to do!'

# the [00000800] tags should have [] in them, but that's 
# a regular expression reserved character, and i'll be damned if
# I can figure out how many \ i need to add to get through 
# however many times the shell interprets this string.  sigh.

runtest1 buggyhello \
	'.00000800. PFM_KILL va 00000001 ip f01.....' \
	'.00000800. free env 00000800'

runtest1 evilhello \
	'.00000800. PFM_KILL va ef800000 ip f01.....' \
	'.00000800. free env 00000800'

runtest1 divzero \
	! '1/0 is ........!' \
	'TRAP frame at 0xefbfff..' \
	'  trap 0x00000000 Divide error' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	'.00000800. free env 00000800'

runtest1 breakpoint \
	'Welcome to the JOS kernel monitor!' \
	'TRAP frame at 0xefbfffbc' \
	'  trap 0x00000003 Breakpoint' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	! '.00000800. free env 00000800'

runtest1 softint \
	'Welcome to the JOS kernel monitor!' \
	'TRAP frame at 0xefbfffbc' \
	'  trap 0x0000000d General Protection' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	'.00000800. free env 00000800'

runtest1 badsegment \
	'TRAP frame at 0xefbfffbc' \
	'  trap 0x0000000d General Protection' \
	'  err  0x0000001c' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	'.00000800. free env 00000800'

runtest1 faultread \
	! 'I read ........ from location 0!' \
	'.00000800. user fault va 00000000 ip 008.....' \
	'TRAP frame at 0xefbfffbc' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000004' \
	'.00000800. free env 00000800'

runtest1 faultreadkernel \
	! 'I read ........ from location 0xf0100000!' \
	'.00000800. user fault va f0100000 ip 008.....' \
	'TRAP frame at 0xefbfffbc' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000005' \
	'.00000800. free env 00000800' \

runtest1 faultwrite \
	'.00000800. user fault va 00000000 ip 008.....' \
	'TRAP frame at 0xefbfffbc' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000006' \
	'.00000800. free env 00000800'

runtest1 faultwritekernel \
	'.00000800. user fault va f0100000 ip 008.....' \
	'TRAP frame at 0xefbfffbc' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000007' \
	'.00000800. free env 00000800'

runtest1 testbss \
	'Making sure bss works right...' \
	'Yes, good.  Now doing a wild write off the end...' \
	'.00000800. user fault va 00c..... ip 008.....' \
	'.00000800. free env 00000800'



echo Score: $score/60

score=0



