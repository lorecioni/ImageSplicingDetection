#!/usr/bin/perl -w

use strict;

if ((scalar @ARGV != 1) || ((lc($ARGV[0]) ne "mask") && (lc($ARGV[0]) ne "no_mask"))) {
	print "usage: $0 {mask|no_mask}\n";
	print "where no_mask does not employ an extra mask (recommended: mask should be encoded in the image)\n";
	exit(1);
}

print "# generated with $0 $ARGV[0]\n";

my $input = '/net/cv/illum/multi_shida/img/';
my $output_GW = '/disks/data1/riess/mimorem/table_shida/single/gw/gt_100/';
my $output_TAN = '/disks/data1/riess/mimorem/table_shida/single/tan/gt_100/';

my $mask_dir = '/net/cv/illum/multi_shida/mask/';

my $saturation_level = 10000;
my $bin1 = '/disks/data1/riess/code/build/bin/vole gwOrig';
my $bin2 = '/disks/data1/riess/code/build/bin/vole tanOrig';

my @image = `ls $input | grep -v '_gt.png' | grep -v 'noise_mask.png' | grep -v '_weights.png'`;
my @N = (0, 1, 2);
my @P = (-1, 1, 2);
my @S = (0, 1);

foreach my $img(@image) {
	chomp($img);
	my $img_full = $input . '/' . $img;

	my ($mask) = getSegAndMask($img);

	my $out = getOutput2($img);
	if (lc($ARGV[0]) eq "mask") {
		print "$bin2 --img.image $img_full --img.saturation_level $saturation_level --mask $mask -O $out\n";
	} else {
		print "$bin2 --img.image $img_full --img.saturation_level $saturation_level -O $out\n";
	}

	foreach my $n(@N) {
		foreach my $p(@P) {
			foreach my $s(@S) {
				$out = getOutput($img, $n, $p, $s);
				if (lc($ARGV[0]) eq "mask") {
					print "$bin1 --img.image $img_full --img.saturation_level $saturation_level --mask $mask --n $n --p $p --sigma $s -O $out\n";
				} else {
					print "$bin1 --img.image $img_full --img.saturation_level $saturation_level --n $n --p $p --sigma $s -O $out\n";
				}
			}
		}
	}
}

sub getSegAndMask {
	my $img = shift;

	return ($mask_dir . "/" . $img ."_mask.png");
}

sub getOutput {
	my ($img, $n, $p, $s) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output_GW . '/' . $without_extension . '_' . $n . '_'. $p . '_' . $s . ".png";
}

sub getOutput2 {
	my ($img) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output_TAN . '/' . $without_extension . ".png";
}



