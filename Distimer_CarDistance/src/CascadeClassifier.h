#pragma once

#include <opencv/cv.h>
#include <opencv2/objdetect/objdetect.hpp>

namespace DemoCarDistance
{
	class CCascadeClassifier
	{
	public:
		/**
		 * @brief Default constructor
		 */
		CCascadeClassifier(void);

		/**
		 * @brief Destructor
		 */
		virtual ~CCascadeClassifier(void);

		/**
		 * @brief Gets a new instance of CvHaarClassifierCascade. 
		 * The caller is responsible for release the instance.
		 *
		 * @return CvHaarClassifierCascade instance.
		 */
		CvHaarClassifierCascade* GetClassifierCascade();

	private:
		/** 
		 * @brief Initializes data for instance's members.
		 */
		void Init();

		/**
		 * @brief Gets the number of features in a specified stage.
		 *
		 * @param [in] stageIndex stage index.
		 * @return Number of features in this stage.
		 */
		int GetNumberOfFeaturesInStage(const int stageIndex);

		/**
		 * @brief Sets values for a specified stage.
		 *
		 * @param [out] stageClassifierRef Reference to the stage classifier
		 * @param [in] stageIndex Stage index
		 * @param [io] classifierIndex Classifier index used to idenfify stage's classifiers
		 * @param [io] featureIndex Feature index used to identify features of stage's classifiers
		 * @param [io] featureWeightIndex Weight index of features
		 */
		void SetValuesForStage(CvHaarStageClassifier **stageClassifierRef, const int stageIndex, int &classifierIndex, int &featureIndex, int &featureWeightIndex);
		
		/**
		 * @brief Sets values for a specified classifier.
		 *
		 * @param [out] classifierRef Reference to the classifier
		 * @param [in] classifierIndex Classifier index
		 * @param [io] featureIndex Feature index used to identify features of stage's classifiers
		 * @param [io] featureWeightIndex Weight index of features
		 */
		void SetValuesForClassifier(CvHaarClassifier **classifierRef, const int classifierIndex, int &featureIndex, int &featureWeightIndex);
		
		/**
		 * @brief Sets values for a specified haar feature.
		 *
		 * @param [out] haarFeatureRef Reference to the haar feature
		 * @param [in] featureIndex Feature index
		 * @param [io] featureWeightIndex Weight index of features
		 */
		void SetValuesForFeature(CvHaarFeature **haarFeatureRef, const int featureIndex, int &featureWeightIndex);

	private:
		bool m_initialized;    ///< Indicates whether the instance has been initialized or not

		///< Cascade data members
		int m_numOfStages, m_flags, m_windowWidth, m_windowHeight;
		const int *m_numStageNodes, *m_numStageChild, *m_stageNexts, *m_stageParents;
		const float *m_stageThresholds;

		const int *m_classifierCounts;
		float *m_classifierAlphas, *m_classifierThresholds;
		int *m_classifierLefts, *m_classifierRights;

		const int *m_featureTilted, *m_featureRects;
		const float *m_featureWeights;
	};
}