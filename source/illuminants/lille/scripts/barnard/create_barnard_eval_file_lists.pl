#!/usr/bin/perl -w

if (scalar @ARGV != 2) {
	print STDERR "usage: $0 <estimation_directory> <output_prefix>

<estimation_directory> points to a path of image estimates. The first
subdirectories have to be \"barnard_dielectric_specularities\"
\"barnard_fluorescent_surfaces\" \"barnard_metallic_specularities\"
\"barnard_minimal_specularities\".

<output_prefix> contains the directory and the file name prefix for the four
created file lists (one for each part of barnards database).

You can evaluate this file list using \"vole illumEvalPw\".
";
	exit(1);
}

my $gt_dir = '/net/cv/illum/barnard/';
my @dirs = qw(barnard_dielectric_specularities barnard_fluorescent_surfaces barnard_metallic_specularities barnard_minimal_specularities);

my $barnard_estimation_dir = $ARGV[0];

my $failure = 0;
foreach my $d(@dirs) {
	if (! -e $barnard_estimation_dir . '/' . $d) {
		print STDERR "directory $d expected below $barnard_estimation_dir\n";
		$failure = 1;
	}
}
exit(1) if ($failure);

# find ground truth
foreach my $d(@dirs) {
	my $search_dir = $gt_dir . "/" . $d;
	my @input = `find $search_dir -name "*.rgb"`;
	my $out_file = $ARGV[1] . $d . ".txt";

	if (scalar(@input) == 0) {
		print STDERR "no *.rgb-files found below $search_dir, skipped\n";
		next;
	}
	
	open OUT, ">$out_file" or die $!;
	foreach my $in(@input) {
		chomp($in);
		open IN, "<$in" or ( warn "unable to open file $in for reading, skipped." and next );
		my $line = <IN>;
		my ($r, $g, $b) = split(/\s+/, $line);
		close IN;

		my $only_suffix = $in;
		$only_suffix =~ s/^$gt_dir//;
		my $output_file = $barnard_estimation_dir . "/" . $only_suffix;
		$output_file =~ s/\.rgb$/.png/;
		$output_file =~ s/\/+/\//g;
		if (! -e $output_file) {
			warn "estimation file $output_file does not exist, skipped.";
			next;
		}
		print OUT "$output_file $r $g $b\n";
	}
	close OUT;
	print "wrote $out_file\n";
}
