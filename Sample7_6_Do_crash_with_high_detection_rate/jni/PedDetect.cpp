#include <PedDetect.h>
// PhoneComPedestrianDetect.cpp
#include "PhoneComPedestrianDetect.h"
#include<android/log.h>
#define LOG_TAG "System.out"
#define LOGD(...);// __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
using namespace PhoneComPedestrianDetect;

void PrintTheModelForVector(vector<double> m_TempSplitTheta, int len)
{
	//ofstream fout(TXT_PATH);
	//fout << "******�������ķָ���*****" << endl;
	FILE *PtrToModelTXT = fopen("mnt/sdcard/PED_DETECT/ModelTXT.txt", "w");
	for (int i = 0; i < len; i++)
	{
		fprintf(PtrToModelTXT,"%g\n", m_TempSplitTheta[i]);
		LOGD("m_TempSplitTheta[%d]=%g",i, m_TempSplitTheta[i]);
		//fout << "g=" << pData[i + len] << "; ";
		//fout << "b=" << pData[i + len + len] << "; ";
		
	}
	/*fout << endl << endl << endl << "whole image is" << endl << endl << endl;
	for (int i = 0; i < 3 * len; i++)
	{
	fout << pData[i] << "; ";
	}
	*/
	//fout << "******�������ķָ���*****" << endl;
	fclose(PtrToModelTXT);
	return;

}

IntChnPedestrianDetect::IntChnPedestrianDetect()
{
	m_OriImage.data = NULL;
	m_ChannelsImage.data = NULL;

	m_SumImage.avg = NULL;
	m_SumImage.var = NULL;
	m_SumImage.data = NULL;
	m_Sum2Image.avg = NULL;
	m_Sum2Image.var = NULL;
	m_Sum2Image.data = NULL;

	pGradOrient = NULL;
	pGradMagSmooth = NULL;
	pPedestrianRect = NULL;
	pDetectRect = NULL;
	pMiddleRect = NULL;
	pMiddleNum = NULL;
}

void IntChnPedestrianDetect::ConvertImageFormat(int *pRGBData, int len, TxtChnImage &m_OriImage)
{
	float *pData = m_OriImage.data;
	//FILE * PtrToFileImage = NULL;
	//PtrToFileImage = fopen("mnt/sdcard/PED_DETECT/ImageOnAndroid.txt","w");
	//if (PtrToFileImage == NULL)
	//{
	//	LOGD("fopen txt error");
	//}
	//LOGD("fopen txt succeeded");
	for (int i = 0; i < len; i++)
	{
		int color = pRGBData[i];
		int a = color >> 24;
		int r = (color & 0x00ff0000) >> 16;
		int g = (color & 0x0000ff00) >> 8;
		int b = (color & 0x000000ff);

		pData[i] = r / 255.0f;
		//pData[i] = 0.5;
		//fprintf(PtrToFileImage, "r=%f; ", m_OriImage.data[i]);
		pData[i + len] = g / 255.0f;
		//fprintf(PtrToFileImage, "g=%f; ", m_OriImage.data[i + len]);
		//LOGD("pData[i+len]=%d", pData[i + len]);
		
		pData[i + len + len] = b / 255.0f;
		//fprintf(PtrToFileImage, "b=%f; ", m_OriImage.data[i + len + len]);
		//LOGD("pData[i+len+len]=%d", pData[i + len + len]);
	}
	//size_t count = 3 * len;
	//if(count!=fwrite())
	//if (fclose(PtrToFileImage) != 0)
	//{
	//	LOGD("fclose txt error");
	//}
	//LOGD("fclose txt succeeded");
}

int * IntChnPedestrianDetect::StartDetect(int * OpencvImage, PtrToParam MyParam)
{
	TxtChnImage m_OriImage;
	m_OriImage.height = MyParam->m_ImageHeight;
	m_OriImage.width = MyParam->m_ImageWidth;
	m_OriImage.nChannels = 3;
	m_OriImage.data = new float[m_OriImage.height* m_OriImage.width * 3];
	memset(m_OriImage.data, 0, sizeof(float)*m_OriImage.height* m_OriImage.width * 3);
	int len = m_OriImage.width*m_OriImage.height;
	LOGD("len=%d", len);
	ConvertImageFormat(OpencvImage,len, m_OriImage);
	LOGD("ConvertImageFormat Succeeded");
	//LOGD("m_OriImage,%f");
	int m_PedestrianNum = DetectImage(m_OriImage, pPedestrianRect);
	LOGD("m_PedestrianNum=%d", m_PedestrianNum);
	int * pResult = (int *)malloc((4 * m_PedestrianNum + 1) * sizeof(int));
	pResult[0] = m_PedestrianNum;
	for (int i = 1; i < m_PedestrianNum + 1; i++)
	{
		pResult[4 * i - 3] = pPedestrianRect[i - 1].iTl.iX;
		pResult[4 * i - 2] = pPedestrianRect[i - 1].iTl.iY;
		pResult[4 * i - 1] = pPedestrianRect[i - 1].iBr.iX;
		pResult[4 * i] = pPedestrianRect[i - 1].iBr.iY;
	}
	return pResult;
}

