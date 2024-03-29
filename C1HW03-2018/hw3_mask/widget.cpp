#include "widget.h"
#include "ui_widget.h"

#define PI 3.14
#define E 2.7183

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui_config();
    connect(ui->pushButton_ChooseFolder,SIGNAL(clicked()),this,SLOT(choose_folder()));
    connect(ui->listWidget_ImageList,SIGNAL(currentRowChanged(int)),this,SLOT(displayOriIMG(int)));
    connect(ui->horizontalSlider_MaskSize,SIGNAL(valueChanged(int)),ui->lcdNumber,SLOT(display(int)));
    connect(ui->horizontalSlider_MaskSize,SIGNAL(valueChanged(int)),this,SLOT(setRCCount(int)));
    connect(ui->pushButton_Run,SIGNAL(clicked()),this,SLOT(run()));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),ui->lcdNumber_2,SLOT(display(int)));
}

void Widget::ui_config(){
    QTableWidgetItem *item;
    int sizeOfMask = ui->horizontalSlider_MaskSize->value();
    for(int i=0;i<sizeOfMask;i++){
        for(int j=0;j<sizeOfMask;j++){
            item = new QTableWidgetItem("1");
            ui->tableWidget->setItem(i,j,item);
        }
    }
    QString folderPath = "./img";
    QDir* dir = new QDir(folderPath);
    QStringList filter;
    filter << "*.jpg" << "*.bmp";
    QList<QFileInfo>* fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    int count = fileInfo->count();
    for(int i=0;i<count;i++){
        struct image img;

        QString fileName = fileInfo->at(i).fileName();
        img.name = fileName;
        QString path = fileInfo->at(i).filePath();
        img.path = path;
        img_vec.push_back(img);
        ui->listWidget_ImageList->addItem(fileName);
    }
}

/* Run !!!!!!!!!!!!!!!!! */
void Widget::run(){
    checkTheFunc();
//    ui->label->setText(0);
}

/* Choose the function */
int Widget::checkTheFunc(){
    ui->progressBar->setValue(0);
    int index = ui->tabWidget_CreateMask->currentIndex();
    switch (index) {
        case 0:{
            if(ui->radioButton_BoxKernal->isChecked()){
                boxKernal(ui->horizontalSlider_MaskSize->value());
                return 0;
                break;
            }
            if(ui->radioButton_GKernal->isChecked()){
                gaussianKernal(ui->horizontalSlider_MaskSize->value());
                return 0;
                break;
            }
            return -1;
            break;
        }
        case 1:{
            if(ui->radioButton_MedFilter->isChecked()){
                nonlinearKernalC(0, ui->horizontalSlider_MaskSize->value());
                return 0;
                break;
            }
            if(ui->radioButton_MaxFilter->isChecked()){
                nonlinearKernalC(1, ui->horizontalSlider_MaskSize->value());
                return 0;
                break;
            }
            if(ui->radioButton_MinFilter->isChecked()){
                nonlinearKernalC(2, ui->horizontalSlider_MaskSize->value());
                return 0;
                break;
            }
            return -1;
            break;
        }
        case 2:{
            int r = ui->horizontalSlider_MaskSize->value();
            customKernal(r);
            return 0;
            break;
        }
        case 3:{
            if(ui->radioButton->isChecked()){
                int t = ui->horizontalSlider->value();
                edgeDetection(t);
                return 0;
                break;
            }
            if(ui->radioButton_2->isChecked()){
                sobel();
                return 0;
                break;
            }



        }
    }
}

