#include "algorithm.h"

// cv::Mat转换成QImage
// QImage cvMat2QImage(const Mat& mat)
// {
//     const uchar *pSrc = (const uchar*)mat.data;
//     QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
//     return image.rgbSwapped();
// }

// QImage转换成cv::Mat
// Mat QImage2cvMat(QImage image)
// {
//     Mat mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
//     cv::cvtColor(mat, mat, CV_BGRA2BGR);
//     return mat;
// }


//证件照智能处理算法
QImage MakeIDPhoto(QImage img)
{
    return img;
}