bool IntChnPedestrianDetect::InitDetect(int m_ImageWidth, int m_ImageHeight, bool m_FlagWhole)
{
	// ȫ��ģ��
	/*string m_CascadeModelPath1 = "Model_60_160_L1.dat";
	string m_CascadeModelPath2 = "Model_90_240_L1.dat";
	string m_CascadeModelPath3 = "Model_120_320_L1.dat";

	string m_SplitThetaPath1 = "MaxValue_60_160_244_L1.dat";
	string m_SplitThetaPath2 = "MaxValue_90_240_210_L1.dat";
	string m_SplitThetaPath3 = "MaxValue_120_320_189_L1.dat";
	*/
	string m_CascadeModelPath1 = "mnt/sdcard/PED_DETECT/Model_60_160_L1.dat";
	string m_CascadeModelPath2 = "mnt/sdcard/PED_DETECT/Model_90_240_L1.dat";
	string m_CascadeModelPath3 = "mnt/sdcard/PED_DETECT/Model_120_320_L1.dat";

	string m_SplitThetaPath1 = "mnt/sdcard/PED_DETECT/MaxValue_60_160_244_L1.dat";
	string m_SplitThetaPath2 = "mnt/sdcard/PED_DETECT/MaxValue_90_240_210_L1.dat";
	string m_SplitThetaPath3 = "mnt/sdcard/PED_DETECT/MaxValue_120_320_189_L1.dat";

	CascadeModelSize m_ModelSize1 = { 60,160 };
	CascadeModelSize m_ModelSize2 = { 90,240 };
	CascadeModelSize m_ModelSize3 = { 120,320 };

	double m_ThetaAdjust1 = 0.0;
	double m_ThetaAdjust2 = 0.0;
	double m_ThetaAdjust3 = 0.0;

	// ����ģ��
	string m_HalfCascadeModelPath1 = "Model_60_120_L1.dat";
	string m_HalfCascadeModelPath2 = "Model_90_180_L1.dat";
	string m_HalfCascadeModelPath3 = "Model_120_240_L1.dat";

	string m_HalfSplitThetaPath1 = "MaxValue_60_120_403_L1.dat";
	string m_HalfSplitThetaPath2 = "MaxValue_90_180_335_L1.dat";
	string m_HalfSplitThetaPath3 = "MaxValue_120_240_332_L1.dat";
	

	CascadeModelSize m_HalfModelSize1 = { 60,120 };
	CascadeModelSize m_HalfModelSize2 = { 90,180 };
	CascadeModelSize m_HalfModelSize3 = { 120,240 };

	double m_HalfThetaAdjust1 = 0.0;
	double m_HalfThetaAdjust2 = 0.0;
	double m_HalfThetaAdjust3 = 0.0;

	vector<string> m_MultiModelPath;
	vector<string> m_MutliSplitThetaPath;
	vector<CascadeModelSize> m_MultiModelSize;
	vector<double> m_MultiTheta;

	if (m_FlagWhole == true)
	{
		m_MultiModelPath.push_back(m_CascadeModelPath1);
		m_MultiModelPath.push_back(m_CascadeModelPath2);
		m_MultiModelPath.push_back(m_CascadeModelPath3);

		m_MutliSplitThetaPath.push_back(m_SplitThetaPath1);
		m_MutliSplitThetaPath.push_back(m_SplitThetaPath2);
		m_MutliSplitThetaPath.push_back(m_SplitThetaPath3);

		m_MultiModelSize.push_back(m_ModelSize1);
		m_MultiModelSize.push_back(m_ModelSize2);
		m_MultiModelSize.push_back(m_ModelSize3);

		m_MultiTheta.push_back(m_ThetaAdjust1);
		m_MultiTheta.push_back(m_ThetaAdjust2);
		m_MultiTheta.push_back(m_ThetaAdjust3);
	}
	else
	{
		m_MultiModelPath.push_back(m_HalfCascadeModelPath1);
		m_MultiModelPath.push_back(m_HalfCascadeModelPath2);
		m_MultiModelPath.push_back(m_HalfCascadeModelPath3);

		m_MutliSplitThetaPath.push_back(m_HalfSplitThetaPath1);
		m_MutliSplitThetaPath.push_back(m_HalfSplitThetaPath2);
		m_MutliSplitThetaPath.push_back(m_HalfSplitThetaPath3);

		m_MultiModelSize.push_back(m_HalfModelSize1);
		m_MultiModelSize.push_back(m_HalfModelSize2);
		m_MultiModelSize.push_back(m_HalfModelSize3);

		m_MultiTheta.push_back(m_HalfThetaAdjust1);
		m_MultiTheta.push_back(m_HalfThetaAdjust2);
		m_MultiTheta.push_back(m_HalfThetaAdjust3);

	}
	LOGD("%s", "checkpoint 3.1");
	LOGD("%d,%d", m_ImageWidth, m_ImageHeight);
	if (InitDetectParam(m_MultiModelPath, m_MutliSplitThetaPath, m_MultiModelSize, m_ImageWidth, m_ImageHeight, 3, m_MultiTheta, 1.2) == false)
	{
		LOGD("%s", "checkpoint 3.2");

		return false;
		
	}
	else 
	{
		LOGD("%s", "checkpoint 3.3");
		return true;
	}
}
void IntChnPedestrianDetect::ReleaseDetect()
{
	if (m_RealMultiScaleClassfier.size() > 0)
		m_RealMultiScaleClassfier.clear();

	if (m_CascadeClassfier.size() > 0)
		m_CascadeClassfier.clear();

	if (m_MultiRealSplitTheta.size() > 0)
		m_MultiRealSplitTheta.clear();

	if (m_RealSplitTheta.size() > 0)
		m_RealSplitTheta.clear();

	if (pGradOrient != NULL)
		free(pGradOrient);
	if (pGradMagSmooth != NULL)
		free(pGradMagSmooth);

	if (m_SumImage.var != NULL)
		free(m_SumImage.var);
	if (m_SumImage.avg != NULL)
		free(m_SumImage.avg);
	if (m_SumImage.data != NULL)
		free(m_SumImage.data);

	if (m_Sum2Image.var != NULL)
		free(m_Sum2Image.var);
	if (m_Sum2Image.avg != NULL)
		free(m_Sum2Image.avg);
	if (m_Sum2Image.data != NULL)
		free(m_Sum2Image.data);

	if (m_ChannelsImage.data != NULL)
		free(m_ChannelsImage.data);
	if (m_OriImage.data != NULL)
		free(m_OriImage.data);

	if (pPedestrianRect != NULL)
		delete[]pPedestrianRect;
	if (pDetectRect != NULL)
		delete[]pDetectRect;
	if (pMiddleRect != NULL)
		delete[]pMiddleRect;
	if (pMiddleNum != NULL)
		delete[]pMiddleNum;

}

/*void IntChnPedestrianDetect::ConvertImageFormat(int *pRGBData, int len, TxtChnImage &m_OriImage)
{
	float *pData = m_OriImage.data;
	for (int i = 0; i < len; i++)
	{
		int color = pRGBData[i];
		int a = color >> 24;
		int r = (color & 0x00ff0000) >> 16;
		int g = (color & 0x0000ff00) >> 8;
		int b = (color & 0x000000ff);

		pData[i] = r / 255.0f;
		pData[i + len] = g / 255.0f;
		pData[i + len + len] = b / 255.0f;
	}
}
*/

