#include<opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>
#include <stdio.h>

void imgHistograma(cv::Mat src);
void identificacaoTumor(cv::Mat imagem);
void imgHistograma(cv::Mat src){
    cv::Mat histograma;
    //eu crio uma Mat de zeros pois eu irei ficar incrementando o valor dos pixels, ou seja, quanto mais
    //claro, mais incidencia tem
    histograma = cv::Mat::zeros(256,1,CV_32F);
    src.convertTo(src,CV_32F);
    double value =0;
    for (int i = 0; i< src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            value = src.at<float>(i, j);

            /*
            
            [0][0][0][0][0][0][0][0][0]
            pego o valor do pixel na posição I,J da mat que quero calcular
            eu acesso o elemento da Mat do histograma, então por exemplo o valor
            da 5 e no i, J+1 da 5 tbm,logo

            [0][0][0][0][0][2][0][0][0] sacou????
            */
            histograma.at<float>(value) = histograma.at<float>(value) + 1;
            // std::cout<<histograma.at<unsigned char>(value)<<std::endl;
        }
    }

    cv::Mat histogramaImage(400,512,CV_8UC3,cv::Scalar(0,0,0));
    cv::Mat histogramaNormalizado;
    cv::normalize(histograma, histogramaNormalizado, 0, 400, cv::NORM_MINMAX, -1, cv::Mat());
    histogramaNormalizado.convertTo(histogramaNormalizado, CV_32F);

    for (int i = 0; i < 256; i++)
    {   
        // std::cout<<histograma.at<int>(i)<<std::endl;
        // cv::rectangle(histogramaImage,cv::Rect(2*i,0,2,256-i),cv::Scalar(255,0,0));
        cv::rectangle(histogramaImage,cv::Point(2*i,histogramaImage.rows-histogramaNormalizado.at<float>(i)),
        cv::Point(2*(i+1),histogramaImage.rows),cv::Scalar(255,0,0));
    }   
    // cv::namedWindow("Histograma",cv::WINDOW_NORMAL);
    // cv::imshow("Histograma",histogramaImage);
    // cv::waitKey(0);
}
void identificacaoTumor(cv::Mat imagem){
    cv::Mat imgTreshHold;
    cv::threshold(imagem,imgTreshHold,120,255,cv::THRESH_BINARY);
    cv::waitKey(0);
    cv::Mat kernelForMorpologicOperations=cv::Mat::ones(cv::Size(5,5),CV_8U);
    cv::erode(imgTreshHold,imgTreshHold,kernelForMorpologicOperations,cv::Point(-1,-1),1);
    cv::dilate(imgTreshHold,imgTreshHold,kernelForMorpologicOperations,cv::Point(-1,-1),1);
    cv::erode(imgTreshHold,imgTreshHold,kernelForMorpologicOperations,cv::Point(-1,-1),1);
    cv::dilate(imgTreshHold,imgTreshHold,kernelForMorpologicOperations,cv::Point(-1,-1),1);
    
    cv::Mat labelImage(imgTreshHold.size(),CV_8UC1);
    //temos 30 labels ou 30 componentes
    int numLabels = cv::connectedComponents(imgTreshHold,labelImage,4);
    
    int largestArea = -1;
    int largestLabel = -1;
    
    for(int label=2;label<numLabels;label++){
        //nos criamos uma imagem binária simplesmente ele vai passando pixel por pixel para saber
        // se aquele pixel é de um label do for ou não
        cv::Mat mascara = label==labelImage;
        // cv::imshow("mascara", mascara);
        // cv::waitKey(0);
        int area = cv::countNonZero(mascara);
        if(area>largestArea){
            largestArea=area;
            largestLabel=label;
        }
    }    
    cv::Mat MascaraTumor = labelImage==largestLabel;
    cv::imshow("threhold",imgTreshHold);
     std::vector<std::vector<cv::Point> > contours;
    cv::findContours(MascaraTumor,contours,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);
    cv::cvtColor(imagem,imagem,cv::COLOR_GRAY2RGB);
    cv::drawContours(imagem,contours,-1,cv::Scalar(0,255,0));

    cv::imshow("Tumor Achado pelo tamanho",imagem);
    cv::waitKey(0);
    
}

int main(){
    cv::Mat img = cv::imread("imagesT2png/brain.png",cv::IMREAD_GRAYSCALE);
    // cv::imshow("Imagem Original", img);
    // cv::waitKey(0);
    cv::Mat imgFiltered;    
    cv::GaussianBlur(img, imgFiltered, cv::Size(5,5), 0, 0);
    // cv::imshow("Gaussian Blur", img);
    cv::medianBlur(img,imgFiltered,3);
    // cv::imshow("Median Blur", img);
    //A partir daqui vou aplicar o histograma na img
    imgHistograma(imgFiltered);
    identificacaoTumor(imgFiltered);


    return 0;
}
