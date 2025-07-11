#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>


// Window class 
class MainWindow : public QMainWindow {
public:
    MainWindow() {
        initUI();
    }

private:
    // Orinal and Modifed image matrixes, to swift between them quick
    cv::Mat imgOriginal; // Matrix to save original version of image
    cv::Mat imgModified; // Matrix for modified changes to image
    QLabel  *imgLabel;

    void initUI() {
        setWindowTitle("Painting Analtzer App 2.0");

        // Buttons
        QPushButton *button1 = new QPushButton("One");
        QPushButton *button2 = new QPushButton("Two");

        connect(button1, &QPushButton::clicked, this, &MainWindow::grayscale);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(button1);
        buttonLayout->addWidget(button2);

        // Image
        imgLabel = new QLabel;
        imgSelect("../pics/jinx.jpg");
        imgLabel->setAlignment(Qt::AlignCenter);

        // Layouts
        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);
        mainLayout->addWidget(imgLabel);
        mainLayout->addLayout(buttonLayout);

        setCentralWidget(central);
    }

    // Sets the current image displayed on screen
    void imgSet(cv::Mat img) {
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
        QImage qimg(
            img.data, 
            img.cols, 
            img.rows, 
            img.step, 
            QImage::Format_RGB888);

        imgLabel->setPixmap(QPixmap::fromImage(qimg));
    }

    // Select a new image to display
    void imgSelect(const std::string &path) {
        imgOriginal = cv::imread(path);
        if (imgOriginal.empty()) return;
        imgSet(imgOriginal);
    }

    // Connected to button
    void imgUpdate() {
        imgSelect("../pics/vi.jpg");
    }

    void grayscale(){
        cvtColor(imgOriginal, imgModified, cv::COLOR_RGB2GRAY);
        imgSet(imgModified);
    }
};


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
