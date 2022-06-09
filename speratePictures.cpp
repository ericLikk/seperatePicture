
#include "speratePictures.h"
#include <QFileDialog>
#include <QImage>
#include <QFileInfo>
#include <QPixmap>
#include <QFile>
#include <QGraphicsView>
#include <QImage>
#include <QImageReader>
#include <QGraphicsPixmapItem> 
#include <QDebug>
#include <QWheelEvent>
#include <QInputDialog>
#include <QThread>
#include <omp.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <windows.h>
using namespace std;
using namespace cv;

#if _MSC_VER>=1600
#pragma execution_character_set("utf-8")
#endif
speratePictures::speratePictures(QWidget *parent)
    : QMainWindow(parent), m_scene(nullptr),m_item(nullptr), m_progress(nullptr)
{
    ui.setupUi(this);
	m_progress = new QProgressBar(this);
	ui.statusBar->addPermanentWidget(m_progress, 1);
    m_progress->setVisible(false);

    initApp();
}



void speratePictures::log(const QString& logText)
{
    ui.statusBar->showMessage(logText);
}
void speratePictures::openSlot()
{
	cv::Mat img;
	cv::Rect m_select;
    //QString dir = QFileInfo(m_path).absolutePath();
    m_path = QFileDialog::getOpenFileName(this, "��", m_path, "*.*");
    if (m_path.isEmpty())
    {
        ui.statusBar->showMessage("ȡ��������");
        return;
    }
    img = imread(m_path.toLocal8Bit().toStdString());
	int ww = img.cols;
	int hh = img.rows;
    //QPixmap* painterPix= new QPixmap();//�½�һ��ͼ
    //QImage imgRed= QImage(img.data, ww, hh, QImage::Format_RGB32);
		//��ʹ���Ѿ�֪��ͼ���ߺ͸�ʽ������£���һ�ַ�����Ȼ�޷��ɹ�ת��

	////����1  ����
	//QImage image((unsigned char*)imgData, 1920, 1080, QImage::Format_RGB32);

	////����2  QByteArrayҲ����
	//image.loadFromData((unsigned char*)imgData, readLen, "JPG");
    QImage image;
	////����3  QByteArrayҲ����
	//image = image.fromData((unsigned char*)imgData, readLen, "JPG");

    QPixmap painterPix =QPixmap(m_path) /*QPixmap::fromImage(imgRed)*/;
    //QImage painterImage(m_path);
    //int ww = painterPix.width();
    //int hh = painterPix.height();

    if (painterPix.isDetached())
    {
        int ccc = 0;
    }
    if (painterPix.isNull())
    {
        ui.statusBar->showMessage(QString("��ʧ�ܣ�%01").arg(m_path));
        return;
    }
    if (painterPix.isQBitmap())
    {
        int ccc = 0;
    }
    m_item->setPixmap(painterPix.scaledToWidth(height()));//��ͼ��ӽ�item
    //item->setScale(0.05);

    //m_scene->setSceneRect(0, 0, ui.graphicsView->width(), ui.graphicsView->height());

    qDebug() << "black pos:" << m_item->pos() << "black pixPos:" << m_item->mapToParent(m_item->pixmap().rect().center()) << "pix cenPos:" << m_item->pixmap().rect().center();
    ui.statusBar->showMessage(QString("�򿪳ɹ���%01 ��%02 �ߣ�%03").arg(m_path).arg(ww).arg(hh));
}

void speratePictures::saveSLOT()
{
	cv::Mat img;
	cv::Rect m_select;
    QString dirSec = QFileInfo(m_path).absolutePath();
    QString dirPath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirSec);
    if (dirPath.isEmpty())
    {
        log("ѡ��·��Ϊ�գ�ȡ��������");
        return;
    }
    QDir dir(dirPath);
    if (!dir.exists())
    {
        dir.mkpath(dirPath);
    }

    QPixmap pix = m_item->pixmap();
    img = imread(m_path.toLocal8Bit().toStdString());
    if (pix.isNull())
    {
        log("pix is null!");
        return;
    }
    //int width = pix.width();
    //int height = pix.height();
	int width = img.cols;
	int height = img.rows;
    int count = 1;
    int allCount = 0;
    for (int i = 0; i < width; i += 800)
    {
        for (int j = 0; j < height; j += 800)
        {
            allCount++;
        }
    }

    creatProgress();
    Qt::HANDLE curThread = QThread::currentThreadId();
#pragma omp parallel for
    for (int i = 0;i<width;i+=800)
    {
        for (int j=0;j<height;j+=800)
        {
            int m = i, n = j;
            if (curThread==QThread::currentThreadId())
            {
                qDebug() << QThread::currentThreadId();
                setProgressValue(count, allCount);
            }

            if (i+800>=width)
            {
                m = width - 800;
            }
			if (j + 800 >= height)
			{
				n = height - 800;
			}
            //QPixmap curMap = pix.copy(m, n, 800, 800);
			m_select = Rect(m, n, 800, 800);
			Mat ROI = img(m_select);
            QString newPath = m_path.left(m_path.indexOf(".")) + QString("_%01.png").arg(count++);
            newPath = dirPath + "/" + QFileInfo(newPath).fileName();
            imwrite(newPath.toLocal8Bit().toStdString(), ROI);
            //bool isSave= curMap.save(newPath);
            //QImage image2=curMap.toImage();
            //isSave= image2.save(newPath);
            //int ccc = 0;
        }
    }
    closeProgress();
    ui.statusBar->showMessage(QString("������ϣ�%01").arg(dirPath));
}

void speratePictures::rotateSlot()
{
    double rotate= QInputDialog::getDouble(this, "����", "��������ת�Ƕȣ�");
    QTransform tran;
    tran.rotate(rotate);
    m_item->setPixmap(m_item->pixmap().transformed(tran));
    QRectF rect= m_item->boundingRect();
    int ccc = 0;
    ui.statusBar->showMessage(QString("��ת�ɹ���ϣ�%01��").arg(rotate));
}

void speratePictures::randomSpliteSlot()
{
	cv::Mat img;
	cv::Rect m_select;
    int countAll = QInputDialog::getInt(this, "����", "�������и�ͼƬ����", 200);
    if (countAll <= 0)
    {
        log("count <=0!");
        return;
    }


    QString dirSec = QFileInfo(m_path).absolutePath();
    QString dirPath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirSec);
    if (dirPath.isEmpty())
    {
        log("ѡ��·��Ϊ�� ȡ��������");
        return;
    }
    QDir dir(dirPath);
    if (!dir.exists())
    {
        dir.mkpath(dirPath);
    }

    QPixmap pix = m_item->pixmap();
    img = imread(m_path.toLocal8Bit().toStdString());
	//int width = pix.width();
	//int height = pix.height();
	int width = img.cols;
	int height = img.rows;
    int count = 1;
    int allCount = countAll;
    
    if (0 == width || 0 == height)
    {
        ui.statusBar->showMessage("��ȡ�ļ�ʧ�ܣ�");
        return;
    }
    creatProgress();
    qsrand(time(nullptr));
    Qt::HANDLE curThread = QThread::currentThreadId();
