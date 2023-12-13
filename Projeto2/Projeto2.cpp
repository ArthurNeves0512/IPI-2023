#include<opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>
#include <stdio.h>

void imgHistograma(cv::Mat src);
void identificacaoTumor(cv::Mat imagem);
void primeiraQuestao();
void segundaQuestao();
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
    // cv::imshow("threhold",imgTreshHold);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(MascaraTumor,contours,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);
    cv::cvtColor(imagem,imagem,cv::COLOR_GRAY2RGB);
    cv::drawContours(imagem,contours,-1,cv::Scalar(0,255,0));

    cv::imshow("Tumor Achado pelo tamanho",imagem);
    cv::waitKey(0);
    
}

void primeiraQuestao(){
    cv::Mat imgBrain = cv::imread("imagesT2png/brain.png",cv::IMREAD_GRAYSCALE);
    // cv::imshow("Imagem Original", imgBrain);
    // cv::waitKey(0);
    cv::Mat imgBrainFiltered;    
    cv::GaussianBlur(imgBrain, imgBrainFiltered, cv::Size(5,5), 0, 0);
    // cv::imshow("Gaussian Blur", imgBrain);
    cv::medianBlur(imgBrain,imgBrainFiltered,3);
    // cv::imshow("Median Blur", imgBrain);
    //A partir daqui vou aplicar o histograma na imgBrain
    imgHistograma(imgBrainFiltered);
    identificacaoTumor(imgBrainFiltered);
}
void segmentacao(cv::Mat img){
    
}


cv::Mat imagemQuantizacao(cv::Mat img, int numeroDeClusters, int iteracoes){
//primeiro mapeamos a nossa fonte para amostras
//uma matriz de 26730 linhas com 3 colunas(quantida de canais da img de input) e 1 canal
cv::Mat amostras(img.rows * img.cols, img.channels(), CV_32F);
// std::cout<<samples.cols<<" "<<samples.rows<<" "<<samples.channels();
    //vamos agora iterar a nossa img de entrada para preencher a amostra;
    for (int y = 0; y < img.rows; y++){
        for (int x = 0; x < img.cols; x++){
            for (int channel = 0; channel < img.channels(); channel++){
                if(img.channels()==3){
                    amostras.at<float>(y+x*img.rows,channel)=img.at<cv::Vec3b>(y,x)[channel];
                }
                else{
                    amostras.at<float>(y+x*img.rows,channel)=img.at<float>(y,x);
                }    
            }
        }
    }
    cv::Mat labels;
    int attempts = 5;
    cv::Mat centers;
    cv::kmeans(amostras,numeroDeClusters,labels,
    cv::TermCriteria(cv::TermCriteria::EPS|cv::TermCriteria::MAX_ITER,iteracoes,0.0001),attempts,cv::KMEANS_PP_CENTERS,centers);
    cv::Mat imgClusterizada(img.size(),img.type());
    for (int y = 0; y < img.rows; y++)
		for (int x = 0; x < img.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x * img.rows, 0);
			if (img.channels()==3) {
				for (int i = 0; i < img.channels(); i++) {
					imgClusterizada.at<cv::Vec3b>(y, x)[i] = centers.at<float>(cluster_idx, i);
				}
			}
			else {
				imgClusterizada.at<unsigned char>(y, x) = centers.at<float>(cluster_idx, 0);
			}
		}
	imshow("clustered image 6", imgClusterizada);
    cv::waitKey(0);
	return imgClusterizada;

    
}

void segundaQuestao(){
    cv::Mat imgOrion = cv::imread("/home/arthurneves/Projetos/IPI/Projeto2/imagesT2png/onion.png");
    // cv::imshow("orion",imgOrion);
    // cv::waitKey(0);
    int numeroDeClusters = 10;
    int iteracoes = 100;
    cv::Mat imagemSegmentada = imagemQuantizacao(imgOrion,6,iteracoes);
    // cv::imshow("img clusterizada", labels);
    // cv::waitKey(0);
    

}
int main(){
    // primeiraQuestao();
    segundaQuestao();


    return 0;
}
