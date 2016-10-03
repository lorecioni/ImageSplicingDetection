#!/usr/bin/perl -w

use strict;

if ((scalar @ARGV != 1) || ((lc($ARGV[0]) ne "mask") && (lc($ARGV[0]) ne "no_mask"))) {
	print "usage: $0 {mask|no_mask}\n";
	print "where no_mask does not employ an extra mask (recommended: mask should be encoded in the image)\n";
	exit(1);
}

print "# generated with $0 $ARGV[0]\n";

my $input = '/net/cv/illum/gijsenij_liu/mixed_all/';
my $output = '/disks/data1/riess/mimorem/table_theo/diffuse/gw/wrong/';

my $seg_dir  = '/net/cv/illum/gijsenij_liu/seg_50_10/';
my $mask_dir = '/net/cv/illum/gijsenij_liu/mask/';

my $bin = '/disks/data1/riess/code/build/bin/vole lgrayworld';

my @image = `ls ${input}/*.tif`;
my @N = (0, 1, 2);
my @P = (-1, 1, 2);
my @S = (0, 1);

foreach my $img_full(@image) {
	chomp($img_full);
	my $img = $img_full;
	$img =~ s/^.*\/(.+?\.tif)$/$1/;

	my $extra_opt = '';
	if ($img =~ /^\d\d\d\d\d_/) {
		$extra_opt = '--img.color_space srgb'
	}

	my ($seg_img, $mask) = getSegAndMask($img);

	foreach my $n(@N) {
		foreach my $p(@P) {
			foreach my $s(@S) {
				my $out = getOutput($img, $n, $p, $s);
				if (lc($ARGV[0]) eq "mask") {
					print "$bin --img.image $img_full $extra_opt -S $seg_img --mask $mask --n $n --p $p --sigma $s -O $out\n";
				} else {
					print "$bin --img.image $img_full $extra_opt -S $seg_img --n $n --p $p --sigma $s -O $out\n";
				}
			}
		}
	}
}

sub getSegAndMask {
	my $img = shift;

	my ($mask, $seg) = ($img, $img);
	$mask =~ s/\.tif$/.png/i;
	return ($seg_dir . "/" . $seg, $mask_dir . "/" . $mask);
}

sub getOutput {
	my ($img, $n, $p, $s) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output . '/' . $without_extension . '_' . $n . '_'. $p . '_' . $s . ".png";
}


#-S /net/cv/illum/multi_shida/seg_100_30//bmug_seg.png --mask /net/cv/illum/multi_shida/mask//bmug_mask.png --n 1 --p 1 --sigma 1 -O /disks/data1/riess/mimorem/gw//bmug_b_r_0_-1_0.png


