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
my $output_base = '/disks/data1/riess/mimorem/barnard/gw/' . $ARGV[0];

# search segmentation directory for useful files
my @files = `find $seg_base -name "*.png"`;

#my $n = 1;
#my $p = 1;
#my $s = 1;
my @N = (0, 1, 2);
my @P = (-1, 1, 2);
my @S = (0, 1);



if (scalar @files == 0) {
	print STDERR "unknown seg_dir $ARGV[0], no files found there, aborted.";
	print STDERR "check subdirectories of $seg_base for valid entries\n";
	exit(1);
}

my $saturation_level = 255;
my $bin = '/disks/data1/riess/code/build/bin/vole lgrayworld';

my %dirs = ();

foreach my $n(@N) {
	foreach my $p(@P) {
		foreach my $s(@S) {
			foreach my $img(@files) {
				chomp($img);
				my $dir = $img;
				$dir =~ s/^$seg_base//;
				$dir =~ s/^(.+)\/.+?$/$1/;
				$dir = modifyOutputBase($output_base, $n, $p, $s) . "/" . $dir;
				if ((!defined $dirs{$dir}) && (!-e $dir)) {
					$dirs{$dir} = 1;
				}
			}
		}
	}
}

foreach my $dir(keys %dirs) {
	print "mkdir -p $dir\n";
}

foreach my $img(@files) {
	chomp($img);
	my $img_suffix = $img;
	$img_suffix =~ s/^$seg_base//;
	my $img_full = $input_base . '/' . $img_suffix;
	$img_full =~ s/\.png$/.tif/;
	my $seg_img = $img;
	foreach my $n(@N) {
		foreach my $p(@P) {
			foreach my $s(@S) {

				my $out = getOutput($img_suffix, $n, $p, $s);
				print "$bin --img.image $img_full --img.saturation_level $saturation_level --n $n --p $p --sigma $s -S $seg_img -O $out\n";

			}
		}
	}
}

sub getOutput {
	my ($img, $n, $p, $s) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return modifyOutputBase($output_base, $n, $p, $s) . '/' . $without_extension . "_" . $n . "_" . $p . "_" . $s . ".png"; # "_" . $saturation_level . ".png";
}

sub modifyOutputBase {
	my ($output_base, $n, $p, $s) = @_;
	$output_base =~ s/\/+$//g;
	return $output_base . "/" . $n . "_" . $p . "_" . $s . "/";
}




