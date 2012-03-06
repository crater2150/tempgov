TEMPGOV
=======

A daemon that clocks down your CPU if it gets too hot.


Why?
----

I have a laptop with a tendency to overheat under load. It seems not to be
capable of reducing the clock itself in this case and switches off.

tempgov checks temperature information in the sysfs files and sets the CPU
governor to 'powersave', when it gets too hot. As soon as the temperature falls
below a certain level, the governor is changed back to ondemand.

Usage
-----

Tempgov takes several parameters on start:

	--thermal,        -t  File to read system temperature from
	--cpus,           -c  File, where the governor name should be written
	                      Glob patterns are allowed.

	--gov-default,    -d  Governor to use under normal conditions
	--gov-high,       -g  Governor to use, when temperature is high

	--pidfile,        -p  Where to write the PID file

	--high-threshold, -h  Clock down above this temperature
	--low-threshold,  -l  Return to default, when falling below this
	                      temperature

	--interval,       -i  seconds between temperature readings
