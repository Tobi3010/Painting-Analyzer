#include "ClickLabel.hpp"
#include <QMouseEvent>
#include <iostream>

ClickLabel::ClickLabel(QWidget *parent)
    : QLabel(parent), imgOriginalPtr(nullptr) {}

void ClickLabel::setImage(cv::Mat *imgPtr) {
    imgOriginalPtr = imgPtr;
}

void ClickLabel::mousePressEvent(QMouseEvent *event) {
    if (!imgOriginalPtr || imgOriginalPtr->empty()) return;

    QPoint widgetPos = event->pos();
    QSize widgetSize = size();

    int imgW = imgOriginalPtr->cols;
    int imgH = imgOriginalPtr->rows;

    float scaleX = static_cast<float>(imgW) / widgetSize.width();
    float scaleY = static_cast<float>(imgH) / widgetSize.height();

    int imgX = static_cast<int>(widgetPos.x() * scaleX);
    int imgY = static_cast<int>(widgetPos.y() * scaleY);

    if (imgX < 0 || imgX >= imgW || imgY < 0 || imgY >= imgH) return;

    cv::Vec3b color = imgOriginalPtr->at<cv::Vec3b>(imgY, imgX);
    std::cout << "Clicked at (" << imgX << ", " << imgY << ") "
              << "Color: B=" << (int)color[0]
              << " G=" << (int)color[1]
              << " R=" << (int)color[2] << "\n";
}
