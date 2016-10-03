#!/usr/bin/perl -w
#
use strict;

use File::Spec;

if (scalar @ARGV != 3) {
	print "3 arguments required: <root_directory> <output_file_list_filename> <output_ground_truth_filename>\n";
	exit 1;
}

my ($rootdir, $output_filelist, $output_ground_truth) = @ARGV;

my @images = `find $rootdir -iname "*.tif"`;

open(FILELIST_OUT, ">$output_filelist") || die $!;
open(TRUTH_OUT, ">$output_ground_truth") || die $!;

foreach my $img (@images) {
	chomp($img);
	my ($volume,$directories,$file) = File::Spec->splitpath( $img );
	my ($file_trunk) = ($file =~ /(.+).tif$/i);
	my $ground_truth = $file_trunk . ".rgb";
	open(TRUTH_IN, "<$directories/$ground_truth") || die $!;
	my $truth_line = <TRUTH_IN>;
	close(TRUTH_IN);
	chomp($truth_line);
	my ($r, $g, $b) = split(/ +/, $truth_line);
	my $sum = $r + $g + $b;
	# compute chromaticity
	my ($ch_r, $ch_g, $ch_b) = ($r / $sum, $g / $sum, $b / $sum);

	# remove root_directory from image path
	$img =~ s/^$rootdir\/*//;
	print TRUTH_OUT "$img $ch_r $ch_g $ch_b\n";
	print FILELIST_OUT "$img\n";
}

#close(TRUTH_OUT);
#close(FILELIST_OUT);
#
#
#my $classes_files = {};
#
#foreach my $img (@images) {
#	chomp($img);
#	my ($volume,$directories,$file) = File::Spec->splitpath( $img );
#	my ($class) = ($directories =~ /\/?([^\/]+)\/?$/); # extract last directory name (breaks if directory name contains a slash
#	print "class = $class\n";
#	if (!defined $classes_files->{$class}) {
#		$classes_files->{$class} = [];
#	}
#	my $tmpref = $classes_files->{$class};
#	push @$tmpref, $file;
#}


