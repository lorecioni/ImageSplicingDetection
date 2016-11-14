#!/usr/bin/perl -w
#

my $patchx = 31;
my $patchy = 7;
my $sigma = 0.2;
my $k = 200;
#my $mask = "\"/home/riess/vole/trunk/iebv/img/ciurea_funt_mask.png\"";
my $mask = "\"/home/mm/experiments/data/color_constancy/ciurea_funt/ciurea_funt_mask.png\"";
my $root_dir = "\"/home/mm/experiments/data/color_constancy/ciurea_funt\"";
my $filelist = "\"/home/mm/experiments/data/color_constancy/ciurea_funt/file.lst\"";
my $ground_truth = "\"/home/mm/experiments/data/color_constancy/ciurea_funt/files_ground_truth.txt\"";

my $mask_file = "mask_file="; # masks out the gray ball
my $seg_k = "k=";
my $seg_sigma = "sigma=";

my $config_start = "
# funt test run - min_pixels, min_intensity changed
#[iebv]
#isGraphical=false
#input_file=
#output_directory=
#method=
verbose=0

number_patches=200
random_seed=0

# next param changed

max_intensity=0.98823529411764705882
# next param changed
min_intensity=.05882352941176470588
# next param changed
min_pixels=20
#max_std_dev_chroma=0.2
#min_sum_std_dev_intensities=0.03
#min_inverse_intensity_range=0.1
minimum_eigenvalue_ratio=0.2
minimum_eigenvector_slope=0.003
maximum_eigenvector_slope=10.0

# next param changed
#log_mask_size=9

#log_sigma=6.15225e-270
#edge_length_multiplier=2
";

my $config_end = "
[_seg_patches]
strategy=largest
size_x=$patchx
size_y=$patchy
number_tries=1000

[_histogram_voting_fusion]
force_relation=false

[_seg_felz]
min_size=15
#chroma_img=1 # FIXME reinclude that in seg_felz!
";

open(JOBS, ">jobs.sh") || die "jobs.sh: $!";

for ($sigma = 0.15; $sigma <= 0.4; $sigma += 0.05) {
	for ($k = 150; $k < 400; $k += 50) {
		my $filename = "";
		my $basename = "";
		$basename = "funt_config_" . sprintf("%.2f", $sigma) . "_${k}_${patchx}_${patchy}";
		$filename = $basename . ".txt";
		if (!-x $basename) {
			mkdir $basename;
		}
		open(OUT, ">$basename/$filename") || die "file $filename: $!";
			print OUT $config_start;
			print OUT $mask_file . $mask . "\n";
			print OUT $config_end;
			print OUT $seg_k . $k . "\n";
			print OUT $seg_sigma . sprintf("%.2f", $sigma) . "\n";
		close(OUT);
		print JOBS "bin/vole iic_funt --iebv_method ev_fhs --iebv_submethod random --n_estimates 5 --funt_root_dir=$root_dir --funt_filelist=$filelist --ground_truth=$ground_truth --iebv_config_file $basename/$filename -O $basename/\n";
	}
}


close(JOBS);
