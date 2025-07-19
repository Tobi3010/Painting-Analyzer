#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QMenu>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <QDebug>
#include <QTextStream>
#include "grayscale.hpp"
#include <QFileDialog>






// QPushButton inherented class, with added stylesheet
class Button : public QPushButton
{
public:
    Button(const QString &text, QWidget *parent = nullptr): QPushButton(text, parent){
        setStyleSheet(
            "color: #ffffff;"
            "background-color: #000000;"
            "selection-color: #ffffff;"
            "selection-background-color: green;"
        );
    }
};

// Layout widget with elements for grayscaling editing
class GrayscaleLayout : public QWidget 
{
public:
    GrayscaleLayout(
            cv::Mat* imgOriginal,               
            cv::Mat* imgModified, 
            std::function<void(const cv::Mat&) // Callback wrapper for imgSet from MainWindow
            > 
            imgSetCallback, 
            QWidget *parent = nullptr) 
            : 
            QWidget(parent), 
            imgOriginal(imgOriginal), 
            imgModified(imgModified), 
            imgSetCallback(imgSetCallback) 
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        Button *grayscale = new Button("Full Range Grayscale");
        Button *grayscaleBinary = new Button("Binary Grayscale(Black & White)");
        layout->addStretch();
        layout->addWidget(grayscale);
        layout->addWidget(grayscaleBinary);
        layout->addStretch();

        connect(grayscale, &QPushButton::clicked, this, &GrayscaleLayout::fullrange_grayscale);
        connect(grayscaleBinary, &QPushButton::clicked, this, &GrayscaleLayout::binary_grayscale);
    }

private:
    cv::Mat* imgOriginal;
    cv::Mat* imgModified;
    std::function<void(const cv::Mat&)> imgSetCallback;

    void fullrange_grayscale() {
        imgOriginal->copyTo(*imgModified); // Reset imgModified first
        grayscale_fullrange(*imgModified);
        cv::cvtColor(*imgModified, *imgModified, cv::COLOR_GRAY2BGR);
        imgSetCallback(*imgModified);
    }

    void binary_grayscale() {
        imgOriginal->copyTo(*imgModified); // Reset imgModified first
        grayscale_splitrange(*imgModified);
        cv::cvtColor(*imgModified, *imgModified, cv::COLOR_GRAY2BGR);
        imgSetCallback(*imgModified);
    }
};

    



// Window class
class MainWindow : public QMainWindow
{
public:
    MainWindow() {
        initUI();
    }

private:
    // Orinal and Modifed image matrixes, to swift between them quick
    cv::Mat imgOriginal; // Matrix to save original version of image
    cv::Mat imgModified; // Matrix for modified changes to image
    QLabel *imgTextLabel;
    QLabel *imgLabel;
    QWidget *btn_widget;
    QVBoxLayout *btn_layout;


    void initUI() {
        setWindowTitle("Painting Analyzer App 2.0");
    
        // Buttons ------------------------------------------------------------
        btn_widget = new QWidget();
        Button *btn_file = new Button("File");
        Button *btn_options = new Button("Options");
        Button *btn_reset = new Button("Reset");

        connect(btn_file, &QPushButton::clicked, this, &MainWindow::fileSelect);
        connect(btn_options, &QPushButton::clicked, this, &MainWindow::changeMenu);
        connect(btn_reset, &QPushButton::clicked, this, &MainWindow::reset);
    
        btn_layout = new QVBoxLayout(btn_widget);
        btn_layout->addWidget(btn_file);
        btn_layout->addWidget(btn_options);
        btn_layout->addStretch();
        btn_layout->addWidget(btn_reset);

        btn_widget->setStyleSheet(
            "color: #ffffff;"
            "background-color: #000000;"
            "selection-color: #ffffff;"
            "selection-background-color: green;"
        );
        btn_widget->setMinimumWidth(0.4*width());
        btn_widget->setMaximumWidth(0.4*width());
    
        
        // Image ---------------------------------------------------------------
        imgLabel = new QLabel;
        imgTextLabel = new QLabel;
        imgSelect("../pics/traintrack.jpg");  

        QVBoxLayout *imgLayout = new QVBoxLayout();
        imgLayout->addWidget(imgTextLabel, 1);
        imgTextLabel->setAlignment(Qt::AlignCenter);
        imgLayout->addWidget(imgLabel, 20);

        // Layouts
        QWidget *central = new QWidget;
        QHBoxLayout *mainLayout = new QHBoxLayout(central);
        mainLayout->addWidget(btn_widget,1);
        mainLayout->addLayout(imgLayout, 5);
        central->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        
        setCentralWidget(central);
    }
    
    void changeMenu() {
        GrayscaleLayout *grayLayout = new GrayscaleLayout(
            &imgOriginal,
            &imgModified,                                      
            [this](const cv::Mat& img) { this->imgSet(img); },  // Callback wrapper for imgSet
            btn_widget
        );
        btn_layout->insertWidget(2, grayLayout); // Insert widget into current layout
    }


    void fileSelect() {
        QString file1Name = 
            QFileDialog::getOpenFileName(
                this,
                tr("Open mage file"), 
                "/home/tobi/dev/painting-analyzer/pics", 
                tr("Image Files (*.jpg *.png)")
            );
        
        imgTextLabel->setText(file1Name);
        imgSelect(file1Name.toStdString());
    }

    void reset() {
        imgOriginal.copyTo(imgModified); // Reset imgModified
        imgSet(imgOriginal);
    }

    // Sets the current image displayed on screen
    void imgSet(cv::Mat img) {   
        QImage qimg(
            img.data,
            img.cols,
            img.rows,
            img.step,
            QImage::Format_BGR888);
        
        // Scale image along its bigger side, width or height, if equal always do width!
        if (qimg.height() < qimg.width()) {
            qimg = qimg.scaledToWidth(width() * 0.85, Qt::FastTransformation);
        }
        else {
            qimg = qimg.scaledToHeight(height() * 0.85, Qt::FastTransformation);
        }
        imgLabel->setPixmap(QPixmap::fromImage(qimg));
        imgLabel->setAlignment(Qt::AlignCenter);
    }

    // Select a new image to display
    void imgSelect(const std::string &path) {
        imgOriginal = cv::imread(path);
        if (imgOriginal.empty()) { return; }
        imgSet(imgOriginal);
    }
};

// for bug prints
// QTextStream out(stdout);
// out << "colors\n";

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
