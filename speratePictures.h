#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_speratePictures.h"
#include <QProgressBar>
#include <opencv2/opencv.hpp>
class speratePictures : public QMainWindow
{
    Q_OBJECT

public:
    speratePictures(QWidget *parent = Q_NULLPTR);


protected slots:
    void openSlot();
    void saveSLOT();
    void rotateSlot();
    void randomSpliteSlot();
    void selectDirSlot();
    void selectDirRandomSlot();
    void tif2pngSlot();
    void renameSortSlot();
    void unifimPathSlot();
    void randomSelSlot();
    void randomSelRotateSlot();
    void dataAnalyseSlot();
    void sp1wslot();
protected:
    void wheelEvent(QWheelEvent* event);
    void paintEvent(QPaintEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);

    virtual void mousePressEvent(QMouseEvent* event);


private:
    void savePicture(const QString& path,Qt::HANDLE curHandle, const QString& dir="");
	void saveRandomPicture(const QString& path, Qt::HANDLE curHandle, int count = 1000, const QString& dir = "");
    void getDirFiles(const QString& dir, QStringList& fileNames,bool isFindFolder=true,const QStringList& fileFilters=QStringList());
    void renameFile(const QString& path, const QString& oldName, const QString& newName);
    void initApp();
    void creatProgress();
    void setProgressValue(int i, int count);
    void closeProgress();
    void zoomIn();
    void zoomOut();
	int rotateImage(const cv::Mat& src, cv::Mat& dst, const double angle, const int mode);
	int rotateImage2(const cv::Mat& src, cv::Mat& dst, const double angle, const int mode);
    void log(const QString& logText);
    QString getXmlPath(const QString& picPath);
    void uniformDirPath(const QString& path, const QString& newDir);
    QGraphicsScene* m_scene;
    QGraphicsPixmapItem* m_item;
    QProgressBar* m_progress;
    QString m_path;
    double m_scale;
private:
    Ui::speratePicturesClass ui;
};
