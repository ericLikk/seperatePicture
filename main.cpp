#include "speratePictures.h"
#include <QtWidgets/QApplication>
#include <QFileDialog>

//int main()
//{
//	img = imread("F://Visual Studio 2015//ROItest01//ROItest01//01.jpg");
//
//	imshow("ԭͼ", img);
//
//	m_select = Rect(0, 0, 111, 217);
//
//	Mat ROI = img(m_select);
//
//	imshow("����ͼ", ROI);
//
//	waitKey(0);
//
//	return 0;
//}




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/speratePictures/res/back.jpg"));
    a.setApplicationName("�з�ͼƬ");

 //   QString path = QFileDialog::getOpenFileName(nullptr, "", "", "*.*");
 //   if (path.isEmpty())
 //   {
 //       return 0;
 //   }
	//img = imread(path.toLocal8Bit().toStdString());

	//imshow("ԭͼ", img);

	//m_select = Rect(0, 0, 111, 217);
     //   imwrite("a.png", ROI);
	//Mat ROI = img(m_select);
 //   
	//imshow("����ͼ", ROI);



    speratePictures w;
    w.showMaximized();
    return a.exec();
}
