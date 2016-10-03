#include <sstream>
#include <vector>
#include <cassert>
#include "db_index_shida.h"

namespace iread {

DbIndexShida::DbIndexShida(bool sharpenedPreMult, bool sharpenedPostMult)
   : nScenes(11), nLights(4),
	input_raw_data_path("/net/cv/illum/multi_shida/orig_tiff/"), raw_ground_truth_path("/net/cv/illum/multi_shida/orig_gt/"),
	descriptive_data_path("/net/cv/illum/multi_shida/img/"), descriptive_ground_truth_path("/net/cv/illum/multi_shida/gt/"),
	sharpenedPreMult(sharpenedPreMult), sharpenedPostMult(sharpenedPostMult)
{
	this->mask_path = "/net/cv/illum/multi_shida/mask/";
	if (sharpenedPreMult) {
		input_raw_data_path = "/net/cv/illum/multi_shida/orig_tiff_shpre/";
		raw_ground_truth_path = "/net/cv/illum/multi_shida/orig_gt_shpre/";
		descriptive_data_path = "/net/cv/illum/multi_shida/img_shpre/";
		descriptive_ground_truth_path = "/net/cv/illum/multi_shida/gt_shpre/";
	} else {
		if (sharpenedPostMult) {
			input_raw_data_path = "/net/cv/illum/multi_shida/orig_tiff_shpst/";
			raw_ground_truth_path = "/net/cv/illum/multi_shida/orig_gt_shpst/";
			descriptive_data_path = "/net/cv/illum/multi_shida/img_shpst/";
			descriptive_ground_truth_path = "/net/cv/illum/multi_shida/gt_shpst/";
		}
	}
	init();
	initOffsets();
}

DbIndexShida::DbIndexShida(std::string input_raw_data_path, std::string raw_gt_path, std::string descriptive_data_path, std::string descriptive_gt_path)
	: nScenes(11), nLights(4),
	input_raw_data_path(input_raw_data_path), raw_ground_truth_path(raw_gt_path),
	descriptive_data_path(descriptive_data_path), descriptive_ground_truth_path(descriptive_gt_path)
{
	this->mask_path = "/net/cv/illum/multi_shida/mask/";
	init();
	initOffsets();
}

DbIndexShida::~DbIndexShida()
{
	for (int s = 0; s < nScenes; ++s) {
		for (int left = 0; left < nLights; ++left) {
			for (int right = 0; right < nLights; ++right) {
				if (table[s][left][right] != NULL)
					delete table[s][left][right];
				for (int l2 = 0; l2 < nLights; ++l2) {
					for (int r2 = 0; r2 < nLights; ++r2) {
						if (offsets[s][left][right][l2][r2] != NULL)
							delete offsets[s][left][right][l2][r2];
					}
				}
			}
		}
	}
}

void DbIndexShida::setRawDataPath(std::string raw_data_path)
{
	this->input_raw_data_path = raw_data_path;
}

void DbIndexShida::setRawGroundTruthPath(std::string raw_ground_truth_path)
{
	this->raw_ground_truth_path = raw_ground_truth_path;
}

void DbIndexShida::setDescriptiveDataPath(std::string descriptive_data_path)
{
	this->descriptive_data_path = descriptive_data_path;
}

void DbIndexShida::setMaskPath(std::string mask_path)
{
	this->mask_path = mask_path;
}

void DbIndexShida::setDescriptiveGroundTruthPath(std::string descriptive_ground_truth_path)
{
	this->descriptive_ground_truth_path = descriptive_ground_truth_path;
}

std::string DbIndexShida::sceneToString(SCENE scene)
{
	switch (scene) {
		case GT:     return std::string("gt");
		case MSPEC:  return std::string("mspec");
		case LION:   return std::string("lion");
		case DIFF:   return std::string("diff");
		case SPDIF:  return std::string("spdif");
		case YMUG:   return std::string("ymug");
		case PMUG:   return std::string("pmug");
		case BMUG:   return std::string("bmug");
		case BPMUG:  return std::string("bpmug");
		case BYMUG:  return std::string("bymug");
		case SPDIF2: return std::string("spdif2");
		default:
			std::cerr << "DbIndexShida::sceneToString(): unknown scene " << scene << "!" << std::endl;
			return std::string("none");
	};
}

std::string DbIndexShida::illumToString(ILLUM illum)
{
	switch (illum) {
		case NO: return std::string("n");
		case W:  return std::string("w");
		case R:  return std::string("r");
		case B:  return std::string("b");
		default:
			std::cerr << "DbIndexShida::illumToString(): unknown illuminant " << illum << "!" << std::endl;
			return std::string("u");
	};
}

SCENE DbIndexShida::stringToScene(std::string scene)
{
	if (scene.compare("gt") == 0) return GT;
	if (scene.compare("mspec") == 0) return MSPEC;
	if (scene.compare("lion") == 0) return LION;
	if (scene.compare("diff") == 0) return DIFF;
	if (scene.compare("spdif") == 0) return SPDIF;
	if (scene.compare("ymug") == 0) return YMUG;
	if (scene.compare("pmug") == 0) return PMUG;
	if (scene.compare("bmug") == 0) return BMUG;
	if (scene.compare("bpmug") == 0) return BPMUG;
	if (scene.compare("bymug") == 0) return BYMUG;
	if (scene.compare("spdif2") == 0) return SPDIF2;

	std::cerr << "DbIndexShida::stringToScene(): unknown scene " << scene << "!" << std::endl;
	return GT; // FIXME introduce "NONE" scene or so
}

ILLUM DbIndexShida::stringToIllum(std::string illum)
{
	if (illum.compare("n") == 0) return NO;
	if (illum.compare("w") == 0) return W;
	if (illum.compare("r") == 0) return R;
	if (illum.compare("b") == 0) return B;
	std::cerr << "DbIndexShida::stringToIllum(): unknown illuminant " << illum << "!" << std::endl;
	return NO;
}

illumestimators::Illum DbIndexShida::getIllum(ILLUM illum) {
	assert((int)illum < 4);
	if (sharpenedPreMult) {
		return illums_shpre[(int)illum];
	} else {
		if (sharpenedPostMult) {
			return illums_shpst[(int)illum];
		} else {
			return illums[(int)illum];
		}
	}
}


std::vector<SCENE> DbIndexShida::getScenes()
{
	std::vector<SCENE> scenes;
	scenes.push_back(GT); scenes.push_back(MSPEC); scenes.push_back(LION);
	scenes.push_back(DIFF); scenes.push_back(SPDIF); scenes.push_back(YMUG);
	scenes.push_back(PMUG); scenes.push_back(BMUG); scenes.push_back(BPMUG);
	scenes.push_back(BYMUG); scenes.push_back(SPDIF2);
	return scenes;
}

std::vector<ILLUM> DbIndexShida::getIllums()
{
	std::vector<ILLUM> illums;
	illums.push_back(NO); illums.push_back(W); illums.push_back(R); illums.push_back(B);
	return illums;
}

int DbIndexShida::getNumberScenes()
{
	return nScenes;
}

int DbIndexShida::getNumberIlluminants()
{
	return nLights;
}

cv::Point *DbIndexShida::offset(SCENE scene, ILLUM left1, ILLUM right1, ILLUM left2, ILLUM right2)
{
	return offsets[scene][left1][right1][left2][right2];
}

std::string DbIndexShida::gtLookupWithPath(SCENE scene, ILLUM left, ILLUM right, bool no_censorship)
{
	if (table[scene][left][right] == NULL) return std::string();
	if (!no_censorship) {
		if (((left == R) && (right == NO)) || ((left == NO) && (right == R))) return std::string(); // FIXME remove this when gt is available
	}

	std::stringstream full_path;
	full_path << raw_ground_truth_path << *table[scene][left][right];

	return full_path.str();
}

std::string DbIndexShida::descriptiveMaskLookupWithPath(SCENE scene, ILLUM left, ILLUM right)
{
	if (table[scene][left][right] == NULL) return std::string();
	if (((left == R) && (right == NO)) || ((left == NO) && (right == R))) return std::string(); // FIXME remove this when gt is available

	std::stringstream full_path;
	full_path << mask_path << "/" << DbIndexShida::sceneToString(scene) << "_mask.png";

	return full_path.str();
}

std::string *DbIndexShida::gtLookup(SCENE scene, ILLUM left, ILLUM right, bool no_censorship)
{
	if (!no_censorship) {
		if (((left == R) && (right == NO)) || ((left == NO) && (right == R))) return NULL; // FIXME remove this when gt is available
	}
	return table[scene][left][right];
}

std::string DbIndexShida::descriptiveGtLookup(SCENE scene, ILLUM left, ILLUM right)
{
	std::stringstream s;
	if (descriptive_ground_truth_path.length() > 0) s << descriptive_ground_truth_path << "/";
	s << DbIndexShida::sceneToString(scene) << "_"
		<< DbIndexShida::illumToString(left) << "_" << DbIndexShida::illumToString(right) << "_gt.png";

	return s.str();
}

std::string DbIndexShida::descriptiveLookup(SCENE scene, ILLUM left, ILLUM right)
{
	std::stringstream s;
	if (descriptive_data_path.length() > 0) s << descriptive_data_path << "/";
	s << DbIndexShida::sceneToString(scene) << "_"
		<< DbIndexShida::illumToString(left) << "_" << DbIndexShida::illumToString(right) << ".png";

	return s.str();
}

std::string DbIndexShida::lookupWithPath(SCENE scene, ILLUM left, ILLUM right)
{
	if (table[scene][left][right] == NULL) return std::string();

	std::stringstream full_path;
	full_path << input_raw_data_path << *table[scene][left][right];

	return full_path.str();
}

std::string *DbIndexShida::lookup(SCENE scene, ILLUM left, ILLUM right)
{
	return table[scene][left][right];
}

void DbIndexShida::init()
{

	for (int s = 0; s < nScenes; ++s) {
		for (int left = 0; left < nLights; ++left) {
			for (int right = 0; right < nLights; ++right) {
				table[s][left][right] = NULL;
			}
		}
	}

	table[GT][NO][ W] = new std::string("IMG03221.tiff");
	table[GT][NO][ R] = new std::string("IMG03222.tiff");
	table[GT][NO][ B] = new std::string("IMG03223.tiff");
	table[GT][ W][NO] = new std::string("IMG03224.tiff");
	table[GT][ R][NO] = new std::string("IMG03225.tiff");
	table[GT][ B][NO] = new std::string("IMG03226.tiff");
	table[GT][ W][ W] = new std::string("IMG03227.tiff");
	table[GT][ W][ B] = new std::string("IMG03228.tiff");
	table[GT][ W][ R] = new std::string("IMG03229.tiff");
	table[GT][ R][ B] = new std::string("IMG03231.tiff");
	table[GT][ R][ W] = new std::string("IMG03232.tiff");
	table[GT][ B][ W] = new std::string("IMG03233.tiff");
	table[GT][ B][ R] = new std::string("IMG03234.tiff");
	table[GT][ B][ B] = new std::string("IMG03390.tiff");

	table[MSPEC][NO][ W] = new std::string("IMG03235.tiff");
	table[MSPEC][NO][ R] = new std::string("IMG03236.tiff");
	table[MSPEC][NO][ B] = new std::string("IMG03237.tiff");
	table[MSPEC][ W][NO] = new std::string("IMG03238.tiff");
	table[MSPEC][ R][NO] = new std::string("IMG03239.tiff");
	table[MSPEC][ B][NO] = new std::string("IMG03240.tiff");
	table[MSPEC][ W][ W] = new std::string("IMG03241.tiff");
	table[MSPEC][ W][ B] = new std::string("IMG03242.tiff");
	table[MSPEC][ W][ R] = new std::string("IMG03243.tiff");
	table[MSPEC][ R][ B] = new std::string("IMG03244.tiff");
	table[MSPEC][ R][ W] = new std::string("IMG03245.tiff");
	table[MSPEC][ B][ W] = new std::string("IMG03246.tiff");
	table[MSPEC][ B][ R] = new std::string("IMG03247.tiff");
	table[MSPEC][ B][ B] = new std::string("IMG03248.tiff");

	table[LION][NO][ W] = new std::string("IMG03249.tiff");
	table[LION][NO][ R] = new std::string("IMG03250.tiff");
	table[LION][NO][ B] = new std::string("IMG03251.tiff");
	table[LION][ W][NO] = new std::string("IMG03252.tiff");
	table[LION][ R][NO] = new std::string("IMG03253.tiff");
	table[LION][ B][NO] = new std::string("IMG03254.tiff");
	table[LION][ W][ W] = new std::string("IMG03255.tiff");
	table[LION][ W][ B] = new std::string("IMG03257.tiff");
	table[LION][ W][ R] = new std::string("IMG03256.tiff");
	table[LION][ R][ B] = new std::string("IMG03258.tiff");
	table[LION][ R][ W] = new std::string("IMG03259.tiff");
	table[LION][ B][ W] = new std::string("IMG03260.tiff");
	table[LION][ B][ R] = new std::string("IMG03261.tiff");
	table[LION][ B][ B] = new std::string("IMG03262.tiff");

	table[DIFF][NO][ W] = new std::string("IMG03263.tiff");
	table[DIFF][NO][ R] = new std::string("IMG03264.tiff");
	table[DIFF][NO][ B] = new std::string("IMG03265.tiff");
	table[DIFF][ W][NO] = new std::string("IMG03266.tiff");
	table[DIFF][ R][NO] = new std::string("IMG03267.tiff");
	table[DIFF][ B][NO] = new std::string("IMG03268.tiff");
	table[DIFF][ W][ W] = new std::string("IMG03269.tiff");
	table[DIFF][ W][ B] = new std::string("IMG03272.tiff");
	table[DIFF][ W][ R] = new std::string("IMG03271.tiff");
	table[DIFF][ R][ B] = new std::string("IMG03273.tiff");
	table[DIFF][ R][ W] = new std::string("IMG03274.tiff");
	table[DIFF][ B][ W] = new std::string("IMG03275.tiff");
	table[DIFF][ B][ R] = new std::string("IMG03276.tiff");
	table[DIFF][ B][ B] = new std::string("IMG03277.tiff");

	table[SPDIF][NO][ W] = new std::string("IMG03278.tiff");
	table[SPDIF][NO][ R] = new std::string("IMG03279.tiff");
	table[SPDIF][NO][ B] = new std::string("IMG03280.tiff");
	table[SPDIF][ W][NO] = new std::string("IMG03281.tiff");
	table[SPDIF][ R][NO] = new std::string("IMG03282.tiff");
	table[SPDIF][ B][NO] = new std::string("IMG03283.tiff");
	table[SPDIF][ W][ W] = new std::string("IMG03284.tiff");
	table[SPDIF][ W][ B] = new std::string("IMG03286.tiff");
	table[SPDIF][ W][ R] = new std::string("IMG03285.tiff");
	table[SPDIF][ R][ B] = new std::string("IMG03287.tiff");
	table[SPDIF][ R][ W] = new std::string("IMG03288.tiff");
	table[SPDIF][ B][ W] = new std::string("IMG03289.tiff");
	table[SPDIF][ B][ R] = new std::string("IMG03290.tiff");
	table[SPDIF][ B][ B] = new std::string("IMG03291.tiff");

	table[YMUG][NO][ W] = new std::string("IMG03295.tiff");
	table[YMUG][NO][ R] = new std::string("IMG03296.tiff");
	table[YMUG][NO][ B] = new std::string("IMG03297.tiff");
	table[YMUG][ W][NO] = new std::string("IMG03298.tiff");
	table[YMUG][ R][NO] = new std::string("IMG03299.tiff");
	table[YMUG][ B][NO] = new std::string("IMG03300.tiff");
	table[YMUG][ W][ W] = new std::string("IMG03301.tiff");
	table[YMUG][ W][ B] = new std::string("IMG03303.tiff");
	table[YMUG][ W][ R] = new std::string("IMG03302.tiff");
	table[YMUG][ R][ B] = new std::string("IMG03304.tiff");
	table[YMUG][ R][ W] = new std::string("IMG03305.tiff");
	table[YMUG][ B][ W] = new std::string("IMG03306.tiff");
	table[YMUG][ B][ R] = new std::string("IMG03307.tiff");
	table[YMUG][ B][ B] = new std::string("IMG03308.tiff");

//	table[PMUG][NO][ W] = new std::string("IMG03313.tiff"); // file appears to be damaged
	table[PMUG][NO][ R] = new std::string("IMG03314.tiff");
	table[PMUG][NO][ B] = new std::string("IMG03315.tiff");
	table[PMUG][ W][NO] = new std::string("IMG03316.tiff");
	table[PMUG][ R][NO] = new std::string("IMG03317.tiff");
	table[PMUG][ B][NO] = new std::string("IMG03318.tiff");
	table[PMUG][ W][ W] = new std::string("IMG03319.tiff");
	table[PMUG][ W][ B] = new std::string("IMG03321.tiff");
	table[PMUG][ W][ R] = new std::string("IMG03320.tiff");
	table[PMUG][ R][ B] = new std::string("IMG03323.tiff");
	table[PMUG][ R][ W] = new std::string("IMG03324.tiff");
	table[PMUG][ B][ W] = new std::string("IMG03325.tiff");
	table[PMUG][ B][ R] = new std::string("IMG03326.tiff");
	table[PMUG][ B][ B] = new std::string("IMG03327.tiff");

	table[BMUG][NO][ W] = new std::string("IMG03329.tiff");
	table[BMUG][NO][ R] = new std::string("IMG03330.tiff");
	table[BMUG][NO][ B] = new std::string("IMG03331.tiff");
	table[BMUG][ W][NO] = new std::string("IMG03332.tiff");
	table[BMUG][ R][NO] = new std::string("IMG03333.tiff");
	table[BMUG][ B][NO] = new std::string("IMG03334.tiff");
	table[BMUG][ W][ W] = new std::string("IMG03335.tiff");
	table[BMUG][ W][ B] = new std::string("IMG03337.tiff");
	table[BMUG][ W][ R] = new std::string("IMG03336.tiff");
	table[BMUG][ R][ B] = new std::string("IMG03338.tiff");
	table[BMUG][ R][ W] = new std::string("IMG03339.tiff");
	table[BMUG][ B][ W] = new std::string("IMG03340.tiff");
	table[BMUG][ B][ R] = new std::string("IMG03341.tiff");
	table[BMUG][ B][ B] = new std::string("IMG03342.tiff");

	table[BPMUG][NO][ W] = new std::string("IMG03344.tiff");
	table[BPMUG][NO][ R] = new std::string("IMG03345.tiff");
	table[BPMUG][NO][ B] = new std::string("IMG03346.tiff");
	table[BPMUG][ W][NO] = new std::string("IMG03347.tiff");
	table[BPMUG][ R][NO] = new std::string("IMG03348.tiff");
	table[BPMUG][ B][NO] = new std::string("IMG03349.tiff");
	table[BPMUG][ W][ W] = new std::string("IMG03350.tiff");
	table[BPMUG][ W][ B] = new std::string("IMG03352.tiff");
	table[BPMUG][ W][ R] = new std::string("IMG03351.tiff");
	table[BPMUG][ R][ B] = new std::string("IMG03353.tiff");
	table[BPMUG][ R][ W] = new std::string("IMG03354.tiff");
	table[BPMUG][ B][ W] = new std::string("IMG03355.tiff");
	table[BPMUG][ B][ R] = new std::string("IMG03356.tiff");
	table[BPMUG][ B][ B] = new std::string("IMG03357.tiff");

	table[BYMUG][NO][ W] = new std::string("IMG03362.tiff");
	table[BYMUG][NO][ R] = new std::string("IMG03363.tiff");
	table[BYMUG][NO][ B] = new std::string("IMG03364.tiff");
	table[BYMUG][ W][NO] = new std::string("IMG03365.tiff");
	table[BYMUG][ R][NO] = new std::string("IMG03366.tiff");
	table[BYMUG][ B][NO] = new std::string("IMG03367.tiff");
	table[BYMUG][ W][ W] = new std::string("IMG03368.tiff");
	table[BYMUG][ W][ B] = new std::string("IMG03370.tiff");
	table[BYMUG][ W][ R] = new std::string("IMG03369.tiff");
	table[BYMUG][ R][ B] = new std::string("IMG03371.tiff");
	table[BYMUG][ R][ W] = new std::string("IMG03372.tiff");
	table[BYMUG][ B][ W] = new std::string("IMG03373.tiff");
	table[BYMUG][ B][ R] = new std::string("IMG03374.tiff");
	table[BYMUG][ B][ B] = new std::string("IMG03375.tiff");

	table[SPDIF2][NO][ W] = new std::string("IMG03376.tiff");
	table[SPDIF2][NO][ R] = new std::string("IMG03377.tiff");
	table[SPDIF2][NO][ B] = new std::string("IMG03378.tiff");
	table[SPDIF2][ W][NO] = new std::string("IMG03379.tiff");
	table[SPDIF2][ R][NO] = new std::string("IMG03380.tiff");
	table[SPDIF2][ B][NO] = new std::string("IMG03381.tiff");
	table[SPDIF2][ W][ W] = new std::string("IMG03382.tiff");
	table[SPDIF2][ W][ B] = new std::string("IMG03384.tiff");
	table[SPDIF2][ W][ R] = new std::string("IMG03383.tiff");
	table[SPDIF2][ R][ B] = new std::string("IMG03385.tiff");
	table[SPDIF2][ R][ W] = new std::string("IMG03386.tiff");
	table[SPDIF2][ B][ W] = new std::string("IMG03387.tiff");
	table[SPDIF2][ B][ R] = new std::string("IMG03388.tiff");
	table[SPDIF2][ B][ B] = new std::string("IMG03389.tiff");

	illums[(int)NO] = illumestimators::Illum(); // no illum
	illums[(int) W] = illumestimators::Illum(0.395077, 0.347945, 0.256978); // white
	illums[(int) R] = illumestimators::Illum(0.439926, 0.339551, 0.220523); // red
	illums[(int) B] = illumestimators::Illum(0.353101, 0.353849, 0.293049); // blue

	// FIXME manually determined from bright pixels:
	illums[(int) W] = illumestimators::Illum(0.404475, 0.344234, 0.251291); // white
	illums[(int) R] = illumestimators::Illum(0.458194, 0.334448, 0.207358); // red
	illums[(int) B] = illumestimators::Illum(0.365217, 0.347826, 0.286956); // blue

	// FIXME manually determined from darker pixels:
	illums[(int) W] = illumestimators::Illum(0.393728, 0.348432, 0.257840); // white
	illums[(int) R] = illumestimators::Illum(0.440678, 0.338983, 0.220339); // red
	illums[(int) B] = illumestimators::Illum(0.350877, 0.357895, 0.291228); // blue

	// FIXME SUPER MANUALLY determined from OUTPUT IMAGES:
	illums[(int) W] = illumestimators::Illum(0.388728, 0.348432, 0.262840); // white
	illums[(int) R] = illumestimators::Illum(0.428678, 0.343983, 0.227339); // red
	illums[(int) B] = illumestimators::Illum(0.340877, 0.357895, 0.301228); // blue

	illums[(int) W] = illumestimators::Illum(0.388728, 0.348432, 0.262840); // white
	illums[(int) R] = illumestimators::Illum(0.428678, 0.343983, 0.227339); // red
	illums[(int) B] = illumestimators::Illum(0.340877, 0.357895, 0.301228); // blue

	// SHARPENED PRE-MULTIPLICATION
	illums_shpre[(int)NO] = illumestimators::Illum(); // no illum
	illums_shpre[(int) W] = illumestimators::Illum(0.368662, 0.22628,  0.405059); // white
	illums_shpre[(int) R] = illumestimators::Illum(0.412981, 0.224986, 0.362033); // red
	illums_shpre[(int) B] = illumestimators::Illum(0.303797, 0.226448, 0.469756); // blue

	// SHARPENED POST-MULTIPLICATION
	illums_shpst[(int)NO] = illumestimators::Illum(); // no illum
	illums_shpst[(int) W] = illumestimators::Illum(0.453566, 0.28388, 0.262554); // white
	illums_shpst[(int) R] = illumestimators::Illum(0.559823, 0.226087, 0.21409); // red
	illums_shpst[(int) B] = illumestimators::Illum(0.26809, 0.376353, 0.355557); // blue
}

void DbIndexShida::setOffset(SCENE scene, ILLUM left1, ILLUM right1, ILLUM left2, ILLUM right2, int xOffset, int yOffset)
{
	offsets[scene][left1][right1][left2][right2] = new cv::Point();
	offsets[scene][left1][right1][left2][right2]->x = xOffset;
	offsets[scene][left1][right1][left2][right2]->y = yOffset;

	offsets[scene][left2][right2][left1][right1] = new cv::Point();
	offsets[scene][left2][right2][left1][right1]->x = -xOffset;
	offsets[scene][left2][right2][left1][right1]->y = -yOffset;
}

void DbIndexShida::initOffsets()
{

	for (int s = 0; s < nScenes; ++s) {
		for (int left = 0; left < nLights; ++left) {
			for (int right = 0; right < nLights; ++right) {
				for (int l2 = 0; l2 < nLights; ++l2) {
					for (int r2 = 0; r2 < nLights; ++r2) {
						offsets[s][left][right][l2][r2] = NULL;
					}
				}
			}
		}
	}
//	offsets[SPDIF2][ B][ B][ B][ B] = new cv::Point(1, 1);

}



}
