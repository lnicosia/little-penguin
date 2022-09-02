#!/bin/sh

i=0;
while true
do
	echo $i > /sys/kernel/debug/fortytwo/foo;
	i=$((i + 1));
done
