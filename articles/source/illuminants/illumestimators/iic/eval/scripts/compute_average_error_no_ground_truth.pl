#!/usr/bin/perl -w
#
use strict;
use Math::Trig qw(acos_real :pi);

# what we need:
# 1. type of error metric to use
# 2. Algorithm output file
# 3. Ground truth

if (scalar @ARGV != 1) {
	print "ERROR: need one argument: <algorithm output file>\n";
	exit 1;
}

my ($algo_output) = @ARGV;

# 1. open algo output
# 2. take all results for the same input file together (average)
# 3. compute $error_metric for all or a part of the files

open(ALG, "<$algo_output") || die $!;

my $eval = {};
# sort $algo_output acc. to file name
while (my $line = <ALG>) {
	chomp($line);
	next if ($line !~ /\d/); # ignore lines without figures
	next if ($line =~ / nan /); # something broke our algorithm...
	my @entries = split(/ /, $line); # breaks if file name contains spaces
	if (!defined($eval->{$entries[0]})) {
		$eval->{$entries[0]} = [];
	}
	my $arrayref = $eval->{$entries[0]};
	push(@$arrayref, [@entries[1..3]]);
}
close(ALG);

my $eval_avg = {};
my $eval_median = {};

# compute average estimate
foreach my $key (keys %$eval) {
	my $arrayref = $eval->{$key};
	my $avgred = 0.0;
	my $avggreen = 0.0;
	my $avgblue = 0.0;
	my $count = 0;
	my $minpos = -1;
	foreach my $estimate (@$arrayref) {
		$count++;
		$avgred += $estimate->[0];
		$avggreen += $estimate->[1];
		$avgblue += $estimate->[2];
	}
	$avgred /= $count;
	$avggreen /= $count;
	$avgblue /= $count;
	my $stddevred = 0;
	my $stddevgreen = 0;
	my $stddevblue = 0;
	foreach my $estimate (@$arrayref) {
		$stddevred += ($avgred - $estimate->[0]) * ($avgred - $estimate->[0]);
		$stddevgreen += ($avggreen - $estimate->[1]) * ($avggreen - $estimate->[1]);
		$stddevblue += ($avgblue - $estimate->[2]) * ($avgblue - $estimate->[2]);
	}

	$stddevred = sqrt($stddevred / ($count - 1));
	$stddevgreen = sqrt($stddevgreen / ($count - 1));
	$stddevblue = sqrt($stddevblue / ($count - 1));

	# compute median of estimates (maybe we improve?)
	my $median_table = [];
	for (my $i = 0; $i < (scalar @$arrayref); $i++) {
		for (my $j = 0; $j < (scalar @$arrayref); $j++) {
			$median_table->[$i]->[$j] = 0;
		}
	}
	for (my $i = 0; $i < (scalar @$arrayref); $i++) {
		$median_table->[$i]->[$i] = 0;
		for (my $j = $i+1; $j < (scalar @$arrayref); $j++) {
			# compute angular error between the two estimates
			my $ang_error = &compute_angular_error($arrayref->[$i], $arrayref->[$j]);
			$median_table->[$i]->[$j] = $ang_error;
			$median_table->[$j]->[$i] = $ang_error;
		}
	}
	# find the estimate with the minimum sum of angular errors
	for (my $i = 0; $i < (scalar @$median_table); $i++) {
		my $min = 1000000;
		$minpos = -1;
		my $sum = 0;
		for (my $j = 0; $j < (scalar @$median_table); $j++) {
			$sum += $median_table->[$i]->[$j];
		}
		if ($sum < $min) {
			$min = $sum;
			$minpos = $i;
		}
	}


	# normalize the estimate (shouldn't be necessary, but who knows)
	my $sum = $avgred + $avggreen + $avgblue;
	if ($sum != 0) {
		$avgred /= $sum;
		$avggreen /= $sum;
		$avgblue /= $sum;
		$eval_avg->{$key}->{avg_estimate} = [$avgred, $avggreen, $avgblue];
		$eval_avg->{$key}->{stddev} = [$stddevred, $stddevgreen, $stddevblue];
	} else {
		$eval_avg->{$key}->{avg_estimate} = [-1, -1, -1];
		$eval_avg->{$key}->{stddev} = [-1, -1, -1];
	}

	if ($sum != 0) {
		my $median_red = $arrayref->[$minpos]->[0] / $sum; 
		my $median_green = $arrayref->[$minpos]->[1] / $sum; 
		my $median_blue = $arrayref->[$minpos]->[2] / $sum; 
		$eval_median->{$key}->{avg_estimate} = [$median_red, $median_green, $median_blue];
		$eval_median->{$key}->{stddev} = [$stddevred, $stddevgreen, $stddevblue];
	} else {
		$eval_median->{$key}->{avg_estimate} = [-1, -1, -1];
		$eval_median->{$key}->{stddev} = [-1, -1, -1];
	}

}

foreach my $key (keys %$eval_avg) {
	print "$key:\n\t\t\taverage: ", $eval_avg->{$key}->{avg_estimate}[0], " ", $eval_avg->{$key}->{avg_estimate}[1], " ", $eval_avg->{$key}->{avg_estimate}[2], "\n";
	print "\t\t\t(+- ", $eval_avg->{$key}->{stddev}[0], " ", $eval_avg->{$key}->{stddev}[1], " " , $eval_avg->{$key}->{stddev}[2],")\n";
}

sub compute_angular_error() {
	my $ang1 = shift;
	my $ang2 = shift;

	my $zaehler = $ang1->[0]*$ang2->[0] + $ang1->[1]*$ang2->[1] + $ang1->[2]*$ang2->[2];

	my $length1 = sqrt($ang1->[0] * $ang1->[0] +
				  $ang1->[1] * $ang1->[1] +
				  $ang1->[2] * $ang1->[2]);
	my $length2 = sqrt($ang2->[0] * $ang2->[0] +
					   $ang2->[1] * $ang2->[1] +
					   $ang2->[2] * $ang2->[2]);

	my $ang_error = 100000;
	if ($length1 != 0 && $length2 != 0) {
		$ang_error = acos_real($zaehler / ($length1 * $length2)) * 360.0 / pi2;
	}
	return $ang_error;
}

