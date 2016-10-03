#!/usr/bin/perl -w
#
use strict;

if (scalar @ARGV != 3) {
	print "usage: <funt_file.lst> <funt_ground_truth.txt> <output_file>\n";
	exit(1);
}

my ($funt_file_list, $funt_ground_truth, $output_file) = @ARGV;

open(LIST, "<$funt_file_list") || die $!;
open(TRUTH, "<$funt_ground_truth") || die $!;
open(OUT, ">$output_file") || die $!;

while (my $line = <LIST>) {
	my $truth = <TRUTH>;
	chomp($line);
	chomp($truth);

	print OUT "$line $truth\n";
}

close(OUT);
close(TRUTH);
close(LIST);