#pragma omp parallel for
    for (int h = 0; h < countAll; h++)
    {
        int i = 0;
        int j = 0;
        i = qrand() % width;
        j = qrand() % height;
		if (curThread == QThread::currentThreadId())
		{
			qDebug() << QThread::currentThreadId();
			setProgressValue(count, allCount);
		}
        if (i + 800 >= width)
        {
            i = width - 800;
        }
        if (j + 800 >= height)
        {
            j = height - 800;
        }
        //QPixmap curMap = pix.copy(i, j, 800, 800);
		m_select = Rect(i, j, 800, 800);
		Mat ROI = img(m_select);
        QString newPath = m_path.left(m_path.indexOf(".")) + QString("_%01.png").arg(count++);
        newPath = dirPath + "/" + QFileInfo(newPath).fileName();
        //bool isSave = curMap.save(newPath);
        imwrite(newPath.toLocal8Bit().toStdString(), ROI);
        //QImage image2=curMap.toImage();
        //isSave= image2.save(newPath);
        //int ccc = 0;

    }
	closeProgress();
	ui.statusBar->showMessage(QString("������ϣ�%01").arg(dirPath));
}

void speratePictures::selectDirSlot()
{
	QString dirSec = QFileInfo(m_path).absolutePath();
	QString dirPath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirSec);
	QDir dir(dirPath);
	if (dirPath.isEmpty() || !dir.exists())
	{
        log("ѡ��·��Ϊ�� ����ȡ����");
		return;
	}
	dir.setFilter(QDir::Files);
	//dir.setNameFilters(QStringList() << "tif");
	creatProgress();
	int i = 0;
	QFileInfoList fileInfos = dir.entryInfoList();
	int count = fileInfos.count();
	//dir.mkdir("pngFolder");
    Qt::HANDLE curThread = QThread::currentThreadId();
#pragma omp parallel for
	for (int iter = 0; iter < fileInfos.count(); ++iter)
	{
		QFileInfo var = fileInfos.at(iter);
		if (curThread == QThread::currentThreadId())
		{
			qDebug() << QThread::currentThreadId();
			setProgressValue(count, count);
		}
		if (!var.isFile())
		{
			continue;
		}
		QString oldPath = var.absoluteFilePath();
        savePicture(oldPath, curThread);
		//QString newPath = var.absolutePath() + QString("/pngFolder/%01.png").arg(var.baseName());
		//QPixmap pix(oldPath);
		//if (pix.isNull())
		//{
		//	continue;
		//}
		//pix.save(newPath);

	}

	closeProgress();

	ui.statusBar->showMessage(QString("������ϣ�%01��").arg(dirPath));
}

void speratePictures::selectDirRandomSlot()
{
	QString dirSec = QFileInfo(m_path).absolutePath();
	QString dirPath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirSec);
	QDir dir(dirPath);
	if (dirPath.isEmpty() || !dir.exists())
	{
        log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}
    int randomCount = QInputDialog::getInt(this,"����", "����������ü���������", 1000);
	if (0>=randomCount)
	{
        log("count С�ڵ����� ȡ��������");
        return;
	}
    dir.setFilter(QDir::Files);
	//dir.setNameFilters(QStringList() << "tif");
	creatProgress();
	int i = 0;
	QFileInfoList fileInfos = dir.entryInfoList();
	int count = fileInfos.count();
	//dir.mkdir("pngFolder");
	Qt::HANDLE curThread = QThread::currentThreadId();
#pragma omp parallel for
	for (int iter = 0; iter < fileInfos.count(); ++iter)
	{
		QFileInfo var = fileInfos.at(iter);
		if (curThread == QThread::currentThreadId())
		{
			qDebug() << QThread::currentThreadId();
			setProgressValue(count, count);
		}
		if (!var.isFile())
		{
			continue;
		}
		QString oldPath = var.absoluteFilePath();
		saveRandomPicture(oldPath, curThread, randomCount);
		//QString newPath = var.absolutePath() + QString("/pngFolder/%01.png").arg(var.baseName());
		//QPixmap pix(oldPath);
		//if (pix.isNull())
		//{
		//	continue;
		//}
		//pix.save(newPath);

	}

	closeProgress();

	ui.statusBar->showMessage(QString("������ϣ�%01��").arg(dirPath));
}

void speratePictures::tif2pngSlot()
{
	QString dirSec = QFileInfo(m_path).absolutePath();
	QString dirPath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirSec);
    QDir dir(dirPath);
    if (dirPath.isEmpty()|| !dir.exists())
	{
        log("ѡ��·��Ϊ�գ�ȡ������!");
		return;
	}
    dir.setFilter(QDir::Files);
    //dir.setNameFilters(QStringList() << "tif");
    creatProgress();
    int i = 0;
    QFileInfoList fileInfos = dir.entryInfoList();
    int count = fileInfos.count();
    dir.mkdir("pngFolder");
    Qt::HANDLE curThread = QThread::currentThreadId();
#pragma omp parallel for
    for (int iter=0;iter<fileInfos.count();++iter)
    {
        QFileInfo var = fileInfos.at(iter);
		if (curThread == QThread::currentThreadId())
		{
			qDebug() << QThread::currentThreadId();
			setProgressValue(count, count);
		}
        if (!var.isFile())
        {
            continue;
        }
        QString oldPath = var.absoluteFilePath();
        QString newPath = var.absolutePath() + QString("/pngFolder/%01.png").arg(var.baseName());
        QPixmap pix(oldPath);
        if (pix.isNull())
        {
            continue;
        }
        pix.save(newPath);

    }

    closeProgress();

    ui.statusBar->showMessage(QString("������ϣ�%01��").arg(dirPath));
}

