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






class Button : public QPushButton
{
public:
    Button(const QString &text, QWidget *parent = nullptr): QPushButton(text, parent)
    {
        setStyleSheet(
            "color: #ffffff;"
            "background-color: #000000;"
            "selection-color: #ffffff;"
            "selection-background-color: green;"
        );
    }
};
    



// Window class
class MainWindow : public QMainWindow
{
public:
    MainWindow()
    {
        initUI();
        
    }

private:
    // Orinal and Modifed image matrixes, to swift between them quick
    cv::Mat imgOriginal; // Matrix to save original version of image
    cv::Mat imgModified; // Matrix for modified changes to image
    QLabel *imgLabel;

    void initUI()
    {
        setWindowTitle("Painting Analyzer App 2.0");
        //setMaximumSize(500, 500);


        // Buttons ------------------------------------------------------------
        QWidget *btn_widget = new QWidget();
        Button *btn_file = new Button("File");
        Button *btn_options = new Button("Options");
        Button *btn_reset = new Button("Reset");

        connect(btn_file, &QPushButton::clicked, this, &MainWindow::fileSelect);
        connect(btn_options, &QPushButton::clicked, this, &MainWindow::grayscale);
        
       
        QVBoxLayout *btn_layout = new QVBoxLayout(btn_widget);
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
        btn_widget->setMinimumWidth(0.3*width());
        btn_widget->setMaximumWidth(0.3*width());
    
        
        // Image ---------------------------------------------------------------
        imgLabel = new QLabel;
        imgSelect("../pics/traintrack.jpg");  

        // Layouts
        QWidget *central = new QWidget;
        QHBoxLayout *mainLayout = new QHBoxLayout(central);
        mainLayout->addWidget(btn_widget,1);
        mainLayout->addWidget(imgLabel, 5);
        central->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        
        setCentralWidget(central);

    }


    void fileSelect(){
        QString file1Name = 
            QFileDialog::getOpenFileName(
                this,
                tr("Open mage file"), 
                "/home/tobi/dev/painting-analyzer/pics", 
                tr("Image Files (*.jpg *.png)")
            );
        
        imgSelect(file1Name.toStdString());
    }

    // Sets the current image displayed on screen
    void imgSet(cv::Mat img)
    {   
        QImage qimg(
            img.data,
            img.cols,
            img.rows,
            img.step,
            QImage::Format_BGR888);

        QTextStream out(stdout);
        //out << height() << ">\n";
        //out << width() << ">\n";

        
        if (qimg.height() < qimg.width()){
            qimg = qimg.scaledToWidth(width() * 0.85, Qt::FastTransformation);
        }
        else {
            qimg = qimg.scaledToHeight(height() * 0.85, Qt::FastTransformation);
        }
        imgLabel->setPixmap(QPixmap::fromImage(qimg));
        imgLabel->setAlignment(Qt::AlignCenter);
    }

    // Select a new image to display
    void imgSelect(const std::string &path)
    {
        imgOriginal = cv::imread(path);
        if (imgOriginal.empty())
            return;
        imgOriginal.copyTo(imgModified);
        imgSet(imgOriginal);
    }

    // Connected to button
    void imgUpdate()
    {
        imgSelect("../pics/vi.jpg");
    }

    void grayscale()
    {
        if (isGrayImage(imgModified))
        {
            imgSet(imgOriginal);
            imgOriginal.copyTo(imgModified);
        }
        else
        {   
            grayscaleShades(imgOriginal).copyTo(imgModified);
            cvtColor(imgModified, imgModified, cv::COLOR_GRAY2BGR);  // Gray in three BGR channels for displaying
            imgSet(imgModified);
        }
    }
};

// for bug prints
// QTextStream out(stdout);
// out << "colors\n";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
