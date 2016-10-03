#!/usr/bin/perl -w
use strict;

if (scalar @ARGV != 2) {
	print "usage: $0 <estimation_dir> <list_file_dir_and_prefix>\n";
	print "where <estimation_dir> contains localized illuminant estimates, named \$1.png\n";
	print "<list_file_dir_and_prefix> ist a filename prefix for collecting ground truth\n";
	print "and estimation files, e.g. /tmp/my_collection_. The grayworld-key is appended to this prefix.\n";
	exit(1);
}

my $vole = '/disks/data1/riess/code/build/bin/vole illumEvalPw';
my $GT_DIR = '/net/cv/illum/multi_michael/gt_pw/';

my $search_string_gt = $GT_DIR . "/*_gt.png";
my @gt_files = `ls -1 $search_string_gt`;


my $info_hash = {};

foreach my $gt_file(@gt_files) {
	chomp($gt_file);
	my $est_file_name = $gt_file;
	$est_file_name =~ s/_gt\.png$//i;
	$est_file_name =~ s/^.*\/(.+?)$/$1/i;

	my $est_file_with_path = $ARGV[0] . "/" . $est_file_name;
	my @estimation_cand = `ls -1 ${est_file_with_path}*.png`;
	if (scalar @estimation_cand == 0) {
		print STDERR "there exists a ground truth file \"$gt_file\", but no estimation file \"${est_file_with_path}*.png\"\n";
		next;
	}
	# start to examine these files
	foreach my $cand(@estimation_cand) {
		chomp($cand);
		my $identifier = $cand;
		$identifier =~ s/^$est_file_with_path//g;
		$identifier =~ s/\.png$//g;
		$info_hash->{$identifier} = [] unless (defined $info_hash->{$identifier});
		my $ref = $info_hash->{$identifier};
		push @$ref, [$gt_file, $cand];
		$info_hash->{$identifier} = $ref;
	}
}

foreach my $k(keys %$info_hash) {
	my $list_file = $ARGV[1] . $k . ".txt";
	open(LIST, ">$list_file") || die $!;
	my $ref = $info_hash->{$k};
	foreach my $entry(@$ref) {
		print LIST $entry->[0] . " " . $entry->[1] . "\n";
	}
	close LIST;
	my $last_part = $list_file;
	$last_part =~ s/^.*\/([^\/]+)$/$1/;
	print $vole . " --eval_filelist $list_file --per_file_median=1 >> result_${last_part}\n";
	print $vole . " --eval_filelist $list_file >> result_${last_part}\n";
}




