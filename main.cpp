#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>


// Window class 
class MainWindow : public QMainWindow {
public:
    MainWindow() {
        initUI();
    }

private:
    
    QLabel *imageLabel; // Make it a class member

    void initUI() {
        setWindowTitle("My Qt App");

        // Buttons
        QPushButton *button1 = new QPushButton("One");
        QPushButton *button2 = new QPushButton("Two");

        connect(button1, &QPushButton::clicked, this, &MainWindow::imgUpdate);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(button1);
        buttonLayout->addWidget(button2);

        // Image label
        imageLabel = new QLabel;
        imgSet("../pics/jinx.jpg");

        imageLabel->setAlignment(Qt::AlignCenter);

        // Layouts
        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(central);
        mainLayout->addWidget(imageLabel);
        mainLayout->addLayout(buttonLayout);

        setCentralWidget(central);
    }

    void imgSet(const std::string &path) {
        cv::Mat img = cv::imread(path);
        if (img.empty()) return;
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
        QImage qimg(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
        imageLabel->setPixmap(QPixmap::fromImage(qimg));
    }

    void imgUpdate() {
        imgSet("../pics/vi.jpg");
    }
};


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);


   
    MainWindow window;
    window.show();

    return app.exec();
}
