#!/usr/bin/perl -w

use strict;

if (scalar @ARGV != 1) {
	print "usage: $0 <seg_dir>\n";
	print "where <seg_dir> denotes the directory for the segmentation\n";
	exit(1);
}

print "# generated with $0 $ARGV[0]\n";

my $input_base = '/net/cv/illum/barnard/';
my $seg_base = '/disks/data1/riess/mimorem/barnard/' . $ARGV[0] ."/";
my $output_base = '/disks/data1/riess/mimorem/barnard/specular/iebv/' . $ARGV[0] . '/';
my $iebv_config = '/disks/data1/riess/code/reflectance/lille/config/config_iebv_avg.txt';

# search segmentation directory for useful files
my @files = `find $seg_base -name "*.png"`;

if (scalar @files == 0) {
	print STDERR "unknown seg_dir $ARGV[0], no files found there, aborted.";
	print STDERR "check subdirectories of $seg_base for valid entries\n";
	exit(1);
}

my $saturation_level = 255;
my $bin = '/disks/data1/riess/code/build/bin/vole liebv';

my %dirs = ();
foreach my $img(@files) {
	chomp($img);
	my $dir = $img;
	$dir =~ s/^$seg_base//;
	$dir =~ s/^(.+)\/.+?$/$1/;
	$dir = $output_base . $dir;
	if ((!defined $dirs{$dir}) && (!-e $dir)) {
		$dirs{$dir} = 1;
	}
}

foreach my $dir(keys %dirs) {
	print "mkdir -p $dir\n";
}

foreach my $img(@files) {
	chomp($img);
	my ($dir) = ($img =~ /(.+)\/.+?/);
	my $img_suffix = $img;
	$img_suffix =~ s/^$seg_base//;
	my $img_full = $input_base . '/' . $img_suffix;
	$img_full =~ s/\.png$/.tif/;
	my $seg_img = $img;
	my $out = getOutput($img_suffix);
	print "$bin --img.image $img_full --img.saturation_level $saturation_level --iebv_config $iebv_config -S $seg_img -O $out\n";
}

sub getSeg {
	my $img = shift;

	my ($scene) = ($img =~ /^([^_]+)_/);

	return ($seg_base . "/" . $scene ."_seg.png");
}

sub getOutput {
	my ($img) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output_base . '/' . $without_extension . ".png"; # "_" . $saturation_level . ".png";
}




