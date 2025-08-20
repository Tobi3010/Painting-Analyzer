#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include <QLineEdit>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>
#include <QFrame>

// Only include what you need from OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "grayscale.hpp"
#include "perspective.hpp"
#include "ClickLabel.hpp"





// QPushButton inherented class, with added stylesheet
class Button : public QPushButton
{
public:
    Button(const QString &text, QWidget *parent = nullptr): QPushButton(text, parent){
        setStyleSheet(
            "color: #ffffff;"
            "background-color: #050303ff;"
            "selection-color: #ffffff;"
            "selection-background-color: green;"
        );
    }
};

class PerspectiveLayout : public QWidget
{
    
public:
    PerspectiveLayout(
            int imgWidth,
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
        Button *test = new Button("test");
        int w = 0.85 * imgWidth;

        layout->addStretch();
        layout->addWidget(test);

        connect(test, &QPushButton::clicked, this, &PerspectiveLayout::draw_lines);


    }

private:
    cv::Mat* imgOriginal;
    cv::Mat* imgModified;
    std::function<void(const cv::Mat&)> imgSetCallback;

    void draw_lines() {
        imgOriginal->copyTo(*imgModified); // Reset imgModified first
        perspective_lines(*imgModified);
        imgSetCallback(*imgModified);
    }
};


// Layout widget with elements for grayscaling editing
class GrayscaleLayout : public QWidget 
{
public:
    GrayscaleLayout(
            int imgWidth,
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
        grayscaleCustom = new QLineEdit("Number of shades");

        int w = 0.85 * imgWidth;
        QTextStream out(stdout);
        out << w << "\n";

        cv::Mat gradientImg(50, w, CV_8UC1);
        for (int x = 0; x < gradientImg.cols; ++x) {
            gradientImg.col(x).setTo(x);
        }
        cv::Mat gradientColor;
        cv::cvtColor(gradientImg, gradientColor, cv::COLOR_GRAY2BGR);
        QImage gradientQImg(gradientColor.data, gradientColor.cols, gradientColor.rows, gradientColor.step, QImage::Format_RGB888);
        QLabel *gradientLabel = new QLabel;
        gradientLabel->setPixmap(QPixmap::fromImage(gradientQImg));
        gradientLabel->setFrameShape(QFrame::Panel);
        gradientLabel->setLineWidth(2);

        // Black & white box
        cv::Mat binaryImg(50, w, CV_8UC1, cv::Scalar(0));
        binaryImg.colRange(w/2, w).setTo(255);  // Right half white
        cv::Mat binaryColor;
        cv::cvtColor(binaryImg, binaryColor, cv::COLOR_GRAY2BGR);
        QImage binaryQImg(binaryColor.data, binaryColor.cols, binaryColor.rows, binaryColor.step, QImage::Format_RGB888);
        QLabel *binaryLabel = new QLabel;
        binaryLabel->setPixmap(QPixmap::fromImage(binaryQImg));
        binaryLabel->setFrameShape(QFrame::Panel);
        binaryLabel->setLineWidth(2);

        // Black & white box
        cv::Mat customImg(50, w, CV_8UC1, cv::Scalar(17));
        customImg.colRange(w/4, (w/4)*2).setTo(51);  
        customImg.colRange((w/4)*2, (w/4)*3).setTo(119);  
        customImg.colRange((w/4)*3, w).setTo(153);  

        
        cv::Mat customColor;
        cv::cvtColor(customImg, customColor, cv::COLOR_GRAY2BGR);
        QImage customQImg(customColor.data, customColor.cols, customColor.rows, customColor.step, QImage::Format_RGB888);
        QLabel *customLabel = new QLabel;
        customLabel->setPixmap(QPixmap::fromImage(customQImg));
        customLabel->setFrameShape(QFrame::Panel);
        customLabel->setLineWidth(2);

        // te
        QLabel *txt1 = new QLabel;
        txt1->setMaximumWidth(w);
        txt1->setWordWrap(true);
        txt1->setText("Convert to full range grayscale, all 255 shades between black and white.");

        QLabel *txt2 = new QLabel;
        txt2->setMaximumWidth(w);
        txt2->setWordWrap(true);
        txt2->setText("Convert to binary grayscale, all colors will be turned either black or white.");

        QLabel *txt3 = new QLabel;
        txt3->setMaximumWidth(w);
        txt3->setWordWrap(true);
        txt3->setText("Type number of shades desired, and image will be converted to limited grayscale. Number must be between 2 and 255.");

    

        layout->addStretch();
        layout->addWidget(gradientLabel);   
        layout->addWidget(grayscale);
        layout->addWidget(txt1);  
        layout->addStretch();
        layout->addWidget(binaryLabel);         
        layout->addWidget(grayscaleBinary);
        layout->addWidget(txt2);  
        layout->addStretch();
        layout->addWidget(customLabel);
        layout->addWidget(grayscaleCustom);
        layout->addWidget(txt3);  
      

        connect(grayscale, &QPushButton::clicked, this, &GrayscaleLayout::fullrange_grayscale);
        connect(grayscaleBinary, &QPushButton::clicked, this, &GrayscaleLayout::binary_grayscale);
        connect(grayscaleCustom, &QLineEdit::editingFinished, this, &GrayscaleLayout::custom_grayscale);
    }

private:
    cv::Mat* imgOriginal;
    cv::Mat* imgModified;
    std::function<void(const cv::Mat&)> imgSetCallback;
    QLineEdit* grayscaleCustom;  

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

    void custom_grayscale() {
        bool ok;
        int num = grayscaleCustom->text().toInt(&ok); 
        if (!ok || num < 2 || num > 255) {
            QMessageBox::warning(this, "Invalid Input", "Enter number between 2 and 255.");
            return;
        }
      
        imgOriginal->copyTo(*imgModified); // Reset imgModified first
        grayscale_splitrange2(*imgModified, num, 42);
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
        imgLabel = new ClickLabel;
        static_cast<ClickLabel*>(imgLabel)->setImage(&imgOriginal);
        imgTextLabel = new QLabel;
        imgSelect("../pics/perspective-test.png");  

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
        PerspectiveLayout *grayLayout = new PerspectiveLayout(
            btn_widget->width(),
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
