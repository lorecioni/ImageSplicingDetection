#!/usr/bin/perl -w
use strict;

if (scalar @ARGV != 3) {
	print "usage: $0 <gt_dir> <estimation_dir> <list_file>\n";
	print "where <gt_dir> contains masked ground truth files, named (.*)_gt.png\n";
	print "and <estimation_dir> contains localized illuminant estimates, named \$1.png\n";
	print "<list_file> ist just a filename for collecting ground truth and estimation files.\n";
	exit(1);
}

my $vole = '/disks/data1/riess/code/build/bin/vole illumEvalPw';

my $search_string_gt = $ARGV[0] . "/*.png";
my @gt_files = `ls -1 $search_string_gt`;

open(LIST, ">$ARGV[2]") || die $!;

foreach my $gt_file(@gt_files) {
	chomp($gt_file);
	my $est_file_name = $gt_file;
	$est_file_name =~ s/^.*\/(.+?)\.png$/$1/i;

	my $est_file_with_path = $ARGV[1] . "/" . $est_file_name;
	my @estimation_cand = `ls -1 ${est_file_with_path}*.png`;
	if (scalar @estimation_cand == 0) {
		print STDERR "there exists a ground truth file \"$gt_file\", but no estimation file \"${est_file_with_path}*.png\"\n";
		next;
	}
	if (scalar @estimation_cand > 1) {
		print STDERR "for ground truth file \"$gt_file\", multiple potential estimation files \"${est_file_with_path}*.png\" found\n";
		print STDERR "in detail: \n";
		print STDERR "  " . join("\n  ", @estimation_cand) . "\n";

		my $min_length = 100000;
		my $min_word = "";
		foreach my $est(@estimation_cand) {
			if (length($est) < $min_length) {
				$min_length = length($est);
				$min_word = $est;
			}
		}
		$estimation_cand[0] = $min_word;
		print STDERR "\n Because this is Theos dataset, I took the shortest file, namely $min_word\n";
	}
	chomp($estimation_cand[0]);
	print LIST $gt_file . " " . $estimation_cand[0] . "\n";
}

close LIST;

print $vole . " --eval_filelist $ARGV[2] --per_file_median=1\n";
print $vole . " --eval_filelist $ARGV[2]\n";


