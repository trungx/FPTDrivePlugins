#include "Cascade.h"
#include "CascadeClassifier.h"

using namespace DemoCarDistance;
//using namespace CarCascade;

CCascadeClassifier::CCascadeClassifier(void):
	m_initialized(false)
{
}

CCascadeClassifier::~CCascadeClassifier(void)
{
}

void CCascadeClassifier::Init()
{
	if (!m_initialized)
	{
		m_initialized = true;
	} 
	else
	{
		return;
	}

	m_numOfStages = CarCascade::NumOfStages;
	m_flags = CarCascade::Flags;
	m_windowWidth = CarCascade::WindowWidth;
	m_windowHeight = CarCascade::WindowHeight;

	m_numStageNodes = &CarCascade::NumStageNodes[0];
	m_numStageChild = &CarCascade::NumStageChild[0];
	m_stageNexts = &CarCascade::StageNexts[0];
	m_stageParents = &CarCascade::StageParents[0];
	m_stageThresholds = &CarCascade::StageThresholds[0];

	m_classifierCounts = &CarCascade::ClassifierCounts[0];
	m_classifierAlphas = &CarCascade::ClassifierAlphas[0];
	m_classifierThresholds = &CarCascade::ClassifierThresholds[0];
	m_classifierLefts = &CarCascade::ClassifierLefts[0];
	m_classifierRights = &CarCascade::ClassifierRights[0];

	m_featureTilted = &CarCascade::FeatureTilted[0];
	m_featureRects = &CarCascade::FeatureRects[0];
	m_featureWeights = &CarCascade::FeatureWeights[0];
}

CvHaarClassifierCascade* CCascadeClassifier::GetClassifierCascade()
{
	Init();

	int cascadeSize = sizeof(CvHaarClassifierCascade) +
					  sizeof(CvHaarStageClassifier) * m_numOfStages;
	CvHaarClassifierCascade *cascade = (CvHaarClassifierCascade *)cvAlloc(cascadeSize);
	memset(cascade, 0, cascadeSize);
	cascade->stage_classifier = (CvHaarStageClassifier*)(cascade + 1);

	cascade->flags = m_flags;
	cascade->count = m_numOfStages;
	cascade->orig_window_size.width = m_windowWidth;
	cascade->orig_window_size.height = m_windowHeight;
	
	int classifierIndex = 0, featureIndex = 0, featureWeightIndex = 0;
	for (int stage = 0; stage < m_numOfStages; stage++)
	{
		CvHaarStageClassifier *stageClassifier = cascade->stage_classifier + stage;
		SetValuesForStage(&stageClassifier, stage, classifierIndex, featureIndex, featureWeightIndex);		
	}

	return cascade;
}

void CCascadeClassifier::SetValuesForStage(CvHaarStageClassifier **stageClassifierRef, const int stage, int &classifierIndex, int &featureIndex, int &featureWeightIndex)
{
	CvHaarStageClassifier *stageClassifier = *stageClassifierRef;
	int numOfStageNodes = m_numStageNodes[stage];

	stageClassifier->count = numOfStageNodes;
	stageClassifier->child = m_numStageChild[stage];
	stageClassifier->next = m_stageNexts[stage];
	stageClassifier->parent = m_stageParents[stage];
	stageClassifier->threshold = m_stageThresholds[stage];

	int classifierSize = sizeof(CvHaarClassifier) * numOfStageNodes;
	stageClassifier->classifier = (CvHaarClassifier *)cvAlloc(classifierSize);

	for (int node = 0; node < numOfStageNodes; node++)
	{
		CvHaarClassifier *classifier = stageClassifier->classifier + node;
		SetValuesForClassifier(&classifier, classifierIndex, featureIndex, featureWeightIndex);
		classifierIndex++;
	}
}

void CCascadeClassifier::SetValuesForClassifier(CvHaarClassifier **classifierRef, const int classifierIndex, int &featureIndex, int &featureWeightIndex)
{
	CvHaarClassifier *classifier = *classifierRef;

	int numOfClassifierFeatures = m_classifierCounts[classifierIndex];

	classifier->count = numOfClassifierFeatures;
	classifier->alpha = &m_classifierAlphas[2 * classifierIndex];
	classifier->left = &m_classifierLefts[classifierIndex];
	classifier->right = &m_classifierRights[classifierIndex];
	classifier->threshold = &m_classifierThresholds[classifierIndex];
			
	int featureSize = numOfClassifierFeatures * sizeof(CvHaarFeature);
	classifier->haar_feature = (CvHaarFeature *)cvAlloc(featureSize);

	for (int feature = 0; feature < numOfClassifierFeatures; feature++)
	{
		CvHaarFeature *haarFeature = classifier->haar_feature + feature;
		SetValuesForFeature(&haarFeature, featureIndex, featureWeightIndex);
		featureIndex++;
	}
}

void CCascadeClassifier::SetValuesForFeature(CvHaarFeature **haarFeatureRef, const int featureIndex, int &featureWeightIndex)
{
	CvHaarFeature *haarFeature = *haarFeatureRef;

	haarFeature->tilted = m_featureTilted[featureIndex];
	for (int featureRect = 0; featureRect < CV_HAAR_FEATURE_MAX; featureRect++)
	{
		haarFeature->rect[featureRect].weight = m_featureWeights[featureWeightIndex];
		haarFeature->rect[featureRect].r.x = m_featureRects[featureWeightIndex * 4];
		haarFeature->rect[featureRect].r.y = m_featureRects[featureWeightIndex * 4 + 1];
		haarFeature->rect[featureRect].r.width = m_featureRects[featureWeightIndex * 4 + 2];
		haarFeature->rect[featureRect].r.height = m_featureRects[featureWeightIndex * 4 + 3];

		featureWeightIndex++;
	}
}

int CCascadeClassifier::GetNumberOfFeaturesInStage(const int stageIndex)
{
	int numOfClassifiers = m_numStageNodes[stageIndex];
	int numOfFeatures = 0;

	for (int i = 0; i < numOfClassifiers; i++)
	{
		numOfFeatures += m_classifierCounts[i];
	}

	return numOfFeatures;
}