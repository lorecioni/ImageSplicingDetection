#!/usr/bin/perl -w
#
use strict;
use Math::Trig qw(acos_real :pi);

# what we need:
# 1. type of error metric to use
# 2. Algorithm output file
# 3. Ground truth

if (scalar @ARGV != 2) {
	print "ERROR: need 2 arguments: <algorithm output file> <ground truth file>\n";
	exit 1;
}

my ($algo_output, $ground_truth) = @ARGV;

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

	$avgred /= $count;
	$avggreen /= $count;
	$avgblue /= $count;

	# normalize the estimate (shouldn't be necessary, but who knows)
	my $sum = $avgred + $avggreen + $avgblue;
	$avgred /= $sum;
	$avggreen /= $sum;
	$avgblue /= $sum;
	$eval_avg->{$key}->{avg_estimate} = [$avgred, $avggreen, $avgblue];

	my $median_red = $arrayref->[$minpos]->[0] / $sum; 
	my $median_green = $arrayref->[$minpos]->[1] / $sum; 
	my $median_blue = $arrayref->[$minpos]->[2] / $sum; 
	$eval_median->{$key}->{avg_estimate} = [$median_red, $median_green, $median_blue];

}

# now compare the results to the ground truth, apply the error measure
open(TRUTH, "<$ground_truth") || die "ground truth file $ground_truth: $!";

while (my $line = <TRUTH>) {
	chomp($line);

	next if ($line !~ /\d/); # no numbers? no valid line...

	my @entries = split(/ /, $line); # breaks if file name contains spaces

	my @tmp_entries = ($entries[1], $entries[2], $entries[3]);
	
	next if (!defined $eval_avg->{$entries[0]}); # no estimate for this file? abort...
	
	my $ang_error_avg = &compute_angular_error($eval_avg->{$entries[0]}->{avg_estimate}, \@tmp_entries);
	my $ang_error_median = &compute_angular_error($eval_median->{$entries[0]}->{avg_estimate}, \@tmp_entries);

	if ($ang_error_avg < 100000) {
#		print "ang error for ", $entries[0], " is ", $ang_error, "\n";
	} else {
		print "BIG FAIL at $entries[0]\n";
	}
	if ($ang_error_median < 100000) {
#		print "ang error for ", $entries[0], " is ", $ang_error, "\n";
	} else {
		print "BIG FAIL at $entries[0]\n";
	}
	
	$eval_avg->{$entries[0]}->{ang_error} = $ang_error_avg;
	$eval_median->{$entries[0]}->{ang_error} = $ang_error_median;
}

# sort angular error by folder name
my $ang_by_folder_avg = {};
my $ang_by_folder_median = {};
foreach my $elem (keys %$eval_avg) {
	my @dirs = split(/\//, $elem);
	my $distinguishing_dir = $dirs[-2];
	if (!defined $ang_by_folder_avg->{$distinguishing_dir}) {
		$ang_by_folder_avg->{$distinguishing_dir} = [];
	}
	if (!defined $ang_by_folder_median->{$distinguishing_dir}) {
		$ang_by_folder_median->{$distinguishing_dir} = [];
	}
	my $tmpref_avg = $ang_by_folder_avg->{$distinguishing_dir};
	push @$tmpref_avg, $eval_avg->{$elem}->{ang_error};

	my $tmpref_median = $ang_by_folder_median->{$distinguishing_dir};
	push @$tmpref_median, $eval_median->{$elem}->{ang_error};
}

my @total_avg = ();
my @total_median = ();

foreach my $dist_dir (keys %$ang_by_folder_avg) {
	print "$dist_dir: \n";

	my $tmpref_avg = $ang_by_folder_avg->{$dist_dir};
	my @sorted_avg = sort {$a <=> $b} @$tmpref_avg;
	push @total_avg, @sorted_avg;

	# same for median
	my $tmpref_median = $ang_by_folder_median->{$dist_dir};
	my @sorted_median = sort {$a <=> $b} @$tmpref_median;
	push @total_median, @sorted_median;

	# compute rms (out of curiosity)
	my $sum = 0.0;
	my $n = 0;
	foreach my $err (@sorted_avg) {
		$n++;
		$sum += $err * $err;
	}
	print "sum = $sum, n = $n\n";
	my $rms = sqrt($sum/$n);

#	print join(" ", @sorted_avg), "\n";
	print "min = $sorted_avg[0], max = $sorted_avg[-1], rms = $rms, median = ", $sorted_avg[$#sorted_avg/2], ", \tmedian on medians: ", $sorted_median[$#sorted_median/2], "\n";
}

my @global_sort_avg = sort {$a <=> $b} @total_avg;
my @global_sort_median = sort {$a <=> $b} @total_median;

my ($path, $content) = ($algo_output =~ /^(.+)\/(.+?)$/);
my $dumper = '/tmp/' . $content . '_OWN_.txt';
print "writing to $dumper\n";
open (DUMP, ">$dumper") or die "file $dumper: $!\n";
print DUMP join("\n", @global_sort_median). "\n";
close(DUMP);

print "global median: ", $global_sort_avg[$#global_sort_avg/2], ", \tglobal median on median: ", $global_sort_median[$#global_sort_median/2], "\n\n";

close(TRUTH);


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

