#!/usr/bin/perl -w

use strict;

if ((scalar @ARGV != 1) || ((lc($ARGV[0]) ne "mask") && (lc($ARGV[0]) ne "no_mask"))) {
	print "usage: $0 {mask|no_mask}\n";
	print "where no_mask does not employ an extra mask (recommended: mask should be encoded in the image)\n";
	exit(1);
}

print "# generated with $0 $ARGV[0]\n";

my $input = '/net/cv/illum/multi_michael/gt_pw/';
my $output = '/disks/data1/riess/mimorem/table_michael/diffuse/gw/';

my $seg_dir  = '/net/cv/illum/multi_michael/gt/';
my $mask_dir = '/net/cv/illum/multi_michael/mask/';

my $bin = '/disks/data1/riess/code/build/bin/vole lgrayworld';

my @scenes = ('chalk', 'figures', 'fruits', 'rabbit');

my @image = `ls ${input}/*_norm.png`;

my @N = (0, 1, 2);
my @P = (-1, 1, 2);
my @S = (0, 1);

foreach my $img_full(@image) {
	chomp($img_full);
	my $img = $img_full;
	$img =~ s/^.*\/(.+?\.png)$/$1/;

	my ($seg_img, $mask) = getSegAndMask($img);

	foreach my $n(@N) {
		foreach my $p(@P) {
			foreach my $s(@S) {
				my $out = getOutput($img, $n, $p, $s);
				if (lc($ARGV[0]) eq "mask") {
					print "$bin --img.image $img_full -S $seg_img --mask $mask --n $n --p $p --sigma $s -O $out\n";
				} else {
					print "$bin --img.image $img_full -S $seg_img --n $n --p $p --sigma $s -O $out\n";
				}
			}
		}
	}
}

sub getSegAndMask {
	my $img = shift;

	my ($scene) = ($img =~ /^([^_]+)_/);

	return ($seg_dir . "/" . $scene ."_seg.png", $mask_dir . "/" . $scene ."_mask.pbm");
}

sub getOutput {
	my ($img, $n, $p, $s) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output . '/' . $without_extension . '_' . $n . '_'. $p . '_' . $s . ".png";
}


