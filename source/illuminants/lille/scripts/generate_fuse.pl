#!/usr/bin/perl -w

use strict;

if ((scalar @ARGV != 1) || ( (lc($ARGV[0]) ne 'avg') && (lc($ARGV[0]) ne 'select'))){
	print "usage: $0 {avg|select}\n";
	print "  here, 'avg' is fusion by averaging both estimates,\n";
	print "        'select' always takes the specular, and only the diffuse if there is no specular estimate\n";
	exit(1);
}

print "# generated with $0 $ARGV[0]\n";


my $input_diff = '/disks/data1/riess/mimorem/table_shida/diffuse/boosting/gt_100/';
my $input_spec;
my $output = '/disks/data1/riess/mimorem/table_shida/fused_REPLACETHIS_diff_spec/gt_100/';

my $bin;
if (lc($ARGV[0]) eq 'avg') {
	$bin = '/disks/data1/riess/code/build/bin/vole lfuseAvgDiffSpec';
	$output =~ s/REPLACETHIS/avg/;
	$input_spec = '/disks/data1/riess/mimorem/table_shida/specular/vote/voting_avg_6000/';
} else {
	$bin = '/disks/data1/riess/code/build/bin/vole lfuseSelectDiffSpec';
	$output =~ s/REPLACETHIS/select/;
	$input_spec = '/disks/data1/riess/mimorem/table_shida/specular/vote/voting_harsh_6000/';
}

my @image = `ls $input_diff | grep '.png' | grep -v '_gt.png' | grep -v 'noise_mask.png' | grep -v '_weights.png'`;

foreach my $img(@image) {
	chomp($img);

	my $img_full = $input_diff . '/' . $img;

	my $img_w_o_ext = $img;
	$img_w_o_ext =~ s/^([^_]+_[^_]_[^_])_.*\.png$/$1/g;
	my $other_with_path = $input_spec . "/" . $img_w_o_ext;

	my @other_img_cand = `ls -1 ${other_with_path}*.png`;
	if (scalar @other_img_cand == 0) {
		print STDERR "there exists a diffuse-based file \"$img_full\", but no associated specular-based file \"${other_with_path}*.png\"\n";
		next;
	}
	if (scalar @other_img_cand > 1) {
		print STDERR "for diffuse-based file \"$img_full\", multiple potentially associated specular-based files matching \"${other_with_path}*.png\" found\n";
		next;
	}

	chomp($other_img_cand[0]);

	my $out = getOutput($img);
	print "$bin -I $img_full -J $other_img_cand[0] -O $out\n";
}


sub getOutput {
	my ($img) = @_;
	my ($without_extension) = ($img =~ /^([^\.]+)\./);
	return $output . '/' . $without_extension . ".png";
}


#-S /net/cv/illum/multi_shida/seg_100_30//bmug_seg.png --mask /net/cv/illum/multi_shida/mask//bmug_mask.png --n 1 --p 1 --sigma 1 -O /disks/data1/riess/mimorem/gw//bmug_b_r_0_-1_0.png