void speratePictures::renameSortSlot()
{
	QString dirSec = QFileInfo(m_path).absolutePath();
	QString dirPath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirSec);
	QDir dir(dirPath);
	if (dirPath.isEmpty() || !dir.exists())
	{
        log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}
    int indexStart = QInputDialog::getInt(this, "�������", "����������ʼλ�ã�",1, 0, 999);
    if (indexStart<=0)
    {
        indexStart = 1;
    }
    QStringList fileNamesCurDir;
    getDirFiles(dirPath, fileNamesCurDir);
    QMap<QString, QSet<QString>> curFileMaps;
    for (auto iterFile:fileNamesCurDir)
    {
        QString iterTmp = iterFile;
        iterTmp = iterTmp.remove("_rotate").remove("_SC").remove("_S1").replace("_1_", "_");
        QStringList iterList = iterTmp.split("_");
        for (int i=iterList.count()-1;i>=0;--i)
        {
            if ((iterList.at(i).length()>4)&&(iterList.at(i).length() < 10))
            {
                iterList.removeAt(i);
            }
        }
        QString iterCurKey = (QStringList() << iterList[0] << iterList[1]).join("_");
        //QString iterCurKey = iterTmp.left(iterTmp.lastIndexOf("_"));
        curFileMaps[iterCurKey].insert(iterFile);
        int ccc = 0;
    }
    for (auto iterMap=curFileMaps.begin();iterMap!=curFileMaps.end();++iterMap)
    {
        //
        QString key = iterMap.key();
        QStringList values = iterMap.value().toList();

        for (auto iterRename:values)
        {
            int i = indexStart;
            QString suffix = QFileInfo(iterRename).suffix();
            if (0!=suffix.compare("png"))
            {
                continue;
            }
            bool isRename = false;
            while (!isRename)
            {

                QString baseIndex = QString("%01").arg(i++);
                while (baseIndex.length() < 3)
                {
                    baseIndex.insert(0, '0');
                }
                QString baseName = QString("%01.%02").arg(baseIndex).arg(suffix);

                QString newName = QString("%01_%02").arg(key).arg(baseName);
                if (0==iterRename.compare(newName))
                {
                    isRename = true;
                    continue;
                }
                isRename = QFile::rename(iterRename, newName);
                if (!isRename)
                {
                    continue;
                }
                QString xmlPath = getXmlPath(iterRename);
                if (QFile::exists(xmlPath))
                {
                    renameFile(xmlPath, iterRename, newName);
                }
                int ccc = 0;
            }
        }

    }
    int ccc = 0;
    ui.statusBar->showMessage("��������ɣ�");
}

void speratePictures::unifimPathSlot()
{
	QString dirSec = QFileInfo(m_path).absolutePath();
	QString dirOpenPath = QFileDialog::getExistingDirectory(this, "ѡ���Ŀ¼��", dirSec);
	QDir dirOpen(dirOpenPath);
	if (dirOpenPath.isEmpty() || !dirOpen.exists())
	{
        log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}
	QString dirSavePath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirOpenPath);
	QDir dirSave(dirSavePath);
	if (dirSavePath.isEmpty() || !dirSave.exists())
	{
        log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}
    //
    QStringList fileNames;
    getDirFiles(dirOpenPath, fileNames);

    for (auto iterFile:fileNames)
    {
        QString iterSuffix = QFileInfo(iterFile).suffix();
        if (0 == iterSuffix.compare("xml"))
        {
            //�����xml�ļ�
            uniformDirPath(iterFile, dirSavePath);
        }

    }


}

void speratePictures::randomSelSlot()
{
    log("start ransel");
	QString dirSec = QFileInfo(m_path).absolutePath();
	QString dirOpenPath = QFileDialog::getExistingDirectory(this, "ѡ���Ŀ¼��", dirSec);
	QDir dirOpen(dirOpenPath);
	if (dirOpenPath.isEmpty() || !dirOpen.exists())
	{
		log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}
	QString dirSavePath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirOpenPath);
	QDir dirSave(dirSavePath);
	if (!dirSave.exists())
	{
        dirSave.mkpath(dirSavePath);
	}
    if (dirSavePath.isEmpty() || !dirSave.exists())
	{
		log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}

	//
	QStringList fileNames;
	getDirFiles(dirOpenPath, fileNames,false);

    int secCount = QInputDialog::getInt(this, "��ѡ", "ѡ����ѡ����", 100);
    if (secCount<=0)
    {
        log("sec is 0��");
    }
    int filesCount = fileNames.count();
    if (secCount>=fileNames.count())
    {
        secCount = fileNames.count();
    }
    creatProgress();
    //һ������
    qsrand(time(nullptr));
    int copyCount = 0;
    int copyError = 0;
    for (int i = 0; i < secCount; ++i)
    {
        int index = qrand() % filesCount;
        QString fileOld = fileNames.at(index);
		QString suffix = QFileInfo(fileOld).suffix();
		if (0 != suffix.compare("png", Qt::CaseInsensitive))
		{
            --i;
			continue;
		}
        setProgressValue(i, secCount);
        QString newFileName = dirSavePath + "/" + QFileInfo(fileOld).fileName();
        if (QFile::copy(fileOld,newFileName))
        {
            QFile::remove(fileOld);
            copyCount++;
        }
        else {
            copyError++;
        }
        fileNames.removeAt(index);
        filesCount--;
        //Sleep(20);
    }
    closeProgress();
    log(QString("success!count:%01 error:%02").arg(copyCount).arg(copyError));
}
int speratePictures::rotateImage(const cv::Mat& src, cv::Mat& dst, const double angle, const int mode)
{
	//mode = 0 ,Keep the original image size unchanged
	//mode = 1, Change the original image size to fit the rotated scale, padding with zero

	if (src.empty())
	{
		std::cout << "Damn, the input image is empty!\n";
		return -1;
	}

	if (mode == 0)
	{
		cv::Point2f center((src.cols - 1) / 2.0, (src.rows - 1) / 2.0);
		cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
		cv::warpAffine(src, dst, rot, src.size());//the original size
	}
	else {

		double alpha = -angle * CV_PI / 180.0;//convert angle to radian format 

		cv::Point2f srcP[3];
		cv::Point2f dstP[3];
		srcP[0] = cv::Point2f(0, src.rows);
		srcP[1] = cv::Point2f(src.cols, 0);
		srcP[2] = cv::Point2f(src.cols, src.rows);

		//rotate the pixels
		for (int i = 0; i < 3; i++)
			dstP[i] = cv::Point2f(srcP[i].x * cos(alpha) - srcP[i].y * sin(alpha), srcP[i].y * cos(alpha) + srcP[i].x * sin(alpha));
		double minx, miny, maxx, maxy;
		minx = std::min(std::min(std::min(dstP[0].x, dstP[1].x), dstP[2].x), float(0.0));
		miny = std::min(std::min(std::min(dstP[0].y, dstP[1].y), dstP[2].y), float(0.0));
		maxx = std::max(std::max(std::max(dstP[0].x, dstP[1].x), dstP[2].x), float(0.0));
		maxy = std::max(std::max(std::max(dstP[0].y, dstP[1].y), dstP[2].y), float(0.0));

		int w = maxx - minx;
		int h = maxy - miny;

		//translation
		for (int i = 0; i < 3; i++)
		{
			if (minx < 0)
				dstP[i].x -= minx;
			if (miny < 0)
				dstP[i].y -= miny;
		}

		cv::Mat warpMat = cv::getAffineTransform(srcP, dstP);
		cv::warpAffine(src, dst, warpMat, cv::Size(w, h));//extend size

	}//end else

	return 0;
}

