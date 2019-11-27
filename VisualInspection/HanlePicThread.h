#ifndef HANLEPICTHREAD_H
#define HANLEPICTHREAD_H
#include <QThread>
#include "opencv2/opencv.hpp"
#include <QList>
#include "Packaging_Inspection.h"
//using namespace  cv;
class HanlePicThread : public QThread
{
    Q_OBJECT
public:
    HanlePicThread();
    void run();
public:
    bool ThreadRunnning = false;
    QList<HObject>HalconObjlist;
    HObject CurObj;
    CMyMatchClass MatchObj;
    void Mat2HObject(const cv::Mat &image, HObject &outObj);
signals:
    void SendCompareResult(int);
protected slots:
    void GetFrame(cv::Mat Frame);
    void GetTempaltePic(cv::Mat Frame);
};

#endif // HANLEPICTHREAD_H