bool IntChnPedestrianDetect::InitDetectParam(vector<string> MultiModelPath, vector<string> MultiSplitThetaPath, vector<CascadeModelSize> MultiModelSize, int m_ImageWidth, int m_ImageHeight, int MinNeighbors, vector<double> DetaTheta, double DetectScale)
{
	if (MultiModelPath.size() != MultiModelSize.size())
		return false;
	m_MinNeighbors = MinNeighbors;
	m_DetaTheta = DetaTheta;
	m_DetectScale = DetectScale;

	vector<CascadeModelSize> m_OriModelsSize;
	m_OriModelsSize = MultiModelSize;
	if (m_OriModelsSize[0].width >= m_ImageWidth || m_OriModelsSize[0].height >= m_ImageHeight)
		return false;

	vector<vector<StrongClassfier> > m_OriMultiCascadeClassfiers;
	LOGD("%s", "checkpoint 3.1.1");
	if (LoadCasacadeModel(m_OriMultiCascadeClassfiers, MultiModelPath) == false)
	{
		return false;
	}
	LOGD("%s", "checkpoint 3.1.2");
	vector<vector<double> > m_OriMultiSplitTheta;
	if (LoadMultiSplitTheta(m_OriMultiSplitTheta, MultiSplitThetaPath) == false)
		return false;

	for (int i = m_OriModelsSize.size() - 1; i >= 0; i--)
	{
		if (m_OriModelsSize[i].width >= m_ImageWidth || m_OriModelsSize[i].height >= m_ImageHeight)
		{
			m_OriModelsSize.erase(m_OriModelsSize.begin() + i);
			m_OriMultiCascadeClassfiers.erase(m_OriMultiCascadeClassfiers.begin() + i);
			m_DetaTheta.erase(m_DetaTheta.begin() + i);
		}
	}

	InitMemoryMalloc(m_ImageWidth, m_ImageHeight);

	int m_DetectModelWidth = m_OriModelsSize[0].width;
	int m_DetectModelHeight = m_OriModelsSize[0].height;

	double m_ResizeScale = 1.0;
	m_DetectResizeNum = 0;
	int m_MultiModelNum = m_OriModelsSize.size();
	for (int i = 0; i < m_MultiModelNum - 1; i++)
	{
		for (int m_DetectIdx = 0; m_DetectModelWidth < m_OriModelsSize[i + 1].width && m_DetectModelHeight < m_OriModelsSize[i + 1].height; m_DetectIdx++)
		{
			m_DetectResizeNum++;

			vector<StrongClassfier> m_TempCascadeClassfier, m_TempCascadeClassfier1;
			m_TempCascadeClassfier = m_OriMultiCascadeClassfiers[i];

			for (vector<StrongClassfier>::size_type n = 0; n < m_TempCascadeClassfier.size(); n++)
			{
				StrongClassfier m_TempStrongClassfier;
				vector<WeakClassfier> m_TempWeakClassfierSet;
				for (vector<WeakClassfier>::size_type n1 = 0; n1 < m_TempCascadeClassfier[n].weakclassfierset.size(); n1++)
				{
					WeakClassfier m_TempWeakClassfier = m_TempCascadeClassfier[n].weakclassfierset[n1];
					m_TempWeakClassfier.f.x = m_ResizeScale*m_TempWeakClassfier.f.x;
					m_TempWeakClassfier.f.y = m_ResizeScale*m_TempWeakClassfier.f.y;
					m_TempWeakClassfier.f.dx = m_ResizeScale*m_TempWeakClassfier.f.dx;
					m_TempWeakClassfier.f.dy = m_ResizeScale*m_TempWeakClassfier.f.dy;

					if (m_TempWeakClassfier.f.type > 0 && m_TempWeakClassfier.f.type < 4)
					{
						m_TempWeakClassfier.realthres = m_ResizeScale*m_ResizeScale*m_TempWeakClassfier.realthres;
					}
					else
					{
						m_TempWeakClassfier.realthres = m_TempWeakClassfier.realthres;
					}

					m_TempWeakClassfierSet.push_back(m_TempWeakClassfier);
				}
				m_TempStrongClassfier.rawth = m_TempCascadeClassfier[n].rawth + m_DetaTheta[i] * abs(m_TempCascadeClassfier[n].rawth);

				m_TempStrongClassfier.weakclassfierset = m_TempWeakClassfierSet;
				m_TempCascadeClassfier1.push_back(m_TempStrongClassfier);

			}
			MultiScaleClassfier m_TempMultiScaleClassfier;
			m_TempMultiScaleClassfier.width = m_DetectModelWidth;
			m_TempMultiScaleClassfier.height = m_DetectModelHeight;
			m_TempMultiScaleClassfier.CascadeClassfier = m_TempCascadeClassfier1;
			m_RealMultiScaleClassfier.push_back(m_TempMultiScaleClassfier);

			m_MultiRealSplitTheta.push_back(m_OriMultiSplitTheta[i]);

			m_ResizeScale *= m_DetectScale;
			m_DetectModelWidth = m_ResizeScale*m_OriModelsSize[i].width + 0.5;
			m_DetectModelHeight = m_ResizeScale*m_OriModelsSize[i].height + 0.5;
		}

		m_ResizeScale = 1.0;
		m_DetectModelWidth = m_OriModelsSize[i + 1].width;
		m_DetectModelHeight = m_OriModelsSize[i + 1].height;
	}

	for (int m_DetectIdx = 0; m_DetectModelWidth < m_ImageWidth && m_DetectModelHeight < m_ImageHeight; m_DetectIdx++)
	{
		m_DetectResizeNum++;

		vector<StrongClassfier> m_TempCascadeClassfier, m_TempCascadeClassfier1;
		m_TempCascadeClassfier = m_OriMultiCascadeClassfiers[m_MultiModelNum - 1];

		for (vector<StrongClassfier>::size_type n = 0; n < m_TempCascadeClassfier.size(); n++)
		{
			StrongClassfier m_TempStrongClassfier;
			vector<WeakClassfier> m_TempWeakClassfierSet;
			for (vector<WeakClassfier>::size_type n1 = 0; n1 < m_TempCascadeClassfier[n].weakclassfierset.size(); n1++)
			{
				WeakClassfier m_TempWeakClassfier = m_TempCascadeClassfier[n].weakclassfierset[n1];
				m_TempWeakClassfier.f.x = m_ResizeScale*m_TempWeakClassfier.f.x;
				m_TempWeakClassfier.f.y = m_ResizeScale*m_TempWeakClassfier.f.y;
				m_TempWeakClassfier.f.dx = m_ResizeScale*m_TempWeakClassfier.f.dx;
				m_TempWeakClassfier.f.dy = m_ResizeScale*m_TempWeakClassfier.f.dy;

				if (m_TempWeakClassfier.f.type > 0 && m_TempWeakClassfier.f.type < 4)
				{
					m_TempWeakClassfier.realthres = m_ResizeScale*m_ResizeScale*m_TempWeakClassfier.realthres;
				}
				else
				{
					m_TempWeakClassfier.realthres = m_TempWeakClassfier.realthres;
				}

				m_TempWeakClassfierSet.push_back(m_TempWeakClassfier);
			}

			m_TempStrongClassfier.rawth = m_TempCascadeClassfier[n].rawth + m_DetaTheta[m_MultiModelNum - 1] * abs(m_TempCascadeClassfier[n].rawth);

			m_TempStrongClassfier.weakclassfierset = m_TempWeakClassfierSet;
			m_TempCascadeClassfier1.push_back(m_TempStrongClassfier);
		}
		MultiScaleClassfier m_TempMultiScaleClassfier;
		m_TempMultiScaleClassfier.width = m_DetectModelWidth;
		m_TempMultiScaleClassfier.height = m_DetectModelHeight;
		m_TempMultiScaleClassfier.CascadeClassfier = m_TempCascadeClassfier1;
		m_RealMultiScaleClassfier.push_back(m_TempMultiScaleClassfier);

		m_MultiRealSplitTheta.push_back(m_OriMultiSplitTheta[m_MultiModelNum - 1]);

		m_ResizeScale *= m_DetectScale;
		m_DetectModelWidth = m_ResizeScale*m_OriModelsSize[m_MultiModelNum - 1].width + 0.5;
		m_DetectModelHeight = m_ResizeScale*m_OriModelsSize[m_MultiModelNum - 1].height + 0.5;
	}
	return true;
}
void IntChnPedestrianDetect::InitMemoryMalloc(int m_ImageWidth, int m_ImageHeight)
{
	pPedestrianRect = new TRect[1000];
	memset(pPedestrianRect, 0, sizeof(TRect) * 1000);
	pDetectRect = new TRect[10000];
	memset(pDetectRect, 0, sizeof(TRect) * 10000);
	pMiddleRect = new TRect[1000];
	memset(pMiddleRect, 0, sizeof(TRect) * 1000);
	pMiddleNum = new int[1000];
	memset(pMiddleNum, 0, sizeof(int) * 1000);

	m_ChannelsImage.width = m_ImageWidth;
	m_ChannelsImage.height = m_ImageHeight;
	m_ChannelsImage.nChannels = 10;
	m_ChannelsImage.data = (float *)malloc(sizeof(float)*m_ChannelsImage.width*m_ChannelsImage.height*m_ChannelsImage.nChannels);
	memset(m_ChannelsImage.data, 0, sizeof(float)*m_ChannelsImage.width*m_ChannelsImage.height*m_ChannelsImage.nChannels);

	m_OriImage.width = m_ImageWidth;
	m_OriImage.height = m_ImageHeight;
	m_OriImage.nChannels = 3;
	m_OriImage.data = (float *)malloc(sizeof(float)*m_OriImage.width*m_OriImage.height*m_OriImage.nChannels);
	memset(m_OriImage.data, 0, sizeof(float)*m_OriImage.width*m_OriImage.height*m_OriImage.nChannels);

	m_SumImage.width = m_ImageWidth;
	m_SumImage.height = m_ImageHeight;
	m_SumImage.nChannels = 10;
	m_SumImage.avg = (float*)malloc(sizeof(float)*m_SumImage.nChannels);
	memset(m_SumImage.avg, 0, sizeof(float)*m_SumImage.nChannels);
	m_SumImage.var = (float *)malloc(sizeof(float)*m_SumImage.nChannels);
	memset(m_SumImage.var, 0, sizeof(float)*m_SumImage.nChannels);
	m_SumImage.data = (float *)malloc(sizeof(float)*m_SumImage.width*m_SumImage.height*m_SumImage.nChannels);
	memset(m_SumImage.data, 0, sizeof(float)*m_SumImage.width*m_SumImage.height*m_SumImage.nChannels);

	m_Sum2Image.width = m_ImageWidth;
	m_Sum2Image.height = m_ImageHeight;
	m_Sum2Image.nChannels = 10;
	m_Sum2Image.avg = (float *)malloc(sizeof(float)*m_Sum2Image.nChannels);
	memset(m_Sum2Image.avg, 0, sizeof(float)*m_Sum2Image.nChannels);
	m_Sum2Image.var = (float *)malloc(sizeof(float)*m_Sum2Image.nChannels);
	memset(m_Sum2Image.var, 0, sizeof(float)*m_Sum2Image.nChannels);
	m_Sum2Image.data = (float*)malloc(sizeof(float)*m_Sum2Image.width*m_Sum2Image.height*m_Sum2Image.nChannels);
	memset(m_Sum2Image.data, 0, sizeof(float)*m_Sum2Image.width*m_Sum2Image.height*m_Sum2Image.nChannels);

	pGradOrient = (float *)malloc(sizeof(float)*m_ImageWidth*m_ImageHeight);
	pGradMagSmooth = (float *)malloc(sizeof(float)*m_ImageWidth*m_ImageHeight);
}
int IntChnPedestrianDetect::DetectImage(TxtChnImage &m_Image, TRect *pRect)
{
	int m_ImageHeight = m_Image.height;
	int m_ImageWidth = m_Image.width;
	LOGD("m_ImageHeight_In_DetectImage=%d", m_ImageHeight);
	LOGD("m_ImageWidth_In_DetectImage=%d", m_ImageWidth);
	float m_TempVar;

	CalChannelsFeature(m_Image, m_ChannelsImage);
	//CalChannelsFeaIntegral(m_ChannelsImage,m_SumImage);
	//int m_Len = m_ChannelsImage.width*m_ChannelsImage.height*m_ChannelsImage.nChannels;
	//for (int i = 0; i < m_Len; i++)
	//{
	//	m_ChannelsImage.data[i] = m_ChannelsImage.data[i]*m_ChannelsImage.data[i];
	//}
	//CalChannelsFeaIntegral(m_ChannelsImage,m_Sum2Image);
	LOGD("CalChannelsFeature Succeeded");
	float m_TempValue;
	int m_Len = m_ChannelsImage.width*m_ChannelsImage.height*m_ChannelsImage.nChannels;
	for (int i = 0; i < m_Len; i++)
	{
		m_TempValue = m_ChannelsImage.data[i];
		m_SumImage.data[i] = m_TempValue;
		m_Sum2Image.data[i] = m_TempValue*m_TempValue;
	}

	CalChannelsFeaIntegral(m_SumImage, m_Sum2Image);
	LOGD("CalChannelsFeaIntegral Succeeded");
	int m_Offset, m_OffsetY, m_OffsetDY;
	int m_PointX, m_PointY;
	int m_DetectStep;
	m_Len = m_ImageWidth*m_ImageHeight;
	int z = 0, n = m_SumImage.nChannels;
	float *pData = NULL, *pData1 = NULL;

	TRect *pTempRect = pDetectRect;
	int m_DetectNum = 0;
	for (int m_DetectIdx = 0; m_DetectIdx < m_DetectResizeNum; m_DetectIdx++)
	{
		int m_DetectSizeX = m_RealMultiScaleClassfier[m_DetectIdx].width;
		int m_DetectSizeY = m_RealMultiScaleClassfier[m_DetectIdx].height;
		m_CascadeClassfier = m_RealMultiScaleClassfier[m_DetectIdx].CascadeClassfier;
		m_RealSplitTheta = m_MultiRealSplitTheta[m_DetectIdx];
		m_OffsetDY = m_DetectSizeY*m_ImageWidth;
		float m_DetectArea = m_DetectSizeX*m_DetectSizeY;
		m_DetectStep = 4;
		for (m_PointY = 0; (m_PointY + m_DetectSizeY) < m_ImageHeight; m_PointY += m_DetectStep)
		{
			m_OffsetY = m_PointY*m_ImageWidth;
			for (m_PointX = 0; (m_PointX + m_DetectSizeX) < m_ImageWidth; m_PointX += m_DetectStep)
			{
				z = 0;
				pData = m_SumImage.data + z*m_Len;
				m_Offset = m_OffsetY + m_PointX;
				m_SumImage.avg[z] = pData[m_Offset];

				m_Offset += m_DetectSizeX;
				m_SumImage.avg[z] -= pData[m_Offset];

				m_Offset = m_OffsetY + m_OffsetDY + m_PointX;
				m_SumImage.avg[z] -= pData[m_Offset];

				m_Offset += m_DetectSizeX;
				m_SumImage.avg[z] += pData[m_Offset];
				for (z = 1; z < 4; z++)
				{
					pData = m_SumImage.data + z*m_Len;
					pData1 = m_Sum2Image.data + z*m_Len;
					m_Offset = m_OffsetY + m_PointX;
					m_SumImage.avg[z] = pData[m_Offset];
					m_TempVar = pData1[m_Offset];

					m_Offset += m_DetectSizeX;
					m_SumImage.avg[z] -= pData[m_Offset];
					m_TempVar -= pData1[m_Offset];

					m_Offset = m_OffsetY + m_OffsetDY + m_PointX;
					m_SumImage.avg[z] -= pData[m_Offset];
					m_TempVar -= pData1[m_Offset];

					m_Offset += m_DetectSizeX;
					m_SumImage.avg[z] += pData[m_Offset];
					m_TempVar += pData1[m_Offset];

					m_SumImage.avg[z] /= m_DetectArea;
					m_TempVar /= m_DetectArea;
					m_SumImage.var[z] = m_TempVar - m_SumImage.avg[z] * m_SumImage.avg[z];
					if (m_SumImage.var[z] <= 0.0f)
					{
						m_SumImage.var[z] = 0.0f;
					}
					else
					{
						m_SumImage.var[z] = sqrt(m_SumImage.var[z]);
					}
				}
				//m_SumImage.avg[0] = m_DetectSizeX*m_DetectSizeY*m_SumImage.avg[0];				
				for (z = 4; z < n; z++)
				{
					pData = m_SumImage.data + z*m_Len;
					m_Offset = m_OffsetY + m_PointX;
					m_SumImage.avg[z] = pData[m_Offset];

					m_Offset += m_DetectSizeX;
					m_SumImage.avg[z] -= pData[m_Offset];

					m_Offset = m_OffsetY + m_OffsetDY + m_PointX;
					m_SumImage.avg[z] -= pData[m_Offset];

					m_Offset += m_DetectSizeX;
					m_SumImage.avg[z] += pData[m_Offset];
				}

				//BOOL IsObjectFlag = FALSE;
				bool IsObjectFlag = IsExistObjectInScaleCascade(m_SumImage, m_PointX + 1, m_PointY + 1);
				if (IsObjectFlag == true)
				{
					pTempRect->iTl.iX = m_PointX;
					pTempRect->iTl.iY = m_PointY;
					pTempRect->iBr.iX = m_PointX + m_DetectSizeX;
					pTempRect->iBr.iY = m_PointY + m_DetectSizeY;
					pTempRect++;
					m_DetectNum++;
				}
			}
		}
	}
	LOGD("m_DetectNum=%d", m_DetectNum);
	return RectPartition(pRect, pDetectRect, m_DetectNum, m_MinNeighbors);
	//return m_DetectNum;
}
void IntChnPedestrianDetect::CalChannelsFeature(TxtChnImage &m_OriImage, TxtChnImage &m_ChannelsImage)
{
	int m_ImageWidth = m_OriImage.width;
	int m_ImageHeight = m_OriImage.height;
	int m_ImageChannels = m_OriImage.nChannels;

	float *pNormImage = m_OriImage.data;
	LOGD(" Before memset");
	memset(m_ChannelsImage.data, 0, sizeof(float)*m_ChannelsImage.width*m_ChannelsImage.height*m_ChannelsImage.nChannels);
	memset(pGradOrient, 0, sizeof(float)*m_ChannelsImage.width*m_ChannelsImage.height);
	memset(pGradMagSmooth, 0, sizeof(float)*m_ChannelsImage.width*m_ChannelsImage.height);
	LOGD("memset succeeded");
	float *pGradMag = m_ChannelsImage.data;
	float *pLUV = m_ChannelsImage.data + m_ChannelsImage.width*m_ChannelsImage.height;
	float *pGradHist = m_ChannelsImage.data + m_ChannelsImage.width*m_ChannelsImage.height * 4;
	LOGD("m_ImageWidth=%d, m_ImageHeight=%d, m_ImageChannels=%d", m_ImageWidth, m_ImageHeight, m_ImageChannels);
	gradMag(pNormImage, pGradMag, pGradOrient, m_ImageWidth, m_ImageHeight, m_ImageChannels);
	LOGD("gradMag succeeded");
	convBox(pGradMag, pGradMagSmooth, m_ImageWidth, m_ImageHeight, 1, 5);
	LOGD("convBox succeeded");
	gradMagNorm(pGradMag, pGradMagSmooth, m_ImageWidth, m_ImageHeight, 0.005);
	LOGD("gradMagNorm succeeded");
	rgb2luv(pNormImage, pLUV, m_ImageWidth*m_ImageHeight, 1.0f);
	LOGD("rgb2luv succeeded");
	gradHist(pGradMag, pGradOrient, pGradHist, m_ChannelsImage.width, m_ChannelsImage.height, 1, 6, false);
	LOGD("gradHist succeeded");
}
void IntChnPedestrianDetect::CalChannelsFeaIntegral(const TxtChnImage &m_ChannelsFea, TxtIntegralImage &m_IntegralImage)
{
	int w = m_ChannelsFea.width, h = m_ChannelsFea.height, n = m_ChannelsFea.nChannels;
	int len = w*h;

	float *pData = NULL;
	float *pIntegralData = NULL;
	for (int z = 0; z < n; z++)
	{
		pData = m_ChannelsFea.data + z*len;
		pIntegralData = m_IntegralImage.data + z*len;
		int i, j, offset1;
		for (i = 0; i < w; i++)
		{
			offset1 = i;
			pIntegralData[offset1] = pData[offset1];
			for (j = 1; j < h; j++)
			{
				offset1 += w;
				pIntegralData[offset1] = pIntegralData[offset1 - w] + pData[offset1];
			}
		}

		for (j = 0; j < h; j++)
		{
			offset1 = j*w;
			for (i = 1; i < w; i++)
			{
				offset1++;
				pIntegralData[offset1] += pIntegralData[offset1 - 1];
			}
		}
	}
}
void IntChnPedestrianDetect::CalChannelsFeaIntegral(TxtIntegralImage &m_IntegralImage, TxtIntegralImage &m_IntegralImage2)
{
	int w = m_IntegralImage.width, h = m_IntegralImage.height, n = m_IntegralImage.nChannels;
	int len = w*h;

	float *pData = NULL;
	float *pData2 = NULL;

	pData = m_IntegralImage.data;
	int i, j, offset1;
	for (i = 0; i < w; i++)
	{
		offset1 = i;
		for (j = 1; j < h; j++)
		{
			offset1 += w;

			*(pData + offset1) += *(pData + offset1 - w);
		}
	}

	for (j = 0; j < h; j++)
	{
		offset1 = j*w;
		for (i = 1; i < w; i++)
		{
			offset1++;

			*(pData + offset1) += *(pData + offset1 - 1);
		}
	}
	for (int z = 1; z < 4; z++)
	{
		pData = m_IntegralImage.data + z*len;
		pData2 = m_IntegralImage2.data + z*len;
		int i, j, offset1;
		for (i = 0; i < w; i++)
		{
			offset1 = i;
			for (j = 1; j < h; j++)
			{
				offset1 += w;

				*(pData + offset1) += *(pData + offset1 - w);
				*(pData2 + offset1) += *(pData2 + offset1 - w);
			}
		}

		for (j = 0; j < h; j++)
		{
			offset1 = j*w;
			for (i = 1; i < w; i++)
			{
				offset1++;

				*(pData + offset1) += *(pData + offset1 - 1);
				*(pData2 + offset1) += *(pData2 + offset1 - 1);
			}
		}
	}
	for (int z = 4; z < n; z++)
	{
		pData = m_IntegralImage.data + z*len;
		int i, j, offset1;
		for (i = 0; i < w; i++)
		{
			offset1 = i;
			for (j = 1; j < h; j++)
			{
				offset1 += w;

				*(pData + offset1) += *(pData + offset1 - w);
			}
		}

		for (j = 0; j < h; j++)
		{
			offset1 = j*w;
			for (i = 1; i < w; i++)
			{
				offset1++;

				*(pData + offset1) += *(pData + offset1 - 1);
			}
		}
	}
}
bool IntChnPedestrianDetect::IsExistObjectInScaleCascade(TxtIntegralImage &m_Image, int m_PointX, int m_PointY)
{

	double h = 0;
	for (int i = 0; i < 2; i++)
	{
		h += WeakClassValue(m_CascadeClassfier[0].weakclassfierset[i], m_Image, m_PointX, m_PointY);
	}

	int m_WeakCount = m_CascadeClassfier[0].weakclassfierset.size();
	for (int i = 2; i < m_WeakCount; i++)
	{
		if (h < m_RealSplitTheta[i - 2])
		{
			return false;
		}
		h += WeakClassValue(m_CascadeClassfier[0].weakclassfierset[i], m_Image, m_PointX, m_PointY);
	}
	if (h < m_CascadeClassfier[0].rawth)
	{
		return false;
	}
	return true;
}
double IntChnPedestrianDetect::StrongClassValue(StrongClassfier &sc, TxtIntegralImage &m_Image, int m_PointX, int m_PointY)
{
	double h = 0;
	int m_WeakCount = sc.weakclassfierset.size();
	for (int i = 0; i < m_WeakCount; i++)
		h += WeakClassValue(sc.weakclassfierset[i], m_Image, m_PointX, m_PointY);
	return h;
}
double IntChnPedestrianDetect::WeakClassValue(WeakClassfier &wc, TxtIntegralImage &m_Image, int m_PointX, int m_PointY)
{
	double val = FeatureValue(wc.f, m_Image, m_PointX, m_PointY);

	if (wc.f.type > 0 && wc.f.type < 4)
	{
		if (val > wc.realthres*m_Image.var[wc.f.type] + wc.f.dx*wc.f.dy*m_Image.avg[wc.f.type])
			return wc.alphaR;
		else
			return wc.alphaL;
	}
	else
	{
		if (val > wc.realthres*m_Image.avg[wc.f.type])
			return wc.alphaR;
		else
			return wc.alphaL;
	}
}
double IntChnPedestrianDetect::FeatureValue(ChnFeature &f, TxtIntegralImage &m_Image, int m_PointX, int m_PointY)
{
	int x = f.x + m_PointX;
	int y = f.y + m_PointY;
	int dx = f.dx;
	int dy = f.dy;
	int yw = y*m_Image.width;
	int dyw = dy*m_Image.width;
	float *pd = m_Image.data + f.type*m_Image.width*m_Image.height;
	float val;

	int m_Offset;
	m_Offset = yw + x;
	val = pd[m_Offset];
	m_Offset += dx;
	val -= pd[m_Offset];
	m_Offset = yw + dyw + x;
	val -= pd[m_Offset];
	m_Offset += dx;
	val += pd[m_Offset];

	return (double)val;
}
bool IntChnPedestrianDetect::LoadMultiSplitTheta(vector<vector<double> > &m_MultiSplitTheta, vector<string> m_SplitThetaPath)
{
	for (int i = 0; i < m_SplitThetaPath.size(); i++)
	{
		int len;
		string m_ModelPath = m_SplitThetaPath[i];
		FILE *pFileInfo = fopen(m_ModelPath.c_str(), "rb");

		//FILE *PtrToModelTXT = fopen("mnt/sdcard/PED_DETECT/ModelTXT.txt", "w");
		
		//if (fopen(&pFileInfo, m_ModelPath.c_str(), "rb") != 0)
		//if (fopen(m_ModelPath.c_str(), "rb") != 0)
		if (pFileInfo == NULL)
		{
			LOGD("fopen failed");
			return false;
		}
		if (fread(&len, sizeof(int), 1, pFileInfo) != 1)
		{
			LOGD("fread failed");
			return false;
		}
		LOGD("20160929AfternoonCheckingPointForLen = %d", len);
		vector<double> m_TempSplitTheta;
		for (int j = 0; j < len; j++)
		{
			double theta;
			if (fread(&theta, sizeof(double), 1, pFileInfo) != 1)
				return false;
			m_TempSplitTheta.push_back(theta);
		}
		fclose(pFileInfo);
		//fclose(PtrToModelTXT);
		PrintTheModelForVector(m_TempSplitTheta,len);
		m_MultiSplitTheta.push_back(m_TempSplitTheta);
	}
	return true;
}
bool IntChnPedestrianDetect::LoadCasacadeModel(vector<vector<StrongClassfier> > &m_MultiModelClassfier, vector<string> m_CasacadeMultiModelFile)
{

	for (int i = 0; i < m_CasacadeMultiModelFile.size(); i++)
	{
		int len;
		string m_ModelPath = m_CasacadeMultiModelFile[i];
		FILE *pFileInfo = fopen(m_ModelPath.c_str(), "rb");
		//if (fopen(&pFileInfo, m_ModelPath.c_str(), "rb") != 0)
		LOGD("%s", "checkpoint 3.1.1.1");
		LOGD("%s", m_ModelPath.c_str());
		if (pFileInfo == NULL)
			return false;
		LOGD("%s", "checkpoint 3.1.1.2");
		if (fread(&len, sizeof(int), 1, pFileInfo) != 1)
		{
			LOGD("%s", "checkpoint 3.1.1.3");
			return false;
		}
		LOGD("%s", "checkpoint 3.1.1.4");
		LOGD("%d", len);
		vector<StrongClassfier> m_TempCascadeClassfier;
		for (int j = 0; j < len; j++)
		{
			StrongClassfier sc;
			if (fread(&(sc.rawth), sizeof(double), 1, pFileInfo) != 1)
				return false;
			LoadWeakClassfier(sc.weakclassfierset, pFileInfo);
			m_TempCascadeClassfier.push_back(sc);
		}
		fclose(pFileInfo);
		m_MultiModelClassfier.push_back(m_TempCascadeClassfier);
	}
	return true;
}
bool IntChnPedestrianDetect::LoadCasacadeModel(string m_CasacadeModelFile)
{
	int i, len;
	FILE *fp = fopen(m_CasacadeModelFile.c_str(), "rb");
	//if (fopen_s(&fp, m_CasacadeModelFile.c_str(), "rb") != 0)
	if (fp == NULL)
		return false;
	if (fread(&len, sizeof(int), 1, fp) != 1)
		return false;
	for (i = 0; i<len; i++)
	{
		StrongClassfier sc;
		if (fread(&(sc.rawth), sizeof(double), 1, fp) != 1)
			return false;
		LoadWeakClassfier(sc.weakclassfierset, fp);
		m_CascadeClassfier.push_back(sc);
	}
	fclose(fp);
	return true;
}
void IntChnPedestrianDetect::LoadWeakClassfier(std::vector<WeakClassfier> &wcset, FILE* fp)
{
	wcset.clear();
	int i, len;
	fread(&len, sizeof(int), 1, fp);
	for (i = 0; i<len; i++)
	{
		WeakClassfier wc;
		fread(&(wc), sizeof(WeakClassfier), 1, fp);
		wcset.push_back(wc);
	}
}
bool IntChnPedestrianDetect::RectIsEqual(TRect &m_Rect1, TRect &m_Rect2)
{
	int xmax = max(m_Rect1.iTl.iX, m_Rect2.iTl.iX);
	int xmin = min(m_Rect1.iBr.iX, m_Rect2.iBr.iX);
	int ymax = max(m_Rect1.iTl.iY, m_Rect2.iTl.iY);
	int ymin = min(m_Rect1.iBr.iY, m_Rect2.iBr.iY);

	if (xmax >= xmin || ymax >= ymin)
		return false;
	float overlap = (float)(xmin - xmax + 1)*(ymin - ymax + 1);
	float area = (m_Rect1.iBr.iX - m_Rect1.iTl.iX + 1)*(m_Rect1.iBr.iY - m_Rect1.iTl.iY + 1) + (m_Rect2.iBr.iX - m_Rect2.iTl.iX + 1)*(m_Rect2.iBr.iY - m_Rect2.iTl.iY + 1) - overlap;
	if (overlap / area < 0.5f)
		return false;
	return true;
}
int IntChnPedestrianDetect::RectPartition(TRect *pDstRect, TRect *pOriRect, int m_DetectNum, int m_MinNeighbors)
{
	int m_PedestrianNum = 0;
	if (m_DetectNum <= 1)
		return m_PedestrianNum;
	bool *pFlagState = new bool[m_DetectNum];
	for (int i = 0; i < m_DetectNum; i++)
		pFlagState[i] = false;
	int *pNumOverlap = new int[m_DetectNum];

	TRect *pTemRect = pMiddleRect;
	int *pNum = pMiddleNum;

	int m_TempNum = 0;
	while (1)
	{
		memset(pNumOverlap, 0, sizeof(int)*m_DetectNum);
		for (int i = 0; i < m_DetectNum; i++)
		{
			if (pFlagState[i] == true)
				continue;
			for (int j = 0; j < m_DetectNum; j++)
			{
				if (pFlagState[j] == true || j == i)
					continue;
				if (RectIsEqual(pOriRect[i], pOriRect[j]) == false)
					continue;
				pNumOverlap[i]++;
			}
		}
		int m_MaxIdx = 0, m_MaxValue = 0;
		for (int i = 0; i < m_DetectNum; i++)
		{
			if (pNumOverlap[i] > m_MaxValue)
			{
				m_MaxIdx = i;
				m_MaxValue = pNumOverlap[i];
			}
		}
		if (m_MaxValue < m_MinNeighbors)
			break;
		TRect m_TempRect;
		m_TempRect.iTl.iX = 0; m_TempRect.iTl.iY = 0; m_TempRect.iBr.iX = 0; m_TempRect.iBr.iY = 0;
		int m_Num = 0;
		for (int i = 0; i < m_DetectNum; i++)
		{
			if (pFlagState[i] == true)
				continue;
			if (RectIsEqual(pOriRect[m_MaxIdx], pOriRect[i]) == true)
			{
				m_TempRect.iTl.iX += pOriRect[i].iTl.iX;
				m_TempRect.iTl.iY += pOriRect[i].iTl.iY;
				m_TempRect.iBr.iX += pOriRect[i].iBr.iX;
				m_TempRect.iBr.iY += pOriRect[i].iBr.iY;

				m_Num++;
				pFlagState[i] = true;
			}
		}
		m_TempRect.iTl.iX /= m_Num;
		m_TempRect.iTl.iY /= m_Num;
		m_TempRect.iBr.iX /= m_Num;
		m_TempRect.iBr.iY /= m_Num;
		pTemRect[m_TempNum] = m_TempRect;
		pNum[m_TempNum] = m_Num;
		m_TempNum++;
	}
	for (int i = 0; i < m_TempNum; i++)
	{
		TRect r1 = pTemRect[i];
		int num1 = pNum[i];
		int j, flag = 1;

		for (j = 0; j < m_TempNum; j++)
		{
			TRect r2 = pTemRect[j];
			int num2 = pNum[j];
			int distance = (int)((r2.iBr.iX - r2.iTl.iX) * 0.2);

			if (i != j &&
				r1.iTl.iX >= r2.iTl.iX - distance &&
				r1.iTl.iY >= r2.iTl.iY - distance &&
				r1.iBr.iX <= r2.iBr.iX + distance &&
				r1.iBr.iY <= r2.iBr.iY + distance &&
				num2 > num1)
			{
				flag = 0;
				break;
			}
		}

		if (flag != 0)
		{
			pDstRect[m_PedestrianNum] = r1;
			m_PedestrianNum++;
		}
	}

	return m_PedestrianNum;
}
// convolve two columns of I by ones filter
void IntChnPedestrianDetect::convBoxY(float *I, float *O, int h, int r)
{
	float t; int j, p = r + 1, q = 2 * h - (r + 1), h0 = r + 1, h1 = h - r, h2 = h;
	t = 0; for (j = 0; j <= r; j++) t += I[j]; t = 2 * t - I[r]; j = 0;
	for (; j<h0; j++) O[j] = t -= I[r - j] - I[r + j];
	for (; j<h1; j++) O[j] = t -= I[j - p] - I[r + j];
	for (; j<h2; j++) O[j] = t -= I[j - p] - I[q - j];
}
// convolve I by a 2r+1 x 2r+1 ones filter (uses SSE)
void IntChnPedestrianDetect::convBox(float *I, float *O, int h, int w, int d, int r)
{
	float nrm = 1.0f / ((2 * r + 1)*(2 * r + 1));
	int i, j;
	float *T = (float*)malloc(h * sizeof(float));
	while (d-- > 0)
	{
		// initialize T
		memset(T, 0, h * sizeof(float));

		for (i = 0; i <= r; i++) for (j = 0; j<h; j++) T[j] += I[j + i*h];
		for (j = 0; j<h; j++) T[j] = nrm*(2 * T[j] - I[j + r*h]);

		for (i = 0; i<w; i++)
		{
			float *Il, *Ir; Il = Ir = I;
			if (i <= r) { Il += (r - i)*h; Ir += (r + i)*h; }
			else if (i<w - r) { Il -= (r - i + 1)*h; Ir += (r + i)*h; }
			else { Il -= (r - i + 1)*h; Ir += 2 * w*h - (r + i + 1)*h; }
			if (i) for (j = 0; j<h; j++) T[j] -= nrm*(Il[j] - Ir[j]);
			convBoxY(T, O, h, r); O += h;
		}

		I += w*h;
	}
	free(T);
}
void IntChnPedestrianDetect::grad1(float *I, float *Gx, float *Gy, int h, int w, int x)
{
	int y;
	float *Ip, *In, r;
	// compute column of Gx
	Ip = I - h;
	In = I + h;
	r = .5f;
	if (x == 0)
	{
		r = 1; Ip += h;
	}
	else if (x == w - 1)
	{
		r = 1; In -= h;
	}
	for (y = 0; y<h; y++)
		*Gx++ = (*In++ - *Ip++)*r;

	Ip = I; In = Ip + 1;
	r = 1;
	*Gy++ = (*In++ - *Ip++)*r;

	Ip--; r = 0.5f;
	for (y = 1; y<h - 1; y++)
		*Gy++ = (*In++ - *Ip++)*r;

	In--; r = 1;
	*Gy++ = (*In++ - *Ip++)*r;
}
// build lookup table a[] s.t. a[dx/2.02*n]~=acos(dx)
float* IntChnPedestrianDetect::acosTable()
{
	int i, n = 25000, n2 = n / 2;
	float t, ni;
	static float a[25000];
	static bool init = false;
	if (init)
		return a + n2;
	ni = 2.02f / (float)n;
	for (i = 0; i<n; i++)
	{
		t = i*ni - 1.01f;
		t = t<-1 ? -1 : (t>1 ? 1 : t);
		t = (float)acos(t);
		a[i] = (t <= PI - 1e-5f) ? t : 0;
	}
	init = true;
	return a + n2;
}
// compute gradient magnitude and orientation at each location (uses sse)
void IntChnPedestrianDetect::gradMag(float *I, float *M, float *O, int h, int w, int d)
{
	int x, y, y1, c, s;
	float *Gx, *Gy, *M2;
	LOGD("before acosTable ");
	float *acost = acosTable(), acMult = 25000 / 2.02f;
	LOGD("acosTable succeeded ");
	// allocate memory for storing one column of output (padded so h4%4==0)
	s = d*h * sizeof(float);
	M2 = (float*)malloc(s);
	Gx = (float*)malloc(s);
	Gy = (float*)malloc(s);
	LOGD("malloc succeeded ");
	// compute gradient magnitude and orientation for each column
	for (x = 0; x<w; x++)
	{
		// compute gradients (Gx, Gy) and squared magnitude (M2) for each channel
		for (c = 0; c<d; c++)
			grad1(I + x*h + c*w*h, Gx + c*h, Gy + c*h, h, w, x);

		for (y = 0; y<d*h; y++)
			M2[y] = Gx[y] * Gx[y] + Gy[y] * Gy[y];

		// store gradients with maximum response in the first channel
		for (c = 1; c<d; c++)
		{
			for (y = 0; y<h; y++)
			{
				y1 = h*c + y;
				if (M2[y1] > M2[y])
				{
					M2[y] = M2[y1];
					Gx[y] = Gx[y1];
					Gy[y] = Gy[y1];
				}
			}
		}
		// compute gradient mangitude (M) and normalize Gx
		for (y = 0; y<h; y++)
		{
			M2[y] = sqrt(M2[y]);
			if (M2[y] < 1e-10f)
			{
				M2[y] = 1e-10f;
			}
			Gy[y] = Gy[y] / M2[y] * acMult;
			if (Gx[y] <= -0.0f)
				Gy[y] = -Gy[y];
		};
		memcpy(M + x*h, M2, h * sizeof(float));

		if (O != 0)
			for (y = 0; y<h; y++)
				O[x*h + y] = acost[(int)Gy[y]];
	}
	LOGD("calculation succeeded , before free");
	free(Gx); free(Gy); free(M2);
}
// normalize gradient magnitude at each location (uses sse)
void IntChnPedestrianDetect::gradMagNorm(float *M, float *S, int h, int w, float norm)
{
	int i = 0, n = h*w, n4 = n / 4;
	for (; i<n; i++) M[i] /= (S[i] + norm);
}
// helper for gradHist, quantize O and M into O0, O1 and M0, M1
void IntChnPedestrianDetect::gradQuantize(float *O, float *M, int *O0, int *O1, float *M0, float *M1, int nOrients, int nb, int n, float norm)
{
	// assumes all *OUTPUT* matrices are 4-byte aligned
	int i, o0, o1; float o, od, m;
	// define useful constants
	const float oMult = (float)nOrients / PI; const int oMax = nOrients*nb;
	// compute trailing locations without sse
	for (i = 0; i<n; i++) {
		o = O[i] * oMult; m = M[i] * norm; o0 = (int)o; od = o - o0;
		o0 *= nb; o1 = o0 + nb; if (o1 == oMax) o1 = 0;
		O0[i] = o0; O1[i] = o1; M1[i] = od*m; M0[i] = m - M1[i];
	}
}