int speratePictures::rotateImage2(const cv::Mat& src, cv::Mat& dst, const double angle, const int mode)
{
	//mode = 0 ,Keep the original image size unchanged
	//mode = 1, Change the original image size to fit the rotated scale, padding with zero

	if (src.empty())
	{
		std::cout << "Damn, the input image is empty!\n";
		return -1;
	}

	if (mode == 0)
	{
		cv::Point2f center((src.cols - 1) / 2.0, (src.rows - 1) / 2.0);
		cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
		cv::warpAffine(src, dst, rot, src.size());//the original size
	}
	else {

		double alpha = -angle * CV_PI / 180.0;//convert angle to radian format 

		cv::Point2f srcP[3];
		cv::Point2f dstP[3];
		srcP[0] = cv::Point2f(0, src.rows);
		srcP[1] = cv::Point2f(src.cols, 0);
		srcP[2] = cv::Point2f(src.cols, src.rows);

		//rotate the pixels
		for (int i = 0; i < 3; i++)
			dstP[i] = cv::Point2f(srcP[i].x * cos(alpha) - srcP[i].y * sin(alpha), srcP[i].y * cos(alpha) + srcP[i].x * sin(alpha));
		double minx, miny, maxx, maxy;
		minx = std::min(std::min(std::min(dstP[0].x, dstP[1].x), dstP[2].x), float(0.0));
		miny = std::min(std::min(std::min(dstP[0].y, dstP[1].y), dstP[2].y), float(0.0));
		maxx = std::max(std::max(std::max(dstP[0].x, dstP[1].x), dstP[2].x), float(0.0));
		maxy = std::max(std::max(std::max(dstP[0].y, dstP[1].y), dstP[2].y), float(0.0));

		int w = maxx - minx;
		int h = maxy - miny;

		cv::Mat warpMat = cv::Mat::zeros(cv::Size(3, 2), CV_64F);//rows=2,cols=3

		std::cout << warpMat.type() << std::endl;
		std::cout << warpMat.size() << std::endl;
		warpMat.at<double>(0, 0) = cos(alpha);
		warpMat.at<double>(0, 1) = 0 - sin(alpha);
		warpMat.at<double>(1, 0) = sin(alpha);
		warpMat.at<double>(1, 1) = cos(alpha);
		warpMat.at<double>(0, 2) = 0 - minx;
		warpMat.at<double>(1, 2) = 0 - miny;
		//std::cout << warpMat;
		cv::warpAffine(src, dst, warpMat, cv::Size(w, h));//extend size

	}//end else

	return 0;
}
void speratePictures::randomSelRotateSlot()
{
	log("start ransel");
	QString dirSec = QFileInfo(m_path).absolutePath();
	QString dirOpenPath = QFileDialog::getExistingDirectory(this, "ѡ���Ŀ¼��", dirSec);
	QDir dirOpen(dirOpenPath);
	if (dirOpenPath.isEmpty() || !dirOpen.exists())
	{
		log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}
	QString dirSavePath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirOpenPath);
	QDir dirSave(dirSavePath);
	if (!dirSave.exists())
	{
		dirSave.mkpath(dirSavePath);
	}
	if (dirSavePath.isEmpty() || !dirSave.exists())
	{
		log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}

	//
	QStringList fileNames;
	getDirFiles(dirOpenPath, fileNames, false);

	int secCount = QInputDialog::getInt(this, "��ѡ", "ѡ����ѡ����", 100);
	if (secCount <= 0)
	{
		log("sec is 0��");
	}
	int filesCount = fileNames.count();
	if (secCount >= fileNames.count())
	{
		secCount = fileNames.count();
	}
	creatProgress();
	//һ������
	qsrand(time(nullptr));
	int copyCount = 0;
	int copyError = 0;
	for (int i = 0; i < secCount; ++i)
	{
		int index = qrand() % filesCount;
		QString fileOld = fileNames.at(index);
		setProgressValue(i, secCount);
        QString suffix = QFileInfo(fileOld).suffix();
        if (0!=suffix.compare("png",Qt::CaseInsensitive))
        {
            --i;
            continue;
        }
        QString newFileName = dirSavePath + "/" + QFileInfo(fileOld).baseName()+"_rotate."+ QFileInfo(fileOld).suffix();
        bool isWriteTr = false;
        cv::Mat img;
        cv::Mat transposedImage;
		//��ȡһ��ͼƬ
        img = imread(fileOld.toLocal8Bit().toStdString());
        rotateImage(img, transposedImage, 90, 0);
        isWriteTr = imwrite(newFileName.toLocal8Bit().toStdString(), transposedImage);
        
        //��ʾԭͼƬ��
		//cv2.imshow("sourceImage", sourceImage)
		//ת����ͼƬ
		//transposedImage = transpose(img)
        if (isWriteTr)
		{
			//QFile::remove(fileOld);
			copyCount++;
		}
		else {
			copyError++;
		}
		fileNames.removeAt(index);
		filesCount--;
		//Sleep(20);
	}
	closeProgress();
	log(QString("success!count:%01 error:%02").arg(copyCount).arg(copyError));
}

