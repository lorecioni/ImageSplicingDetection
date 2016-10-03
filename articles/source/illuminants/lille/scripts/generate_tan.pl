#!/usr/bin/perl -w

use strict;

if ((scalar @ARGV != 1) || ((lc($ARGV[0]) ne "mask") && (lc($ARGV[0]) ne "no_mask"))) {
	print "usage: $0 {mask|no_mask}\n";
	print "where no_mask does not employ an extra mask (recommended: mask should be encoded in the image)\n";
	exit(1);
}

print "# generated with $0 $ARGV[0]\n";

my $input = '/net/cv/illum/multi_shida/img/';
my $output = '/disks/data1/riess/mimorem/table_shida/specular/tan/img/';

my $seg_dir  = '/net/cv/illum/multi_shida/seg_100_30/';
my $mask_dir = '/net/cv/illum/multi_shida/mask/';

my $saturation_level = 10000;
my $bin = '/disks/data1/riess/code/build/bin/vole ltan';

my @image = `ls $input`;

foreach my $img(@image) {
	chomp($img);
	my $img_full = $input . '/' . $img;

	my ($seg_img, $mask) = getSegAndMask($img);

	my $out = getOutput($img);
	if (lc($ARGV[0]) eq "mask") {
		print "$bin --img.image $img_full --img.saturation_level $saturation_level -S $seg_img --mask $mask -O $out\n";

	} else {
		print "$bin --img.image $img_full --img.saturation_level $saturation_level -S $seg_img -O $out\n";
	}
}

sub getSegAndMask {
	my $img = shift;

	my ($scene) = ($img =~ /^([^_]+)_/);

	return ($seg_dir . "/" . $scene ."_seg.png", $mask_dir . "/" . $scene ."_mask.png");
}

sub getOutput {
	my ($img) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output . '/' . $without_extension . "_" . $saturation_level . ".png";
}