// compute nOrients gradient histograms per bin x bin block of pixels
void IntChnPedestrianDetect::gradHist(float *M, float *O, float *H, int h, int w, int bin, int nOrients, bool softBin)
{
	const int hb = h / bin, wb = w / bin, h0 = hb*bin, w0 = wb*bin, nb = wb*hb;
	const float s = (float)bin, sInv = 1 / s, sInv2 = 1 / s / s;
	float *H1, *M0, *M1; int x, y; int *O0, *O1;
	O0 = (int*)malloc(h * sizeof(int)); M0 = (float*)malloc(h * sizeof(float));
	O1 = (int*)malloc(h * sizeof(int)); M1 = (float*)malloc(h * sizeof(float));

	// main loop
	for (x = 0; x<w0; x++) {
		// compute target orientation bins for entire column - very fast
		gradQuantize(O + x*h, M + x*h, O0, O1, M0, M1, nOrients, nb, h0, sInv2);

		if (!softBin || bin == 1)
		{
			// interpolate w.r.t. orientation only, not spatial bin
			H1 = H + (x / bin)*hb;
#define GH H1[O0[y]]+=M0[y]; H1[O1[y]]+=M1[y]; y++;
			if (bin == 1)      for (y = 0; y<h0;) { GH; H1++; }
			else if (bin == 2) for (y = 0; y<h0;) { GH; GH; H1++; }
			else if (bin == 3) for (y = 0; y<h0;) { GH; GH; GH; H1++; }
			else if (bin == 4) for (y = 0; y<h0;) { GH; GH; GH; GH; H1++; }
			else for (y = 0; y<h0;) { for (int y1 = 0; y1<bin; y1++) { GH; } H1++; }
#undef GH

		}
	}
	free(O0); free(O1); free(M0); free(M1);
}

