#!/usr/bin/perl -w
#
use strict;

if (scalar @ARGV != 1) {
	print "please enter rootdir of evaluation (e.g. /net/faui5pc84/disks/data/riess/eval/)\n";
	exit(1);
}

my $root_dir = $ARGV[0];

#my @barnard_files = grep { $_ !~ /\/[^\/]+\.[^\/]+\/{0,1}$/ } `find $root_dir -name "*barnard*"`; # only directories (heuristic: no "." at last component)
#my @funt_files = grep { $_ !~ /\/[^\/]+\.[^\/]+\/{0,1}$/ } `find $root_dir -name "*funt*"`; # only directories (heuristic: no "." at last component)

my @barnard_files = grep { $_ !~ /\/[^\/]+\..{0,3}\/{0,1}$/ } `find $root_dir -name "*barnard*"`; # only directories (heuristic: no "." and 0-3 characters at last component)
my @funt_files = grep { $_ !~ /\/[^\/]+\..{0,3}\/{0,1}$/ } `find $root_dir -name "*funt*"`; # only directories (heuristic: no "." and 0-3 characters at last componentat last component)

print "\nBarnard:\n";

foreach my $barnard (@barnard_files) {
	chomp($barnard);
#	my $global_result = `/home/simlmeye/code/src/cv/reflectance/iebv/eval/compute_median_error.pl $barnard/eval_result.txt /net/cv/illum/barnard/barnard_dielectric_specularities/specular_8_bit/ground_truth.txt | grep "global median"`;
	my $global_result = `/home/simlmeye/code/src/cv/reflectance/illumestimators/iic/eval/scripts/compute_median_error.pl $barnard/eval_result.txt /net/cv/illum/barnard/barnard_dielectric_specularities/specular_8_bit/ground_truth.txt | grep "global median"`;
	chomp($global_result);
	print "result on $barnard: $global_result\n";
}

print "\nFunt:\n";

foreach my $funt (@funt_files) {
	chomp($funt);
	my $global_result = `/home/simlmeye/code/src/cv/reflectance/illumestimators/iic/eval/scripts/compute_median_error.pl $funt/eval_result.txt /net/cv/illum/ciurea_funt/files_ground_truth.txt | grep "global median"`;
	chomp($global_result);
	print "result on $funt: $global_result\n";
}