void speratePictures::dataAnalyseSlot()
{
    //ѡ���¼��ǩ��txt�ļ� ������ labelImg �еı�ǩ�����ļ� ��ǩ֮�任�зָ�
    static QString labelPathDir = qApp->applicationDirPath();
    QString labelPath = QFileDialog::getOpenFileName(this, "�򿪱�ǩ�����ļ�", labelPathDir, "*.txt");
    if (labelPath.isEmpty())
    {
        log("�˳�");
        return;
    }
    /*****************��ȡҪ�����ı�ǩ �Զ�ȡtxt�ļ�����ʽ��ȡ*********************/
    labelPathDir = QFileInfo(labelPath).absolutePath();
    //��ȡ��ǩ
    QStringList labelMarks;                 //��ǩ����
    QFile labelFile(labelPath);
    if (labelFile.open(QIODevice::ReadOnly))
    {
        QTextStream streamRead(&labelFile);
        streamRead.setCodec("utf-8");
        while (!streamRead.atEnd())
        {
            labelMarks.push_back(streamRead.readLine());
        }
    }
    else
    {
        log("�ļ���ʧ�ܣ�");
        return;
    }
    labelFile.close();
    qDebug() << labelMarks;


    /*******************************��ȡ��������***************************************/
    QString dataDir = QFileDialog::getExistingDirectory(this,"ѡ������·����", labelPathDir);
    if (dataDir.isEmpty())
    {
        log("ȡ��");
        return;
    }

    QStringList fileDatas;
    getDirFiles(dataDir, fileDatas, true, QStringList() << ".xml");

    qDebug() << QString("files count:%01").arg(fileDatas.count());
    
	/*******************************��ʼ��������***************************************/
    //�������� ������������   QPair<int ,int>   ͳ������ָ��  ǰ���Ǹ���ͼƬ��Ŀ  �����Ǹ��� ��ǩ��Ŀ
    QMap<QString, QPair<int ,int>> dataCounts;
    int countTotal = fileDatas.count();
    int iterCount = 0;
    creatProgress();
    for (auto iterFile: fileDatas)
    {
        /***********�����������е��ļ�************/
        setProgressValue(iterCount++, countTotal);
        //��ȡ�ļ�����
			//��ȡ��ǩ
        QString fileContext;
		QFile labelFileIter(iterFile);
		if (labelFileIter.open(QIODevice::ReadOnly))
		{
			QTextStream streamRead(&labelFileIter);
            streamRead.setCodec("utf-8");
            fileContext = streamRead.readAll();

		}
		else
		{
			log("�ļ���ʧ�ܣ�");
			return;
		}
        labelFileIter.close();


        /***********************������ǩ*********************/
        for (auto iterMark: labelMarks)
        {
            //����ͼƬ����
            QRegExp flagIter = QRegExp(QString("<name>%01</name>").arg(iterMark),Qt::CaseInsensitive);
            int pos = flagIter.indexIn(fileContext);
            if (pos != -1)
            {
				//��ǰ�ļ��а����ñ�ǩ
				dataCounts[iterMark].first++;
            }

            while (pos != -1)
            {
                //�м�����ǩ
                int lengthMatch = flagIter.matchedLength();
                dataCounts[iterMark].second ++;
                pos = flagIter.indexIn(fileContext, pos + lengthMatch);
                int ccc = 0;
            }
            int ccc = 0;
            //��������Ŀ



        }



    }

    closeProgress();
    /*************************��ӡ���*******************************/
    //int ccc = 0;
    QString resultFile = QFileDialog::getSaveFileName(this, "����", labelPathDir, "*.txt");
    if (resultFile.isEmpty())
    {
        log("ȡ��");
        return;
    }
    QFile fileResult(resultFile);
    if (!fileResult.open(QIODevice::WriteOnly))
    {
        log("�����ļ�ʧ�ܣ�");
        return;
    }
    else
    {
        QTextStream streamResult(&fileResult);
        streamResult << (QStringList() << QString("%01").arg("��ǩ����", 20) << QString("%01").arg("ͼƬ����", 20) << QString("%01").arg("��ǩ����", 20)).join("        ") << endl;
        for (auto iterData = dataCounts.constBegin(); iterData != dataCounts.constEnd(); ++iterData)
        {
            QString key = iterData.key();                           //��ǩ����
            QString imgCount =QString::number(iterData.value().first);              //���ͼƬ����
            QString labelCount = QString::number(iterData.value().second);           //��ر�ǩ����
            streamResult << (QStringList() << QString("%01").arg(key, 20) << QString("%01").arg(imgCount, 20) << QString("%01").arg(labelCount, 20)).join("        ") << endl;

        }
    }

    log("���ݷ���ͳ����ɣ�");
}

void speratePictures::sp1wslot()
{
    //

	QString dirDataPath = QFileDialog::getExistingDirectory(this, "ѡ������Ŀ¼��", "");
	if (dirDataPath.isEmpty())
	{
		log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}

	QString dirPath = QFileDialog::getExistingDirectory(this, "ѡ�񱣴�Ŀ¼��", dirDataPath);
	if (dirPath.isEmpty())
	{
		log("ѡ��·��Ϊ�� ȡ��������");
		return;
	}
	QDir dir(dirPath);
	if (!dir.exists())
	{
		dir.mkpath(dirPath);
	}
    QStringList fileNames;
    getDirFiles(dirDataPath, fileNames, false);
//#pragma omp parallel for
    for (auto iter : fileNames)
    {
		cv::Mat img;
		cv::Rect m_select;

        //QImage image;
        //QImageReader reader;
        //reader.setFileName(iter);
        //image = reader.read();


        try
        {
            img = imread(iter.toLocal8Bit().toStdString());
        }
        catch (const std::exception&)
        {

        }

        //int width = image.width();
        //int height = image.height();
        int width = img.cols;
        int height = img.rows;

        if (0 == width || 0 == height)
        {
            ui.statusBar->showMessage("��ȡ�ļ�ʧ�ܣ�");
            continue;
        }

        if (width<10000||height<10000)
        {
			ui.statusBar->showMessage("��ȡ�ļ�ʧ�ܣ�");
			continue;;
        }

        int i = (width - 10000) / 2;
        int j = (height - 10000) / 2;

        m_select = Rect(i, j, 10000, 10000);
        Mat ROI = img(m_select);
        QString newPath = dirPath+"/"+QFileInfo(iter).fileName();
        newPath = dirPath + "/" + QFileInfo(newPath).fileName();
        //bool isSave = curMap.save(newPath);
        imwrite(newPath.toLocal8Bit().toStdString(), ROI);
        //image = image.copy(i, j, 10000, 10000);
        //if (image.save(newPath,0,100))
        //{
        //    int ccc = 0;
        //}
        //else
        //{
        //    int ccc = 0;
        //}
        //break;
    }
    log("1w*1w success!");

}