float* IntChnPedestrianDetect::rgb2luv_setup(float z, float *mr, float *mg, float *mb, float &minu, float &minv, float &un, float &vn)
{
	// set constants for conversion
	const float y0 = (float)((6.0 / 29)*(6.0 / 29)*(6.0 / 29));
	const float a = (float)((29.0 / 3)*(29.0 / 3)*(29.0 / 3));
	un = (float) 0.197833; vn = (float) 0.468331;
	mr[0] = (float) 0.430574*z; mr[1] = (float) 0.222015*z; mr[2] = (float) 0.020183*z;
	mg[0] = (float) 0.341550*z; mg[1] = (float) 0.706655*z; mg[2] = (float) 0.129553*z;
	mb[0] = (float) 0.178325*z; mb[1] = (float) 0.071330*z; mb[2] = (float) 0.939180*z;
	float maxi = (float) 1.0 / 270; minu = -88 * maxi; minv = -134 * maxi;
	// build (padded) lookup table for y->l conversion assuming y in [0,1]
	static float lTable[1064]; static bool lInit = false;
	if (lInit) return lTable; float y, l;
	for (int i = 0; i<1025; i++) {
		y = (float)(i / 1024.0);
		l = y>y0 ? 116 * (float)pow((double)y, 1.0 / 3.0) - 16 : y*a;
		lTable[i] = l*maxi;
	}
	for (int i = 1025; i<1064; i++) lTable[i] = lTable[i - 1];
	lInit = true; return lTable;
}
void IntChnPedestrianDetect::rgb2luv(float *I, float *J, int n, float nrm)
{
	float minu, minv, un, vn, mr[3], mg[3], mb[3];
	float *lTable = rgb2luv_setup(nrm, mr, mg, mb, minu, minv, un, vn);
	float *L = J, *U = L + n, *V = U + n; float *R = I, *G = R + n, *B = G + n;
	for (int i = 0; i<n; i++) {
		float r, g, b, x, y, z, l;
		r = (float)*R++; g = (float)*G++; b = (float)*B++;
		x = mr[0] * r + mg[0] * g + mb[0] * b;
		y = mr[1] * r + mg[1] * g + mb[1] * b;
		z = mr[2] * r + mg[2] * g + mb[2] * b;
		l = lTable[(int)(y * 1024)];
		*(L++) = l; z = 1 / (x + 15 * y + 3 * z + (float)1e-35);
		*(U++) = l * (13 * 4 * x*z - 13 * un) - minu;
		*(V++) = l * (13 * 9 * y*z - 13 * vn) - minv;
	}
}

