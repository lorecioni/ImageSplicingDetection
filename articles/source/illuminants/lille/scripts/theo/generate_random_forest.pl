#!/usr/bin/perl -w

use strict;

# ./bin/vole lboosting --trained_tree /disks/data1/riess/mimorem/trained_boosting_tree_gw_variants_shi_gehler.dat -O /tmp/annotated.png --mask /net/cv/illum/multi_shida/mask/spdif_mask.png -S /net/cv/illum/multi_shida/seg_100_30/spdif_seg.png --img.saturation_level 6000 --img.image /net/cv/illum/multi_shida/img/spdif_b_r.png

print "# generated with $0\n";

my $input = '/net/cv/illum/gijsenij_liu/mixed_all/';
my $output = '/disks/data1/riess/mimorem/table_theo/diffuse/random_forest/wrong/';
my $trained_tree = '/disks/data1/riess/mimorem/trained_random_forest_gw_variants_shi_gehler.dat'; 

my $seg_dir  = '/net/cv/illum/gijsenij_liu/seg_50_10/';
my $mask_dir = '/net/cv/illum/gijsenij_liu/mask/';

my $bin = '/disks/data1/riess/code/build/bin/vole lforest';

my @image = `ls ${input}/*.tif | grep -v '_gt.png' | grep -v 'noise_mask.png' | grep -v '_weights.png'`;

foreach my $img_full(@image) {
	chomp($img_full);
	my $img = $img_full;
	$img =~ s/^.*\/(.+?\.tif)$/$1/;

	my ($seg_img, $mask) = getSegAndMask($img);

	my $out = getOutput($img);
	print "$bin --img.image $img_full --trained_tree $trained_tree -S $seg_img --mask $mask -O $out\n";
}

sub getSegAndMask {
	my $img = shift;

	my ($mask, $seg) = ($img, $img);
	$mask =~ s/\.tif$/.png/i;
	return ($seg_dir . "/" . $seg, $mask_dir . "/" . $mask);
}

sub getOutput {
	my ($img) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output . '/' . $without_extension . ".png";
}