/* -- Edge detection -- */
/* Sobel */
void Widget::sobel(){
    clock_t start, end;
    int diff_time;
    start = clock();

    int oriWidth = oriImg->width();
    int oriHeight = oriImg->height();
    int mask1_data[3][3] = {
        { -1, 0, 1},
        { -2, 0, 2},
        { -1, 0, 1}
    };
    vector<vector<double>> mask1;
    for(int i=0;i<3;i++){
        vector<double> row;
        for(int j=0;j<3;j++){
            row.push_back(mask1_data[i][j]);
        }
        mask1.push_back(row);
    }
    int mask2_data[3][3] = {
        { -1,  -2,  -1},
        { 0,  0,  0},
        {1, 2, 1}
    };
    vector<vector<double>> mask2;
    for(int i=0;i<3;i++){
        vector<double> row;
        for(int j=0;j<3;j++){
            row.push_back(mask2_data[i][j]);
        }
        mask2.push_back(row);
    }
    cout.setf(ios::fixed,ios::floatfield);
    vector<vector<double>> img;
    vector<double> border(oriWidth+2,0);
    img.push_back(border);
    for(int i=0;i<=oriHeight+1;i++){
        vector<double> row;
        row.push_back(0);
        for(int j=0;j<=oriWidth+1;j++){
            if(i>0&&i<oriHeight+1&&j>0&&j<oriWidth+1){
//                cout << qRed(oriImg->pixel(j-1,i-1)) << " ";
                double setPixel = double(qRed(oriImg->pixel(j-1,i-1)));
                row.push_back(setPixel);
            }
        }
        row.push_back(0);
        cout << row.size() << endl;
        if(i>0&&i<oriHeight+1) img.push_back(row);
    }
    img.push_back(border);

    vector<vector<double>> g1;
    for(int i=2;i<oriHeight+2;i++){
        vector<double> row;
        for(int j=2;j<oriWidth+2;j++){
            double setPixel = 0;
            for(int k=2;k>=0;k--){
                for(int l=2;l>=0;l--){
//                    cout << i << " " << j << " " << k << " " << l << endl;
                    setPixel += mask1[k][l]*img[i-2+k][j-2+l];
                }
            }
            row.push_back(setPixel);
        }
        g1.push_back(row);
    }

    vector<vector<double>> g2;
    for(int i=2;i<oriHeight+2;i++){
        vector<double> row;
        for(int j=2;j<oriWidth+2;j++){
            double setPixel = 0;
            for(int k=2;k>=0;k--){
                for(int l=2;l>=0;l--){
//                    cout << i-2+k << " " << j-2+l << " " << k << " " << l << " ";
                    setPixel += mask2[k][l]*img[i-2+k][j-2+l];
//                    cout << mask2[k][l] << " " << img[i-2+k][j-2+l] << setPixel << endl;
                }
            }
            row.push_back(setPixel);
//            cout << endl;
        }
        g2.push_back(row);
    }
    vector<vector<double>> g3;
    for(int i=0;i<oriHeight;i++){
        vector<double> row;
        for(int j=0;j<oriWidth;j++){
            double l = pow(g1[i][j],2);
            double r = pow(g2[i][j],2);
            double d = sqrt(l+r);
            if(d>255) d=255;
            if(d<0) d=0;
            row.push_back(d);
        }
        g3.push_back(row);
        double d = 100*i/oriHeight+1;
        ui->progressBar->setValue(int(d));
    }

    QImage* resultIMG = new QImage(oriWidth,oriHeight,QImage::Format_RGB32);
    for(int i=0;i<oriHeight;i++){
        for(int j=0;j<oriWidth;j++){
            resultIMG->setPixel(j,i,qRgb(int(g3[i][j]),int(g3[i][j]),int(g3[i][j])));
        }
    }
    ui->label_aft->setPixmap(QPixmap::fromImage(resultIMG->scaled(360,360,Qt::KeepAspectRatio)));

    end = clock();
    diff_time = (end-start)/1000;
    ui->label->setText(QString::number(diff_time));
}

/* Edge detection */
void Widget::edgeDetection(int t){
    clock_t start, end;
    int diff_time;
    start = clock();

    int oriWidth = oriImg->width();
    int oriHeight = oriImg->height();
    int r = 5;
    int mask_data[5][5] = {
        { 0,  0, -1,  0,  0},
        { 0, -1, -2, -1,  0},
        {-1, -2, 16, -1, -2},
        { 0, -1, -2, -1,  0},
        { 0,  0, -1,  0,  0}
    };
    vector<vector<double>> mask;
    for(int i=0;i<5;i++){
        vector<double> row;
        for(int j=0;j<5;j++){
            row.push_back(mask_data[i][j]);
        }
        mask.push_back(row);
    }

    QImage* withBorderIMG = addBorder(oriImg, r);

    vector<vector<int>> img;
    vector<int> border(oriWidth+2,0);
    img.push_back(border);
    for(int i=0;i<oriHeight+r;i++){
        vector<int> row;
        row.push_back(0);
        for(int j=0;j<oriWidth+r;j++){
            double setPixel = 0;
            for(int k=r-1;k>=0;k--){
                for(int l=r-1;l>=0;l--){
                    setPixel += mask[k][l]*double(qRed(withBorderIMG->pixel(j+k,i+k)));
                }
            }
            if(j>3&&j<oriWidth+4) row.push_back(setPixel);
        }
        row.push_back(0);
        if(i>3&&i<oriHeight+4) img.push_back(row);
    }
    img.push_back(border);

    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            cout << img[oriHeight+1-i][oriWidth+1-j] << " ";
        }
        cout << endl;
    }
    QImage* storeResultIMG = new QImage(oriWidth,oriHeight,QImage::Format_RGB32);
    for(int i=0;i<oriHeight;i++){
        for(int j=0;j<oriWidth;j++){
            vector<int> v;
            for(int k=-1;k<=1;k++){
                for(int l=-1;l<=1;l++){
                    int x = img[i+1+k][j+1+l];
                    v.push_back(x);
                }
            }
            int check = 0;
            int setPix = 0;
            cout << endl;
            for(int m=0;m<4;m++){
                int pl = v[m];
                int pu = v[8-m];
                if(pl*pu<0&&abs(pl-pu)>t) check++;
                if(check>=2){
                    setPix=255; 
                    break;
                }
            }

            storeResultIMG->setPixel(j,i,qRgb(setPix, setPix, setPix));
        }
        double d = 100*i/oriHeight+1;
        ui->progressBar->setValue(int(d));
    }
    ui->label_aft->setPixmap(QPixmap::fromImage(storeResultIMG->scaled(360,360,Qt::KeepAspectRatio)));

    end = clock();
    diff_time = (end-start)/1000;
    ui->label->setText(QString::number(diff_time));
}

