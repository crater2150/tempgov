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
