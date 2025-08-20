#ifndef CLICKLABEL_HPP
#define CLICKLABEL_HPP

#include <QLabel>
#include <opencv2/opencv.hpp>

class ClickLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickLabel(QWidget *parent = nullptr);

    // Set the cv::Mat image pointer so we can read pixel values
    void setImage(cv::Mat *imgPtr);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    cv::Mat *imgOriginalPtr;
};

#endif // CLICKLABEL_HPP

