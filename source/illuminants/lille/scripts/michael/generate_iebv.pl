#!/usr/bin/perl -w

use strict;

if ((scalar @ARGV != 1) || ((lc($ARGV[0]) ne "mask") && (lc($ARGV[0]) ne "no_mask"))) {
	print "usage: $0 {mask|no_mask}\n";
	print "where no_mask does not employ an extra mask (recommended: mask should be encoded in the image)\n";
	exit(1);
}

print "# generated with $0 $ARGV[0]\n";

my $input = '/net/cv/illum/multi_michael/img/';
my $output = '/disks/data1/riess/mimorem/table_michael/specular/harsh/';
my $iebv_config = '/disks/data1/riess/code/reflectance/lille/config/config_iebv_harsh.txt';

my $seg_dir  = '/net/cv/illum/multi_michael/gt/';
my $mask_dir = '/net/cv/illum/multi_michael/mask/';

my $bin = '/disks/data1/riess/code/build/bin/vole liebv';

my $saturation_level = '--img.saturation_level 10000';
my $dark_level = '--img.dark_level 1000';
my @image = `ls ${input}/*0.png`;

foreach my $img_full(@image) {
	chomp($img_full);
	my $img = $img_full;
	$img =~ s/^.*\/(.+?\.png)$/$1/;

	my ($seg_img, $mask) = getSegAndMask($img);

	my $out = getOutput($img);
	if (lc($ARGV[0]) eq "mask") {
		print "$bin --img.image $img_full $dark_level $saturation_level --iebv_config $iebv_config -S $seg_img --mask $mask -O $out\n";

	} else {
		print "$bin --img.image $img_full $dark_level $saturation_level --iebv_config $iebv_config -S $seg_img -O $out\n";
	}
}

sub getSegAndMask {
	my $img = shift;

	my ($scene) = ($img =~ /^([^_]+)_/);

	return ($seg_dir . "/" . $scene ."_seg.png", $mask_dir . "/" . $scene ."_mask.pbm");
}

sub getOutput {
	my ($img) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output . '/' . $without_extension . ".png";
}