void speratePictures::initApp()
{
    m_scale = 1.0;
    bool isConnect = false;
    m_scene=new QGraphicsScene(this);

	m_item = new QGraphicsPixmapItem;//�½�һ��item
	m_item->setPos(0, 0);//����item����Ϊ��0,0��
	m_scene->addItem(m_item);//��item��ӽ�����
    //QRect rect = ui.graphicsView->geometry();
    //m_scene->setSceneRect(ui.graphicsView->geometry());
    ui.graphicsView->setScene(m_scene);
    //ui.graphicsView->setAlignment(Qt::AlignHCenter);
    //initConnect
    isConnect = connect(ui.actionopen, SIGNAL(triggered()), this, SLOT(openSlot()));
    isConnect = connect(ui.actionsave, SIGNAL(triggered()), this, SLOT(saveSLOT()));
	isConnect = connect(ui.actionrotate, SIGNAL(triggered()), this, SLOT(rotateSlot()));
	isConnect = connect(ui.actiontif2png, SIGNAL(triggered()), this, SLOT(tif2pngSlot()));
    isConnect = connect(ui.actionRandom, SIGNAL(triggered()), this, SLOT(randomSpliteSlot()));
    isConnect = connect(ui.actionselDir, SIGNAL(triggered()), this, SLOT(selectDirSlot()));
    isConnect = connect(ui.actionselDirRandom, SIGNAL(triggered()), this, SLOT(selectDirRandomSlot()));
    isConnect = connect(ui.actionreName, SIGNAL(triggered()), this, SLOT(renameSortSlot()));
    isConnect = connect(ui.actionUnifiedPath, SIGNAL(triggered()), this, SLOT(unifimPathSlot()));
	isConnect = connect(ui.actionactionRandomSel, SIGNAL(triggered()), this, SLOT(randomSelSlot()));
    isConnect = connect(ui.actionactionRotateSel, SIGNAL(triggered()), this, SLOT(randomSelRotateSlot()));
    isConnect=connect(ui.actionDataAnalyse, SIGNAL(triggered()), this, SLOT(dataAnalyseSlot()));
    isConnect = connect(ui.actionsp1w, SIGNAL(triggered()), this, SLOT(sp1wslot()));
    int ccc = 0;
}


void speratePictures::creatProgress()
{
    if (!m_progress)
    {
		m_progress = new QProgressBar(this);
        m_progress->setValue(0);
		ui.statusBar->addPermanentWidget(m_progress, 1);
		m_progress->setVisible(false);
    }
    m_progress->setVisible(true);
}

void speratePictures::setProgressValue(int i,int count)
{
    m_progress->setRange(0, count);
    m_progress->setValue(i);
    qApp->processEvents();
}

void speratePictures::closeProgress()
{
    m_progress->setVisible(false);
}

void speratePictures::zoomIn()
{
    m_scale -= 0.05;
    if (m_scale<=0)
    {
        m_scale = 0.2;
    }
    m_item->setScale(m_scale);
    //m_item->setPos()
}

void speratePictures::zoomOut()
{
	m_scale += 0.05;
    if (m_scale>10)
    {
        m_scale = 10;
    }
	m_item->setScale(m_scale);
}

// �����¼�
void speratePictures::wheelEvent(QWheelEvent* event)
{
    QPoint pos;
    QPoint pos1;
    QPoint pos2;
    pos1 = mapToGlobal(QPoint(0, 0));
    pos2 = event->globalPos();
    pos = pos2 - pos1;

    //�ж����λ���Ƿ���ͼ����ʾ����
    if (pos.x() > m_item->x() && pos.x() < m_item->x() + m_item->pixmap().width()
        && pos.y() > m_item->y() && pos.y() < m_item->y() + m_item->pixmap().height())
    {
            // ������Զ��ʹ����ʱ���зŴ󣬵�������ʹ���߷�����תʱ������С
            if (event->delta() > 0)
            {
                    zoomIn();
            }
            else
            {
                    zoomOut();
            }
    }

}




void speratePictures::paintEvent(QPaintEvent* event)
{
 //   if (!m_item||!m_scene)
 //   {
 //       return;
 //   }
 //   QPoint pos= QCursor::pos();
 //   qDebug() << QString("������꣺ X:%01 Y:%02").arg(pos.x()).arg(pos.y());
 //   QPointF scencePointf= ui.graphicsView->mapToScene(pos);
 //   qDebug() << QString("�������꣺ X:%01 Y:%02").arg(scencePointf.x()).arg(scencePointf.y());
	//QPointF scencePointS = ui.graphicsView->mapFromScene(pos);
	//qDebug() << QString("����to��ͼ���꣺ X:%01 Y:%02").arg(scencePointS.x()).arg(scencePointS.y());

	//QPointF scencePointSE = m_item->mapFromScene(pos);
	//qDebug() << QString("����toͼԪ���꣺ X:%01 Y:%02").arg(scencePointSE.x()).arg(scencePointSE.y());

	//QPointF scencePointSS = m_item->mapFromScene(pos);
	//qDebug() << QString("ͼԪto�������꣺ X:%01 Y:%02").arg(scencePointSS.x()).arg(scencePointSS.y());

	////QGraphicsView::mapToScene() - ��ͼ->����

	////	QGraphicsView::mapFromScene() - ����->��ͼ

	////	QGraphicsItem::mapFromScene() - ����->ͼԪ

	////	QGraphicsItem::mapToScene() - ͼԪ->����

	////	QGraphicsItem::mapToParent() - ��ͼԪ->��ͼԪ

	////	QGraphicsItem::mapFromParent() - ��ͼԪ->��ͼԪ

	////	QGraphicsItem::mapToItem() - ��ͼԪ->����ͼԪ

	////	QGraphicsItem::mapFromItem() - ����ͼԪ->��ͼԪ


}

void speratePictures::mouseMoveEvent(QMouseEvent* event)
{
    return;
    QPoint pos = QCursor::pos();
    QPointF scencePointSE = m_item->mapFromScene(pos);
    QPointF scencePointS = ui.graphicsView->mapFromScene(pos);
    if (!m_item)
    {
        return;
    }
    m_item->setPos(scencePointS);
}

void speratePictures::mousePressEvent(QMouseEvent* event)
{
	if (!m_item || !m_scene)
	{
		return;
	}
	QPoint pos = QCursor::pos();
	qDebug() << QString("������꣺ X:%01 Y:%02").arg(pos.x()).arg(pos.y());
	QPointF scencePointf = ui.graphicsView->mapToScene(pos);
	qDebug() << QString("�������꣺ X:%01 Y:%02").arg(scencePointf.x()).arg(scencePointf.y());
	QPointF scencePointS = ui.graphicsView->mapFromScene(pos);
	qDebug() << QString("����to��ͼ���꣺ X:%01 Y:%02").arg(scencePointS.x()).arg(scencePointS.y());

	QPointF scencePointSE = m_item->mapFromScene(pos);
	qDebug() << QString("����toͼԪ���꣺ X:%01 Y:%02").arg(scencePointSE.x()).arg(scencePointSE.y());

	QPointF scencePointSS = m_item->mapFromScene(pos);
	qDebug() << QString("ͼԪto�������꣺ X:%01 Y:%02").arg(scencePointSS.x()).arg(scencePointSS.y());


	QPointF scencePointSP = m_item->mapFromParent(pos);
	qDebug() << QString("ͼԪ���꣺ X:%01 Y:%02").arg(scencePointSP.x()).arg(scencePointSP.y());

}

