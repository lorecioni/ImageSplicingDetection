#!/usr/bin/perl -w

use strict;

if ((scalar @ARGV != 1) || ((lc($ARGV[0]) ne "mask") && (lc($ARGV[0]) ne "no_mask"))) {
	print "usage: $0 {mask|no_mask}\n";
	print "where no_mask does not employ an extra mask (recommended: mask should be encoded in the image)\n";
	exit(1);
}

print "# generated with $0 $ARGV[0]\n";

my $input = '/net/cv/illum/gijsenij_liu/mixed_all/';
my $output = '/disks/data1/riess/mimorem/table_theo/specular/vote/avg_6000/';
my $iebv_config = '/disks/data1/riess/code/reflectance/lille/config/config_iebv_avg.txt';

my $seg_dir  = '/net/cv/illum/gijsenij_liu/seg_50_10/';
my $mask_dir = '/net/cv/illum/gijsenij_liu/mask/';

my $saturation_level = 6000;
my $bin = '/disks/data1/riess/code/build/bin/vole liebv';

my @image = `ls ${input}/*.tif`;

foreach my $img_full(@image) {
	chomp($img_full);
	my $img = $img_full;
	$img =~ s/^.*\/(.+?\.tif)$/$1/;

	my ($seg_img, $mask) = getSegAndMask($img);

	my $out = getOutput($img);
	if (lc($ARGV[0]) eq "mask") {
		print "$bin --img.image $img_full --img.saturation_level $saturation_level --iebv_config $iebv_config -S $seg_img --mask $mask -O $out\n";

	} else {
		print "$bin --img.image $img_full --img.saturation_level $saturation_level --iebv_config $iebv_config -S $seg_img -O $out\n";
	}
}

sub getSegAndMask {
	my $img = shift;

	my ($mask, $seg) = ($img, $img);
	$mask =~ s/\.tif$/.png/i;

	return ($seg_dir."/".$seg, $mask_dir."/".$mask);
}

sub getOutput {
	my ($img) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output . '/' . $without_extension . ".png";
}




