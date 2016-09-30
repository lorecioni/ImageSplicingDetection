#!/usr/bin/perl -w

use strict;

if ((scalar @ARGV != 2) || ((lc($ARGV[0]) ne "mask") && (lc($ARGV[0]) ne "no_mask")) ||
	((lc($ARGV[1] ne "orig")) && (lc($ARGV[1] ne "pre")) && (lc($ARGV[1] ne "post")))) {

	print "usage: $0 {mask|no_mask} {orig|pre|post}\n";
	print "where no_mask does not employ an extra mask (recommended: mask should be encoded in the image)\n";
	print "\"orig\" uses the original images, \"pre\" sharpened with premultiplication, \"post\" sharpened with postmultiplication\n";
	exit(1);
}

print "# generated with $0 $ARGV[0] $ARGV[1]\n";

my $input = '/net/cv/illum/multi_shida/gt_50/';
my $output = '/disks/data1/riess/mimorem/table_shida/diffuse/gw/gt_50/';
my $seg_dir  = '/net/cv/illum/multi_shida/seg_100_30/';
my $mask_dir = '/net/cv/illum/multi_shida/mask/';

if (lc($ARGV[1]) ne "orig") {
	if (lc($ARGV[1]) eq "pre") {
		$input = '/net/cv/illum/multi_shida/gt_50_shpre/';
		$output = '/disks/data1/riess/mimorem/shpre/table_shida/diffuse/gw/gt_50/';
	} else {
		$input = '/net/cv/illum/multi_shida/gt_50_shpst/';
		$output = '/disks/data1/riess/mimorem/shpost/table_shida/diffuse/gw/gt_50/';
	}
}

my $saturation_level = 10000;
my $bin = '/disks/data1/riess/code/build/bin/vole lgrayworld';

my @image = `ls $input | grep -v '_gt.png' | grep -v 'noise_mask.png' | grep -v '_weights.png'`;
my @N = (0, 1, 2);
my @P = (-1, 1, 2);
my @S = (0, 1);

if (!-x $output) {
	print "mkdir -p $output\n";
}

foreach my $img(@image) {
	chomp($img);
	my $img_full = $input . '/' . $img;

	my ($seg_img, $mask) = getSegAndMask($img);

	foreach my $n(@N) {
		foreach my $p(@P) {
			foreach my $s(@S) {
				my $out = getOutput($img, $n, $p, $s);
				if (lc($ARGV[0]) eq "mask") {
					print "$bin --img.image $img_full --img.saturation_level $saturation_level -S $seg_img --mask $mask --n $n --p $p --sigma $s -O $out\n";
				} else {
					print "$bin --img.image $img_full --img.saturation_level $saturation_level -S $seg_img --n $n --p $p --sigma $s -O $out\n";
				}
			}
		}
	}
}

sub getSegAndMask {
	my $img = shift;

	my ($scene) = ($img =~ /^([^_]+)_/);

	return ($seg_dir . "/" . $scene ."_seg.png", $mask_dir . "/" . $scene ."_mask.png");
}

sub getOutput {
	my ($img, $n, $p, $s) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output . '/' . $without_extension . '_' . $n . '_'. $p . '_' . $s . ".png";
}


#-S /net/cv/illum/multi_shida/seg_100_30//bmug_seg.png --mask /net/cv/illum/multi_shida/mask//bmug_mask.png --n 1 --p 1 --sigma 1 -O /disks/data1/riess/mimorem/gw//bmug_b_r_0_-1_0.png


