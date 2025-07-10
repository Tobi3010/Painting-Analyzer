#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <QWidget>
#include <QPushButton>
#include <QLayout>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    cv::Mat img = cv::imread("../pics/jinx.jpg");
    if (img.empty()) return -1;

    // Convert to Qt format
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
    QImage qimg(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);

    QLabel label;
    label.setPixmap(QPixmap::fromImage(qimg));
    label.show();

    return app.exec();
}

