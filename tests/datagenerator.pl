#!/usr/bin/perl
#


open(OUTPUT, ">$ARGV[0]") || die "Can't open $ARGV[0]: $!";

$i = 0;

while (true) {
	print OUTPUT "$i ";
	$ii = $i/10;
	print OUTPUT sin($ii) + rand(0.4) - 0.2;
	print OUTPUT " ";
	print OUTPUT cos($ii) + rand(0.4) - 0.2;
	print OUTPUT " ";
	print OUTPUT sin($ii * $ii) * (cos($ii) + rand(0.4) - 0.2);
	print OUTPUT " ";
	print OUTPUT "\n";
	$i++;
	select(undef, undef, undef, 0.001);
}

