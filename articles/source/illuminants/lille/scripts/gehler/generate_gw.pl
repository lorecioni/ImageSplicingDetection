#!/usr/bin/perl -w

use strict;

if ((scalar @ARGV != 1) || ((lc($ARGV[0]) ne "mask") && (lc($ARGV[0]) ne "no_mask"))) {
	print "usage: $0 {mask|no_mask}\n";
	print "where no_mask does not employ an extra mask (recommended: mask should be encoded in the image)\n";
	exit(1);
}

print "# generated with $0 $ARGV[0]\n";

my $input_dir = '/net/cv/illum/shi/';
my $input_csv = '/net/cv/illum/shi/gehler_raw12bit.csv';

my $output = '/disks/data1/riess/mimorem/table_gehler/diffuse/gw/';

my $seg_dir  = '/net/cv/illum/shi/seg_100_30/';

my $saturation_level = '--img.saturation_level 3690';
my $dark_level = '--img.dark_level 128'; # only if image name starts with 'IMG_' :(
my $bin = '/disks/data1/riess/code/build/bin/vole lgrayworld';

# read image list from csv-file, add segmentation manually

#my @image = `ls $input | grep -v '_gt.png' | grep -v 'noise_mask.png' | grep -v '_weights.png'`;
my @N = (0, 1, 2);
my @P = (-1, 1, 2);
my @S = (0, 1);

open(CSV, "<$input_csv") || die $!;

my $line = <CSV>; # drop first line

while ($line = <CSV>) {
	next if ($line !~ /\S/);    # empty line
	next if ($line =~ /^\s*#/); # commented line

	chomp($line);
	my @entries = split(',', $line); # optimistic parsing: note that commas in quotes kill it.

	my ($img, $tag, $indoor, $color_space, $gt_r, $gt_g, $gt_b, $mask, $target) = @entries;

	my $img_full = $input_dir . '/' . $img;

	my $seg_img = $img;
	$seg_img =~ s/^raw12bit\//seg_100_30\//g;
	$seg_img = $input_dir . '/' . $seg_img;

	$mask = $input_dir . '/' . $mask;

	foreach my $n(@N) {
		foreach my $p(@P) {
			foreach my $s(@S) {
				my $out = getOutput($img, $n, $p, $s);
				my $the_dark_level = '';
				if ($img_full =~ /\/IMG_[^\/]*$/) {
					$the_dark_level = $dark_level
				}
				if (lc($ARGV[0]) eq "mask") {
					print "$bin --img.image $img_full $the_dark_level $saturation_level -S $seg_img --mask $mask --n $n --p $p --sigma $s -O $out\n";
				} else {
					print "$bin --img.image $img_full $the_dark_level $saturation_level -S $seg_img --n $n --p $p --sigma $s -O $out\n";
				}
			}
		}
	}
}

close CSV;

sub getOutput {
	my ($img, $n, $p, $s) = @_;
	my ($without_extension) = ($img =~ /^.*\/(.+?)$/);
	$without_extension =~ s/\.[^\.]*$//;

	return $output . '/' . $without_extension . '_' . $n . '_'. $p . '_' . $s . ".png";
}


#-S /net/cv/illum/multi_shida/seg_100_30//bmug_seg.png --mask /net/cv/illum/multi_shida/mask//bmug_mask.png --n 1 --p 1 --sigma 1 -O /disks/data1/riess/mimorem/gw//bmug_b_r_0_-1_0.png