void speratePictures::savePicture(const QString& path, Qt::HANDLE curHandle, const QString& dir)
{
	cv::Mat img;
	cv::Rect m_select;
	//QPixmap pix(path);
    img = imread(path.toLocal8Bit().toStdString());
    //if (pix.isNull())
    //{
    //    ui.statusBar->showMessage(QString("����ʧ�ܣ�%01").arg(path));
    //    return;
    //}
    //m_item->setPixmap(pix.scaledToWidth(height()));
	//int width = pix.width();
	//int height = pix.height();
	int width = img.cols;
	int height = img.rows;
	int count = 1;
	int allCount = 0;
	for (int i = 0; i < width; i += 800)
	{
		for (int j = 0; j < height; j += 800)
		{
			allCount++;
		}
	}

    QString baseDirName = QFileInfo(path).completeBaseName();
    QDir dirObj(QFileInfo(path).absolutePath());
    if (!dirObj.exists(baseDirName))
    {
        bool isMK= dirObj.mkdir(baseDirName);
        int ccc = 0;
    }
    if (curHandle == QThread::currentThreadId())
    {
        creatProgress();
    }
    //Qt::HANDLE curThread = QThread::currentThreadId();
#pragma omp parallel for
	for (int i = 0; i < width; i += 800)
	{
		for (int j = 0; j < height; j += 800)
		{
            int m = i, n = j;
			if (curHandle == QThread::currentThreadId())
			{
				qDebug() << QThread::currentThreadId();
				setProgressValue(count, allCount);
			}
			if (i + 800 >= width)
			{
				m = width - 800;
			}
			if (j + 800 >= height)
			{
				n = height - 800;
			}
			//QPixmap curMap = pix.copy(m, n, 800, 800);
			m_select = Rect(m, n, 800, 800);
            Mat ROI = img(m_select);
			QString newPath = path.left(path.indexOf(".")) + QString("_%01.png").arg(count++);
			newPath = QString("%02/./%01").arg(baseDirName).arg(QFileInfo(path).absolutePath()) + "/" + QFileInfo(newPath).fileName();
            imwrite(newPath.toLocal8Bit().toStdString(), ROI);
            //bool isSave = curMap.save(newPath);
            // 
			//QImage image2=curMap.toImage();
			//isSave= image2.save(newPath);
			//int ccc = 0;
		}
	}
    if (curHandle == QThread::currentThreadId())
    {
        closeProgress();
    }
	ui.statusBar->showMessage(QString("������ϣ�%01").arg(QString("./%01").arg(baseDirName)));
}

void speratePictures::saveRandomPicture(const QString& path, Qt::HANDLE curHandle, int countRandom, const QString& dir /*= ""*/)
{
	cv::Mat img;
	cv::Rect m_select;
	//QPixmap pix(path);
	img = imread(path.toLocal8Bit().toStdString());
	//if (pix.isNull())
	//{
	//	ui.statusBar->showMessage(QString("����ʧ�ܣ�%01").arg(path));
	//	return;
	//}
	//m_item->setPixmap(pix.scaledToWidth(height()));
	//int width = pix.width();
	//int height = pix.height();
	int width = img.cols;
	int height = img.rows;
	int count = 1;
	int allCount = countRandom;
	QString baseDirName = QFileInfo(path).completeBaseName();
	QDir dirObj(QFileInfo(path).absolutePath());
	if (!dirObj.exists(baseDirName))
	{
		bool isMK = dirObj.mkdir(baseDirName);
		int ccc = 0;
	}
	if (curHandle == QThread::currentThreadId())
	{
		creatProgress();
	}
	//Qt::HANDLE curThread = QThread::currentThreadId();
#pragma omp parallel for
	for (int h = 0; h < countRandom; h++)
	{
		int i = 0;
		int j = 0;
		i = qrand() % width;
		j = qrand() % height;
		if (curHandle == QThread::currentThreadId())
		{
			qDebug() << QThread::currentThreadId();
			setProgressValue(count, allCount);
		}
		if (i + 800 >= width)
		{
			i = width - 800;
		}
		if (j + 800 >= height)
		{
			j = height - 800;
		}
		//QPixmap curMap = pix.copy(m, n, 800, 800);
		m_select = Rect(i, j, 800, 800);
		Mat ROI = img(m_select);
		QString newPath = path.left(path.indexOf(".")) + QString("_%01.png").arg(count++);
		newPath = QString("%02/./%01").arg(baseDirName).arg(QFileInfo(path).absolutePath()) + "/" + QFileInfo(newPath).fileName();
		imwrite(newPath.toLocal8Bit().toStdString(), ROI);
		//QImage image2=curMap.toImage();
		//isSave= image2.save(newPath);
		//int ccc = 0;

	}


	if (curHandle == QThread::currentThreadId())
	{
		closeProgress();
	}
	ui.statusBar->showMessage(QString("������ϣ�%01").arg(QString("./%01").arg(baseDirName)));
}


void speratePictures::getDirFiles(const QString& dir, QStringList& fileNames,bool isFindF, const QStringList& fileFilters)
{
    QDir curDir(dir);
    if (!curDir.exists())
    {
        log("dir not exists!");
        return;
    }

    QFileInfoList infos = curDir.entryInfoList();
    for (int i = 0; i < infos.count(); ++i)
    {
        QFileInfo iterInfo = infos.at(i);
        QString iterName = iterInfo.fileName();
        if (iterInfo.isFile())
        {
            if (!fileFilters.isEmpty())
            {
                bool isCon = true;
                for (auto iterFilter: fileFilters)
                {
					if (iterName.contains(iterFilter))
					{
                        isCon = false;
					}
                }
                if (isCon)
                {
                    continue;;
                }
            }
            fileNames.push_back(iterInfo.absoluteFilePath());
        }
        if (iterInfo.isDir())
        {
            if (!isFindF)
            {
                continue;
            }
            QString folderName = iterInfo.fileName();
            if (0==folderName.compare(".")||0==folderName.compare(".."))
            {
                continue;
            }
            getDirFiles(iterInfo.absoluteFilePath(), fileNames);
        }


    }

}

