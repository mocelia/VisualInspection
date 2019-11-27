#ifndef VISUALINSPECTION_H
#define VISUALINSPECTION_H

#include <QWidget>
#include "opencv2/opencv.hpp"
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPort>
#include <QTimer>
#include <QList>
#include "HanlePicThread.h"
//using namespace cv;

namespace Ui {
class VisualInspection;
}

class VisualInspection : public QWidget
{
    Q_OBJECT

public:
    explicit VisualInspection(QWidget *parent = nullptr);
    ~VisualInspection();

    QImage cvMat_to_QImage(const cv::Mat &mat);

    qint64 WritePort(QString str);
private slots:
    void on_pushButton_clicked();
    void on_PortBox_Clicked();
    void on_pushButton_2_clicked();

private:
    Ui::VisualInspection *ui;

private:
    cv::Mat Frame1;
    cv::Mat Frame2;
    QTimer GetFrameTimer;
    QTimer SendCommandTimer;
    QSerialPort *Serial;
    cv::VideoCapture cap;
    QByteArray buff;
    bool IsAnotherOne1 = false;
    bool IsAnotherOne2 = false;
    QList<bool> CompareResult;
    int Index = 0;
    HanlePicThread ThreadObj;
    bool IsVerticalCyDropDown = false;
    bool IsHorizontalCy = false;

protected slots:
    void GetFrameFromCap();
    void RecvMessgae();
    void SendCommand();
    void GetCompareResult(int ok);

signals:
    void SendFrame(cv::Mat f);
    void SendTemplate(cv::Mat f);
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // VISUALINSPECTION_H
