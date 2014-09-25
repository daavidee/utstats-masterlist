<h2>About</h2>
This is a command-line utility written in C++ to parse the log files of the <a href="http://utstats.sourceforge.net/">UTStats</a> module for Unreal Tournament and generate a masterlist. A masterlist is a list of sorted IPs with the various player nicknames used under each IP address. This is used for quick player identification via an IRC script, etc.

<h2>Performance</h2>
On an i5-3570K @ 4.0GHz with an Intel 330 180GB SSD, logfiles are processed at about 42MB/s. Currently the program is CPU-bottlenecked on an SSD. Though the program could benefit substantially by being made multi-threaded, this was never done since it will finish in under a minute for a typical workload.

<h2>Notes</h2>
<ul>
	<li>The UTStats log files do not give the player name and IP address on the same line but rather link these together with a unique (integer) player identifier, adding a little bit of extra complexity to the code</li>
	<li>This program can be used with an already existing masterlist.txt and will simply append new entries. V1 masterlists can also be used</li>
	<li>The view source section below contains a compiled executable along with a sample log file</li>
</ul>