JNIEXPORT void JNICALL Java_PedDetect_Initialization
(JNIEnv *, jobject)
{
}

void m_BtnInitParam_Click(PhoneComPedestrianDetect::IntChnPedestrianDetect * m_PedestrianDetect, PtrToParam MyParam)
{
	bool m_FlagState = m_PedestrianDetect->InitDetect(MyParam->m_ImageWidth, MyParam->m_ImageHeight, 1);
	//bool m_FlagState = true;
	if (m_FlagState == false)
	{
		LOGD( "Param Error");
	}
	else
	{
		LOGD("Param Initialized");
	}
}

JNIEXPORT jintArray JNICALL Java_com_bn_ex7f_PedDetect_Start_1Detect
(JNIEnv * env, jobject obj, jintArray MyImage_jint, jint width,jint height)
{
	/*if (fopen("mnt/sdcard/PED_DETECT/hello.txt", "r") != 0)
	{
		LOGD("%s","!=0");
	}
	else

	{
		LOGD("%s", "=0");
	}*/
	//return false;
	//jintArray ja;
	//ja = env->NewIntArray(env,10);

	//return MyImage_jint;

	//MyImage = env->GetIntArrayElements(MyImage_jint, JNI_FALSE);
	jint *MyImage;
	
	MyImage = env->GetIntArrayElements(MyImage_jint, JNI_FALSE);
	if (MyImage == NULL) {
		return 0;
	}
	//printf("First Checking Point");
	LOGD("%s", "checkpoint 1");
	PhoneComPedestrianDetect::IntChnPedestrianDetect * m_PedestrianDetect;
	m_PedestrianDetect = new PhoneComPedestrianDetect::IntChnPedestrianDetect();
	//printf("Second Checking Point");
	LOGD("%s", "checkpoint 2");
	PtrToParam MyParam = (PtrToParam)malloc(sizeof(struct Param));
	//MyParam->m_ImageHeight = (int) height;
	//MyParam->m_ImageWidth =  (int) width;
	//printf("Third Checking Point");
	MyParam->m_ImageHeight = (int)height;
	MyParam->m_ImageWidth = (int)width;
	LOGD("m_ImageHeight=%d,m_ImageWidth=%d", MyParam->m_ImageHeight, MyParam->m_ImageWidth);
	LOGD("%s", "checkpoint 3");
	m_BtnInitParam_Click(m_PedestrianDetect, MyParam);
	LOGD("%s", "checkpoint 4");
    int * Result = m_PedestrianDetect->StartDetect(MyImage,MyParam);
	LOGD("%s", "checkpoint 5");
	int NumOfBoundingBox_Yuxuan = *Result;
	LOGD("NumOfBoundingBox_Yuxuan=%d", NumOfBoundingBox_Yuxuan);
		for (int i = 1; i < NumOfBoundingBox_Yuxuan + 1; i++)
		{
			/* pRect [i - 1].iTl.iX = Result[4 * i - 3] ;
			pRect[i - 1].iTl.iY = Result[4 * i - 2] ;
			pRect[i - 1].iBr.iX = Result[4 * i - 1];
			pRect[i - 1].iBr.iY = Result[4 * i];
			*/
			//cvRectangle(MyImage, cvPoint(Result[4 * i - 3], Result[4 * i - 2]), cvPoint(Result[4 * i - 1], Result[4 * i]), cvScalar(255, 0, 0), 1, 1, 0);
			//cout<<endl <<"("<< pRect[i - 1].iTl.iX<<","<< pRect[i - 1].iTl.iY<<")"<<" "<<"("<< pRect[i - 1].iBr.iX<< ","<<pRect[i - 1].iBr.iY<<")"<<endl;
			//cout << endl << "(" << Result[4 * i - 3] << "," << Result[4 * i - 2] << ")" << " " << "(" << Result[4 * i - 1] << "," << Result[4 * i] << ")" << endl;
			LOGD("%d,%d,%d,%d", Result[4 * i - 3], Result[4 * i - 2], Result[4 * i - 1], Result[4 * i]);
		}
	int * NumWithFourPointsForBoundingBoxes = new int[NumOfBoundingBox_Yuxuan * 4];
	for (int i = 0; i < NumOfBoundingBox_Yuxuan * 4 + 1; i++)
	{
		NumWithFourPointsForBoundingBoxes[i] = Result[i];
	}
	jintArray result = env->NewIntArray(NumOfBoundingBox_Yuxuan * 4+1);
	LOGD("jintArray result created");
	env->SetIntArrayRegion(result, 0, NumOfBoundingBox_Yuxuan * 4+1, NumWithFourPointsForBoundingBoxes);
	LOGD("jintArray result seted");
	env->ReleaseIntArrayElements(MyImage_jint, MyImage, JNI_ABORT);
	LOGD("MyImage Released");
	m_PedestrianDetect->ReleaseDetect();
	LOGD("Detection Released");
	free(MyParam);
	LOGD("MyParam Released");
	delete m_PedestrianDetect;
	delete [] NumWithFourPointsForBoundingBoxes;
	return result;	
}