/* ----- Non-linear ----- */
/* Static the non-linear mask */
void Widget::nonlinearKernalC(int type, int r){
    clock_t start, end;
    int diff_time;
    start = clock();

    QImage* withBorderIMG = addBorder(oriImg, r);
    int oriWidth = oriImg->width();
    int oriHeight = oriImg->height();
    QImage* storeResultIMG = new QImage(oriWidth+r,oriHeight+r,QImage::Format_RGB32);
    for(int i=0;i<oriHeight+r;i++){
        for(int j=0;j<oriWidth+r;j++){
            vector<int> v;
            for(int k=0;k<r;k++){
                for(int l=0;l<r;l++){
                    v.push_back(qRed(withBorderIMG->pixel(j+k,i+l)));
                }
            }
            sort(v.begin(),v.end());
            int setPix;
            switch (type) {
                // med
                case 0:{
                    int lengh = pow(r,2);
                    if(lengh%2){
                        setPix = v[int(lengh/2)];
                    }else{
                        setPix = (v[int(lengh/2)]+v[int(lengh/2)+1])/2;
                    }
                    break;
                }
                // max
                case 1:{
                    setPix = v[pow(r,2)-1];
                    break;
                }
                // min
                case 2:{
                    setPix = v[0];
                    break;
                }
            }
            storeResultIMG->setPixel(j,i,qRgb(setPix, setPix, setPix));
        }
        double d = 100*i/(oriHeight+r)+1;
        ui->progressBar->setValue(int(d));
    }
    QImage* retureIMG = new QImage(oriWidth,oriHeight,QImage::Format_RGB32);
    for(int i=0;i<(oriHeight);i++){
        for(int j=0;j<(oriWidth);j++){
            retureIMG->setPixel(j,i,storeResultIMG->pixel(j+int(r/2)+1,i+int(r/2)+1));
        }
    }
    ui->label_aft->setPixmap(QPixmap::fromImage(retureIMG->scaled(360,360,Qt::KeepAspectRatio)));

    end = clock();
    diff_time = (end-start)/1000;
    ui->label->setText(QString::number(diff_time));
}

/* -----   Linear   ----- */
/* Create a custom kernal input linearKernalC() */
/* Input: the size of custom kernal */
void Widget::customKernal(int r){
    vector<vector<double>> mask;
    for(int i=0;i<r;i++){
        vector<double> row;
        for(int j=0;j<r;j++){
            QString str = ui->tableWidget->item(i,j)->text();
            row.push_back(str.toDouble());
        }
        mask.push_back(row);
    }
    QImage aftImg = linearKernalC(mask,r,true);
    ui->label_aft->setPixmap(QPixmap::fromImage(aftImg.scaled(360,360,Qt::KeepAspectRatio)));
}

/* Create a gaussian kernal input linearKernalC() */
/* Input: the size of gaussian kernal */
void Widget::gaussianKernal(int r){
    vector<vector<double>> mask;
    int middleP = float(r)/2-0.5;
    cout << middleP << endl;
    for(int i=0;i<r;i++){
        vector<double> row;
        for(int j=0;j<r;j++){
            double d = pow(E,double((-(pow(i-middleP,2)+pow(j-middleP,2)))/2));
            row.push_back(d);
        }
        mask.push_back(row);
    }
    QImage aftImg = linearKernalC(mask,r,true);
    ui->label_aft->setPixmap(QPixmap::fromImage(aftImg.scaled(360,360,Qt::KeepAspectRatio)));
}

/* Create a box kernal input linearKernalC() */
/* Input: the size of box kernal */
void Widget::boxKernal(int r){
    vector<vector<double>> mask;
    for(int i=0;i<r;i++){
        vector<double> row(r,1);
        mask.push_back(row);
    }
    QImage aftImg = linearKernalC(mask,r,true);
    ui->label_aft->setPixmap(QPixmap::fromImage(aftImg.scaled(360,360,Qt::KeepAspectRatio)));
}

