#ifndef IREAD_DB_INDEX_SHIDA_H
#define IREAD_DB_INDEX_SHIDA_H

#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include "illum.h"

namespace iread {

typedef enum {
	GT     =  0, // ground truth
	MSPEC  =  1, // many specular
	LION   =  2, // lion
	DIFF   =  3, // diffuse
	SPDIF  =  4, // specular & diffuse
	YMUG   =  5, // yellow mug
	PMUG   =  6, // pink mug
	BMUG   =  7, // blue mug
	BPMUG  =  8, // blue & pink mug
	BYMUG  =  9, // blue & yellow mug
	SPDIF2 = 10 // specular & diffuse 2
} SCENE;

typedef enum {
	NO = 0,
	W  = 1,
	R  = 2,
	B  = 3
} ILLUM;

#ifndef ILLUMString
#define ILLUMString {"NO", "W", "R", "B"}
#endif 


class DbIndexShida {
	public:
		DbIndexShida(bool sharpenedPreMult = false, bool sharpenedPostMult = false);
		DbIndexShida(std::string input_raw_data_path, std::string raw_ground_truth_path, std::string descriptive_data_path, std::string descriptive_gt_path);
		~DbIndexShida();

		void setRawDataPath(std::string raw_data_path);
		void setRawGroundTruthPath(std::string raw_ground_truth_path);
		void setDescriptiveDataPath(std::string descriptive_data_path);
		void setDescriptiveGroundTruthPath(std::string descriptive_ground_truth_path);
		void setMaskPath(std::string mask_path);

		int getNumberScenes();
		int getNumberIlluminants();

		std::string descriptiveLookup(SCENE scene, ILLUM left, ILLUM right);
		std::string descriptiveGtLookup(SCENE scene, ILLUM left, ILLUM right);

		std::string lookupWithPath(SCENE scene, ILLUM left, ILLUM right);
		std::string *lookup(SCENE scene, ILLUM left, ILLUM right);

		std::string descriptiveMaskLookupWithPath(SCENE scene, ILLUM left, ILLUM right);

		std::string gtLookupWithPath(SCENE scene, ILLUM left, ILLUM right, bool no_censorship = false);
		std::string *gtLookup(SCENE scene, ILLUM left, ILLUM right, bool no_censorship = false);

		cv::Point *offset(SCENE scene, ILLUM left1, ILLUM right1, ILLUM left2, ILLUM right2);
		void setOffset(SCENE scene, ILLUM left1, ILLUM right1, ILLUM left2, ILLUM right2, int xOffset, int yOffset);

		std::vector<SCENE> getScenes();
		std::vector<ILLUM> getIllums();

		illumestimators::Illum getIllum(ILLUM illum);

		static std::string sceneToString(SCENE scene);
		static std::string illumToString(ILLUM illum);

		static SCENE stringToScene(std::string scene); // scene should be in lowercase
		static ILLUM stringToIllum(std::string illum); // scene should be in lowercase

	private:
		void init();
		void initOffsets();

		int nScenes;
		int nLights;
		std::string *table[11][4][4];
		cv::Point *offsets[11][4][4][4][4];
		std::string input_raw_data_path;
		std::string raw_ground_truth_path;
		std::string descriptive_data_path;
		std::string descriptive_ground_truth_path;
		std::string mask_path;
		
		illumestimators::Illum illums[4];
		illumestimators::Illum illums_shpst[4];
		illumestimators::Illum illums_shpre[4];

		bool sharpenedPreMult;
		bool sharpenedPostMult;
};

}

#endif // IREAD_DB_INDEX_SHIDA_H
