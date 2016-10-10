#pragma once

#include <vector>
#include <string> 
#include <math.h>
#include<stdio.h>
#include <stdlib.h>

using namespace std;
typedef struct Param *PtrToParam;
//全局变量

struct Param {
    int m_ImageWidth, m_ImageHeight;
    bool m_FlagDetect;
};


namespace PhoneComPedestrianDetect {
#define  PI 3.1415926535897931f

    class IntChnPedestrianDetect {
    private:
        struct TxtChnImage1 {
            int width;
            int height;
            int nChannels;
            int *data;
        };
        struct CascadeModelSize {
            int width;
            int height;
        };
        struct TxtChnImage {
            int width;
            int height;
            int nChannels;
            float *data;
        };
        struct TxtIntegralImage {
            int width;
            int height;
            int nChannels;
            float *avg;
            float *var;
            float *data;
        };
        struct TPoint {
            int iX;
            int iY;
        };
        struct TRect {
            TPoint iTl;
            TPoint iBr;
        };
        struct ChnFeature {
            int type;
            int x, y;
            int dx, dy;
        };
        struct WeakClassfier {
            short threshold;
            double realthres;
            double alphaR;
            double alphaL;
            int FeaID;
            ChnFeature f;
            ///by viola
            bool m_PosLeftFlag;
            double m_Beta;
            double m_CombinePara;
        };
        struct StrongClassfier {
            double rawth;
            std::vector<WeakClassfier> weakclassfierset;
        };
        struct MultiScaleClassfier {
            int width;
            int height;
            std::vector<StrongClassfier> CascadeClassfier;
        };

    private:
        std::vector<std::vector<double> > m_MultiRealSplitTheta;
        std::vector<double> m_RealSplitTheta;
        std::vector<MultiScaleClassfier> m_RealMultiScaleClassfier;
        std::vector<StrongClassfier> m_CascadeClassfier;
        std::vector<double> m_DetaTheta;
        int m_MinNeighbors;
        double m_DetectScale;
        int m_DetectResizeNum;

        TxtIntegralImage m_SumImage;
        TxtIntegralImage m_Sum2Image;
        TxtChnImage m_ChannelsImage;
        TxtChnImage m_OriImage;
        float *pGradOrient;
        float *pGradMagSmooth;

        TRect *pPedestrianRect;
        TRect *pDetectRect;
        TRect *pMiddleRect;
        int *pMiddleNum;

    public:
        IntChnPedestrianDetect();

        int *StartDetect(int *OpencvImage, PtrToParam MyParam);

        bool InitDetect(int m_ImageWidth, int m_ImageHeight, bool m_FlagWhole);

        void ReleaseDetect();

    private:
        // 计算梯度及梯度直方图
        void grad1(float *I, float *Gx, float *Gy, int h, int w, int x);

        float *acosTable();

        void gradMag(float *I, float *M, float *O, int h, int w, int d);

        void gradMagNorm(float *M, float *S, int h, int w, float norm);

        void gradQuantize(float *O, float *M, int *O0, int *O1, float *M0, float *M1, int nOrients,
                          int nb, int n, float norm);

        void gradHist(float *M, float *O, float *H, int h, int w, int bin, int nOrients,
                      bool softBin);

        // 计算LUV特征
        float *rgb2luv_setup(float z, float *mr, float *mg, float *mb, float &minu, float &minv,
                             float &un, float &vn);

        void rgb2luv(float *I, float *J, int n, float nrm);

        // 平滑图像
        void convBoxY(float *I, float *O, int h, int r);

        void convBox(float *I, float *O, int h, int w, int d, int r);

        // 转换图像格式
        void ConvertImageFormat(int *pRGBData, int len, TxtChnImage &m_OriImage);

        // 初始化内存分配及相关参数
        bool InitDetectParam(vector<string> MultiModelPath, vector<string> MultiSplitThetaPath,
                             vector<CascadeModelSize> MultiModelSize, int m_ImageWidth,
                             int m_ImageHeight, int MinNeighbors, vector<double> DetaTheta,
                             double DetectScale);

        void InitMemoryMalloc(int m_ImageWidth, int m_ImageHeight);

        // 检测图像
        int DetectImage(TxtChnImage &m_Image, TRect *pRect);

        // 计算一幅给定图像的通道特征及积分图
        void CalChannelsFeature(TxtChnImage &m_OriImage, TxtChnImage &m_ChannelsImage);

        void CalChannelsFeaIntegral(const TxtChnImage &m_ChannelsFea,
                                    TxtIntegralImage &m_IntegralImage);

        void CalChannelsFeaIntegral(TxtIntegralImage &m_IntegralImage,
                                    TxtIntegralImage &m_IntegralImage2);

        // 计算Cacade过程
        bool IsExistObjectInScaleCascade(TxtIntegralImage &m_Image, int m_PointX, int m_PointY);

        double StrongClassValue(StrongClassfier &sc, TxtIntegralImage &m_Image, int m_PointX,
                                int m_PointY);

        double WeakClassValue(WeakClassfier &wc, TxtIntegralImage &m_Image, int m_PointX,
                              int m_PointY);

        double FeatureValue(ChnFeature &f, TxtIntegralImage &m_Image, int m_PointX, int m_PointY);

        // 载入相关模型
        bool LoadMultiSplitTheta(std::vector<std::vector<double> > &m_MultiSplitTheta,
                                 std::vector<std::string> m_SplitThetaPath);

        bool LoadCasacadeModel(std::vector<std::vector<StrongClassfier> > &m_MultiModelClassfier,
                               std::vector<std::string> m_CasacadeMultiModelFile);

        void LoadWeakClassfier(std::vector<WeakClassfier> &wcset, FILE *fp);

        bool LoadCasacadeModel(std::string m_CasacadeModelFile);

        int RectPartition(TRect *pDstRect, TRect *pOriRect, int m_DetectNum, int m_MinNeighbors);

        bool RectIsEqual(TRect &m_Rect1, TRect &m_Rect2);

    };
}