/* Compute the linear kernal */
/* Input:  2-d array, vector<vevtor<double>> */
/* Return: the image after be computed, QImage */
QImage Widget::linearKernalC(vector<vector<double>> vec, int r, bool s){
    clock_t start, end;
    int diff_time;
    start = clock();

    double sum;
    for(int i=0;i<r;i++){
        for(int j=0;j<r;j++){
            sum+=vec[i][j];
        }
    }
    if(sum==0) sum=1;
    QImage* withBorderIMG = addBorder(oriImg, r);
    int oriWidth = oriImg->width();
    int oriHeight = oriImg->height();
    QImage* storeResultIMG = new QImage(oriWidth+r,oriHeight+r,QImage::Format_RGB32);
    for(int i=0;i<oriHeight+r;i++){
        for(int j=0;j<oriWidth+r;j++){
            double setPixel = 0;
            for(int k=r-1;k>=0;k--){
                for(int l=r-1;l>=0;l--){
                    setPixel += vec[k][l]*double(qRed(withBorderIMG->pixel(j+k,i+l)));
                }
            }
            setPixel/=sum;
            if(s){
                if(setPixel>255) setPixel=255;
                if(setPixel<0) setPixel=0;
            }
            storeResultIMG->setPixel(j,i,qRgb(int(setPixel), int(setPixel), int(setPixel)));
        }
        double d = 100*i/(oriHeight+r)+1;
        ui->progressBar->setValue(int(d));
    }
    QImage* retureIMG = new QImage(oriWidth,oriHeight,QImage::Format_RGB32);
    for(int i=0;i<(oriHeight);i++){
        for(int j=0;j<(oriWidth);j++){
            retureIMG->setPixel(j,i,storeResultIMG->pixel(j+int(r/2)+1,i+int(r/2)+1));
        }
    }

    end = clock();
    diff_time = (end-start)/1000;
    ui->label->setText(QString::number(diff_time));
    cout << diff_time << endl;
    return *retureIMG;
}

/* Add border(width=r) to IMG which will be mask */
QImage* Widget::addBorder(QImage* img, int r){
    QImage* aftImg;
    int width = img->width();
    int height = img->height();
    aftImg = new QImage(width+2*r,height+2*r,QImage::Format_RGB32);
    for(int i=0;i<height+2*r;i++){
        for(int j=0;j<width+2*r;j++){
            if(i<r||i>(height+r-1)){
                aftImg->setPixel(j,i,qRgb(0,0,0));
            }else if(j<r||j>(width+r-1)){
                aftImg->setPixel(j,i,qRgb(0,0,0));
            }else{
                aftImg->setPixel(j,i,img->pixel(j-r,i-r));
            }
        }
    }

    return aftImg;
}

/* Set the row&column of inpur kernal */
/* Input:  the count of row&colume, int */
void Widget::setRCCount(int c){
    ui->tableWidget->setRowCount(c);
    ui->tableWidget->setColumnCount(c);
    QTableWidgetItem *item;
    int sizeOfMask = ui->horizontalSlider_MaskSize->value();
    for(int i=0;i<sizeOfMask;i++){
        for(int j=0;j<sizeOfMask;j++){
            item = new QTableWidgetItem("1");
            ui->tableWidget->setItem(i,j,item);
        }
    }

}

/* Selete the Image folder and store them at a vector<QImage> */
void Widget::choose_folder(){
    QString folderPath = QFileDialog::getExistingDirectory(NULL,tr("選擇資料夾"),"./",QFileDialog::ShowDirsOnly);
    QDir* dir = new QDir(folderPath);
    QStringList filter;
    filter << "*.jpg" << "*.bmp";
    QList<QFileInfo>* fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    int count = fileInfo->count();
    for(int i=0;i<count;i++){
        struct image img;

        QString fileName = fileInfo->at(i).fileName();
        img.name = fileName;
        QString path = fileInfo->at(i).filePath();
        img.path = path;
        img_vec.push_back(img);
        ui->listWidget_ImageList->addItem(fileName);
    }
}

/* Display the IMG[d] at label_ori */
void Widget::displayOriIMG(int d){
    displayIMG(ui->label_ori,d);
}

/* Display the IMG[d] at the label */
void Widget::displayIMG(QLabel* label,int d){
    int width = label->width();
    int height = label->height();
    oriImg = new QImage(width,height,QImage::Format_RGB32);
    QImage qimg;
    qimg.load(img_vec[d].path);
    *oriImg = qimg.scaled(width,height,Qt::KeepAspectRatio);
    label->setPixmap(QPixmap::fromImage(*oriImg));
}

Widget::~Widget()
{
    delete ui;
}