void speratePictures::renameFile(const QString&path, const QString& oldName, const QString& newName)
{
	QString xmlPath = getXmlPath(path);
	QFile fileOpen(xmlPath);
    QString text = "";
    if (fileOpen.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&fileOpen);
        stream.setCodec("UTF-8");
        text = fileOpen.readAll();
        QRegExp rx("<path>(.*)</path>");
        int pos = rx.indexIn(text);
        if (pos!=-1)
        {
            QStringList capPaths = rx.capturedTexts();
            QString path = rx.cap(1);
            text.replace(path, newName);
            int ccc = 0;
        }
        rx.setPattern("<filename>(.*)</filename>");
        pos = rx.indexIn(text);
        if (pos != -1)
        {
            QStringList capPaths = rx.capturedTexts();
			QString oldName = rx.cap(1);
            QString newModName = QFileInfo(newName).fileName();
			text.replace(oldName, newModName);
            int ccc = 0;
        }

        stream << text;
        stream.flush();
    }
    else
    {
        ui.statusBar->showMessage(QString("%01 open error��").arg(path));
    }
    fileOpen.close();

    if (fileOpen.open(QIODevice::Truncate|QIODevice::WriteOnly))
    {
        QTextStream stream(&fileOpen);
        stream.setCodec("UTF-8");
		stream << text;
		stream.flush();
    }
    fileOpen.close();
    QString newFileName = QFileInfo(path).absolutePath() + "/" + QFileInfo(newName).baseName()+"." + QFileInfo(path).suffix();
    bool isRename= QFile::rename(xmlPath, newFileName);
    if (!isRename)
    {
        int ccc = 0;
    }
}

QString speratePictures::getXmlPath(const QString& picPath)
{
	QString tempPath = picPath;
	return tempPath.replace(".png", ".xml");
}

void speratePictures::uniformDirPath(const QString& path, const QString& newDir)
{
    QDir dirNew(newDir);
    QString dirName = dirNew.dirName();


    QString xmlPath = getXmlPath(path);
    QFile fileOpen(xmlPath);
    QString text;
    if (fileOpen.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&fileOpen);
        stream.setCodec("UTF-8");
        text = fileOpen.readAll();

        QRegExp rxFolder("<folder>(.*)</folder>");
        QRegExp rxPath("<path>(.*)</path>");
        //int index = text.indexOf(rxPath);

        //int posPath = text.indexOf("<path>");
        //int posEndPath = text.indexOf("</", posPath+6);
        //QString text2 = text.mid(posPath + 6, posEndPath);
        //QString fileName = QFileInfo(text2).fileName();
        //QString newFileName = newDir + "/" + fileName;
        //int length = rxPath.matchedLength();
        //text.replace(text2, newFileName);
        int posPath = 0;
		posPath = rxPath.indexIn(text);
		while (-1 < posPath)
		{
			QString text1 = rxPath.cap();
			QString text2 = rxPath.cap(1);
			QString fileName = QFileInfo(text2).fileName();
			QString newFileName = newDir + "/" + fileName;
			int length = rxPath.matchedLength();
			text.replace(text2, newFileName);
			posPath = rxPath.indexIn(text, posPath + length);
		}


        int offset = 0;
        int pos = 0;
        pos = rxFolder.indexIn(text);
        while (-1 < pos)
        {
            QString text1 = rxFolder.cap();
            QString text2 = rxFolder.cap(1);
            int length = rxFolder.matchedLength();
            text.replace(text2, dirName);
            //types.insert(text2);
            //types+=rx.capturedTexts().toSet();
            int ccc = 0;

            pos = rxFolder.indexIn(text, pos + length);
        }

        //text.replace(oldName, newName);
        stream << text;
    }
    else
    {
        ui.statusBar->showMessage(QString("%01 open error��").arg(path));
    }
    fileOpen.close();
	if (fileOpen.open(QIODevice::Truncate | QIODevice::WriteOnly))
	{
		QTextStream stream(&fileOpen);
        stream.setCodec("UTF-8");
		stream << text;
		stream.flush();
	}
	fileOpen.close();
}


////�½�һ��Qt Widgets Application
////��mainwindow.ui�����һ��QGraphicsView
////�����Ҫ��ͷ�ļ�����MainWindow���캯����������º���
////д��ui->setupUi(this);��
////
//QGraphicsScene* scene;
//QGraphicsPixmapItem* item;
//QPixmap* painterPix;
//
//scene = new QGraphicsScene;//�½�һ������
//ui->graphicsView->setScene(scene);//��������ӽ���ͼ
//scene->addRect(0, 0, 500, 500);//���ó���λ�ã�0,0������С500x500
//
////����������ͬ�ķ��飬��ӡ��ÿ����������꣬�������ĳ������꣬������������
////��һ�����飬��ɫ
// //QGraphicsPixmapItem* item;
//item = new QGraphicsPixmapItem;//�½�һ��item
//painterPix = new QPixmap(50, 50);//�½�һ��ͼ
//painterPix = fill(Qt::black);//��ͼ����Ϊ��ɫ
//item->setPixmap(*painterPix);//��ͼ��ӽ�item
//scene->addItem(item);//��item��ӽ�����
//item->setPos(0, 0);//����item����Ϊ��0,0��
//qDebug() << "black pos:" << item->pos() << "black pixPos:" << item->mapToParent(item->pixmap().rect().center()) << "pix cenPos:" << item->pixmap().rect().center();
//
////�ڶ������飬��ɫ����ת90��
//item = new QGraphicsPixmapItem;
//painterPix = new QPixmap(50, 50);
//painterPix = fill(Qt::blue);
//item->setPixmap(*painterPix);
//scene->addItem(item);
//item->setPos(50, 50);
//item->setRotation(90);//��ת90��
//qDebug() << "black pos:" << item->pos() << "black pixPos:" << item->mapToParent(item->pixmap().rect().center()) << "pix cenPos:" << item->pixmap().rect().center();
//
////���������飬��ɫ���Ŵ�Ϊԭ����2��
//item = new QGraphicsPixmapItem;
//painterPix = new QPixmap(50, 50);
//painterPix = fill(Qt::red);
//item->setPixmap(*painterPix);
//scene->addItem(item);
//item->setPos(100, 100);
//item->setRotation(2);//�Ŵ�����
//qDebug() << "black pos:" << item->pos() << "black pixPos:" << item->mapToParent(item->pixmap().rect().center()) << "pix cenPos:" << item->pixmap().rect().center();




//char* filename = "F:/0.tif";
///*read + write*/
//QImage image;
//QImageReader qReader(filename);
//image = qReader.read();
//QImageWriter writer("F:/A.tiff", "tiff");
//writer.write(image);
//
///*load + write*/
//QImage image1;
//image1.load(filename);
//QImageWriter writer1("F:/B.tiff", "tiff");
//writer1.write(image1);
//
///*load + save*/
//QImage image2;
//image2.load(filename);
//image2.save("F:/C.tiff", "tiff");
//
///*write*/
//QImage image3 = QImage(filename);
//QImageWriter writer3("F:/D.tiff", "tiff");
//writer3.write(image3);

//QString filename = "F:/4.tiff";
//QPixmap qimage(filename);
//qimage = qimage.scaled(800, 800);
//ui->labelPic->setPixmap(qimage);
//ui->labelPic->show();