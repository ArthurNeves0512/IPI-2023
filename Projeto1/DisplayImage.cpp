#include<iostream>
#include<fstream>
#include<opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>
#include<vector>


void dobrarImagem(unsigned char * imagemNormal,unsigned char * imagemAumentada,int quantColunaMatrizMenor = 176,int quantLinhaMatrizMenor =144, int quantColunaMatrizMaior =352 ,int quantLinhaMatrizMaior = 288){
    unsigned char *ptrImagemNormal = imagemNormal;
    unsigned char *ptrImagemAumentada = imagemAumentada;
    for(unsigned int i =0;i<quantLinhaMatrizMenor;i++){//linhas menor
        for(unsigned int j=0;j<quantColunaMatrizMenor;j++){//colunas Menor
            *ptrImagemAumentada= *ptrImagemNormal;
            ptrImagemAumentada++;
            *ptrImagemAumentada=0;
            ptrImagemAumentada++;
            ptrImagemNormal++;
        }
        for(unsigned int m =0;m<quantColunaMatrizMaior;m++){//colunas da imagem maior
            *ptrImagemAumentada=0;
            ptrImagemAumentada++;
        }
        
    }
}

void interpolacaoPadrao(unsigned char * imagemParaInterpolar,int quantColunaMatrizMaior =352 ,int quantLinhaMatrizMaior = 288){
    unsigned char *ptrImagemInterpolar = imagemParaInterpolar;
    unsigned char superior;
    unsigned char esquerdo;
    for(int linhaAtual =0;linhaAtual<quantLinhaMatrizMaior;linhaAtual++){
        for( int colunaAtual =0;colunaAtual<quantColunaMatrizMaior;colunaAtual++){
            if(linhaAtual%2==0){
                if(colunaAtual%2!=0){
                    esquerdo = ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual -1];
                    ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual]=esquerdo;
                }
            }
            else{
                superior = ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual-quantColunaMatrizMaior];
                ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual]=superior;
            }
        }
    }
}

void interpolacaoMedia(unsigned char * imagemParaInterpolar,int quantColunaMatrizMaior =352 ,int quantLinhaMatrizMaior = 288){
    unsigned char *ptrImagemInterpolar = imagemParaInterpolar;
    unsigned char superior;
    unsigned char media;
    unsigned char esquerdo;
    for(int linhaAtual =0;linhaAtual<quantLinhaMatrizMaior;linhaAtual++){
        for( int colunaAtual =0;colunaAtual<quantColunaMatrizMaior;colunaAtual++){
            if(linhaAtual%2==0){
                if(colunaAtual%2!=0&&colunaAtual<quantColunaMatrizMaior-1){
                    media = (ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual -1]+ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual +1])/2;
                    ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual]=media;
                }
                else if(colunaAtual%2!=0&&colunaAtual==quantColunaMatrizMaior-1){
                    esquerdo = ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual -1];
                    ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual]=esquerdo;
                }
                
            }
        }
    }

    for(int linhaAtual =0;linhaAtual<quantLinhaMatrizMaior;linhaAtual++){
        for( int colunaAtual =0;colunaAtual<quantColunaMatrizMaior;colunaAtual++){
            if(linhaAtual%2!=0){
                unsigned char superior = ptrImagemInterpolar[(linhaAtual-1)*quantColunaMatrizMaior+colunaAtual];
                unsigned char inferior = ptrImagemInterpolar[(linhaAtual+1)*quantColunaMatrizMaior+colunaAtual];
                unsigned char media = (superior+inferior)/2;
                ptrImagemInterpolar[colunaAtual + linhaAtual*quantColunaMatrizMaior]=media;
            }
        }
    }
    
    
}

void lerYuV(std::string nome, int colunas,int linhas,int frame=0){
    frame--;
    //está para dobrar o y e quadruplicar o u e v, caso queira trocar, precisa mudar os tamanhos, etc 
    std::ifstream fin;
    fin.open(nome,std::ios_base::in|std::ios_base::binary);

    unsigned char y[linhas][colunas];
    unsigned char u[linhas/2][colunas/2];
    unsigned char v[(linhas/2)][(colunas/2)];


    unsigned int tamanhoFrame = sizeof(y)+sizeof(u)+sizeof(v);
    fin.ignore(frame*tamanhoFrame);// 1*352*288
    if(fin.is_open()){
        fin.read((char * )&y,sizeof(y));
        fin.read((char * )&u,sizeof(u));
        fin.read((char * )&v,sizeof(v));
        fin.close();
    }
    unsigned char uMaior[colunas][linhas];
    unsigned char vMaior[colunas][linhas];
    unsigned char yMaior[colunas*2][linhas*2];
    unsigned char uMaior2[colunas*2][linhas*2];
    unsigned char vMaior2[colunas*2][linhas*2];


    cv::Mat imageAntes = cv::Mat(linhas/2,colunas/2,CV_8U,v);
    
    dobrarImagem((unsigned char *)v,*vMaior,colunas/2,linhas/2,colunas,linhas);
    dobrarImagem((unsigned char *)u,*uMaior,colunas/2,linhas/2,colunas,linhas);

    interpolacaoMedia(*vMaior,colunas,linhas);
    interpolacaoMedia(*uMaior,colunas,linhas);

    dobrarImagem((unsigned char *)y,*yMaior,colunas,linhas,colunas*2,2*linhas);
    dobrarImagem((unsigned char *)uMaior,*uMaior2,colunas,linhas,colunas*2,2*linhas);
    dobrarImagem((unsigned char *)vMaior,*vMaior2,colunas,linhas,colunas*2,2*linhas);

    interpolacaoMedia(*vMaior2,colunas*2,linhas*2);
    interpolacaoMedia(*uMaior2,colunas*2,linhas*2);
    interpolacaoMedia(*yMaior,colunas*2,linhas*2);


    std::vector<cv::Mat> imagemReconstruída;
    imagemReconstruída.push_back(cv::Mat(linhas*2,colunas*2,CV_8U,yMaior));
    imagemReconstruída.push_back(cv::Mat(linhas*2,colunas*2,CV_8U,uMaior2));
    imagemReconstruída.push_back(cv::Mat(linhas*2,colunas*2,CV_8U,vMaior2));

    cv::Mat imagemJunta;
    cv::Mat imagemColorida=cv::Mat(linhas*2,colunas*2,CV_8UC3) ;

    cv::merge(imagemReconstruída,imagemJunta);
    cv::cvtColor(imagemJunta,imagemColorida,cv::COLOR_YUV2BGR);
    cv::imshow("interpolação Media",imagemColorida);
    cv::waitKey(0);
}

float regeitaFiltroNotchPositive(float raio, float v,float u,float centroX,float centroY,float notchU,float notchK){
    int a =u-centroX-notchU;
    int b =v-centroY-notchK;
    float distanciaPositiva = std::pow(a,2);
    distanciaPositiva = std::sqrt(distanciaPositiva+std::pow(b,2));
    float filtro =    1/
                    (1+pow((raio/distanciaPositiva),8));
    return filtro      ;                                                                                                                                                                                                                       ;
}

cv::Mat agucamentoLaplaciano(std::string nomeDaImagem,cv::Mat_<int> filtro){
    cv::Mat imgLida = cv::imread(nomeDaImagem);
    cv::Mat imagemFiltrada;
    cv::filter2D(imgLida,imagemFiltrada,-1,filtro);
    return imagemFiltrada;
}




float regeitaFiltroNotchNegative(float raio, float v,float u,float centroX,float centroY,float notchU,float notchK){
    int a =u-centroX+notchU;
    int b =v-centroY+notchK;
    float distanciaPositiva = std::pow(a,2);
    distanciaPositiva = std::sqrt(distanciaPositiva+std::pow(b,2));
    float filtro =    1/
                    (1+pow((raio/distanciaPositiva),8));
    return filtro      ;
}
void filtrarNaFrequencia(){
    //Construção da imagem com o domínio da frequencia
    cv::Mat img = cv::imread("./imagens/moire.tif",cv::IMREAD_GRAYSCALE);

    cv::Mat_<float> imgFloat;
    img.convertTo(imgFloat,CV_32F,1.0/255.0);
    
    // cv::Mat arrayFloat[2]={imgFloat,cv::Mat::zeros(imgFloat.size(),CV_32F)};
    // cv::merge(arrayFloat,2,teste2);
    

    cv::Mat imgComplex [2]={imgFloat,cv::Mat::zeros(imgFloat.size(),CV_32F)};
    cv::Mat dftReady,odio;
    cv::merge(imgComplex,2,dftReady);
    cv::merge(imgComplex,2,odio);
    cv::dft(odio,odio,cv::DFT_SCALE);
    
    cv::Mat dftOriginal;
    cv::Mat saida;

    cv::dft(dftReady,saida,cv::DFT_COMPLEX_OUTPUT);
    
    cv::split(saida,imgComplex);

    cv::magnitude(imgComplex[0],imgComplex[1],dftReady);
    dftReady+=cv::Scalar::all(1);
    cv::log(dftReady,dftReady);

    int meioColunas=dftReady.cols/2;
    int meioLinhas=dftReady.rows/2;

    cv::Mat esquerdaSuperior(dftReady,cv::Rect(0,0,meioColunas,meioLinhas));
    cv::Mat direitaSuperior(dftReady,cv::Rect(meioColunas,0,meioColunas,meioLinhas));
    cv::Mat esquerdaInferior(dftReady,cv::Rect(0,meioLinhas,meioColunas,meioLinhas));
    cv::Mat DireitaInferior(dftReady,cv::Rect(meioColunas,meioLinhas,meioColunas,meioLinhas));


    cv::Mat temp;
    esquerdaSuperior.copyTo(temp);
    DireitaInferior.copyTo(esquerdaSuperior);
    temp.copyTo(DireitaInferior);

    direitaSuperior.copyTo(temp);
    esquerdaInferior.copyTo(direitaSuperior);
    temp.copyTo(esquerdaInferior);

    cv::normalize(dftReady,dftReady,0,1,cv::NORM_MINMAX);

    // cv::namedWindow("imagem domínio da frequencia",cv::WINDOW_GUI_EXPANDED);
    
    // cv::imshow("imagem domínio da frequencia",dftReady);
    cv::waitKey(0);

    //até aqui é a parte de pegar a transformada de fourier
    //
    cv::Mat filtro(cv::Size(dftReady.cols,dftReady.rows),CV_32F,cv::Scalar(1.0));
    cv::Mat teste(cv::Size(dftReady.cols,dftReady.rows),CV_32F,cv::Scalar(1.0));

    for(int i=0;i<dftReady.rows;i++){
        for(int j =0;j<dftReady.cols;j++){
            float valor = regeitaFiltroNotchPositive(10,j,i,meioLinhas,meioColunas,39,30);
            teste.at<float>(i,j)=valor;
        }
    }
    cv::multiply(teste,filtro,filtro);
    for(int i=0;i<dftReady.rows;i++){
        for(int j =0;j<dftReady.cols;j++){
            float valor = regeitaFiltroNotchNegative(10,j,i,meioLinhas,meioColunas,39,30);
            teste.at<float>(i,j)=valor;
        }
    }
    cv::multiply(teste,filtro,filtro);

    for(int i=0;i<dftReady.rows;i++){
        for(int j =0;j<dftReady.cols;j++){
            float valor = regeitaFiltroNotchNegative(5,j,i,meioLinhas,meioColunas,78,30);
            teste.at<float>(i,j)=valor;
        }
    }
    cv::multiply(teste,filtro,filtro);
    for(int i=0;i<dftReady.rows;i++){
        for(int j =0;j<dftReady.cols;j++){
            float valor = regeitaFiltroNotchPositive(5,j,i,meioLinhas,meioColunas,78,30);
            teste.at<float>(i,j)=valor;
        }
    }
    cv::multiply(teste,filtro,filtro);

    




    for(int i=0;i<dftReady.rows;i++){
        for(int j =0;j<dftReady.cols;j++){
            float valor = regeitaFiltroNotchPositive(10,j,i,meioLinhas,meioColunas,-39,30);
            teste.at<float>(i,j)=valor;
        }
    }
    cv::multiply(teste,filtro,filtro);
    for(int i=0;i<dftReady.rows;i++){
        for(int j =0;j<dftReady.cols;j++){
            float valor = regeitaFiltroNotchNegative(10,j,i,meioLinhas,meioColunas,-39,30);
            teste.at<float>(i,j)=valor;
        }
    }
    cv::multiply(teste,filtro,filtro);

    for(int i=0;i<dftReady.rows;i++){
        for(int j =0;j<dftReady.cols;j++){
            float valor = regeitaFiltroNotchNegative(5,j,i,meioLinhas,meioColunas,-78,30);
            teste.at<float>(i,j)=valor;
        }
    }
    cv::multiply(teste,filtro,filtro);
    for(int i=0;i<dftReady.rows;i++){
        for(int j =0;j<dftReady.cols;j++){
            float valor = regeitaFiltroNotchPositive(5,j,i,meioLinhas,meioColunas,-78,30);
            teste.at<float>(i,j)=valor;
        }
    }
    cv::multiply(teste,filtro,filtro);

    cv::Mat esquerdaSuperior1(filtro,cv::Rect(0,0,meioColunas,meioLinhas));
    cv::Mat direitaSuperior1(filtro,cv::Rect(meioColunas,0,meioColunas,meioLinhas));
    cv::Mat esquerdaInferior1(filtro,cv::Rect(0,meioLinhas,meioColunas,meioLinhas));
    cv::Mat DireitaInferior1(filtro,cv::Rect(meioColunas,meioLinhas,meioColunas,meioLinhas));

    esquerdaSuperior1.copyTo(temp);
    DireitaInferior1.copyTo(esquerdaSuperior1);
    temp.copyTo(DireitaInferior1);

    direitaSuperior1.copyTo(temp);
    esquerdaInferior1.copyTo(direitaSuperior1);
    temp.copyTo(esquerdaInferior1);

    cv::Mat array[2]={filtro,cv::Mat::zeros(filtro.size(),CV_32F)};
    cv::Mat fim;
    cv::merge(array,2,fim);
    cv::Mat imgFinal;
    cv::mulSpectrums(fim,odio,imgFinal,0);
    
    cv::idft(imgFinal,imgFinal);
    split(imgFinal,array);
    imgFinal=array[0];
    

    
    cv::normalize(imgFinal,imgFinal,0,255,cv::NORM_MINMAX);
    imgFinal.convertTo(imgFinal,CV_8U);
    cv::imshow("img com filtro",imgFinal);
    cv::waitKey(0);
}

void agucamento(cv::Mat img, cv::Mat kernel){
    cv::filter2D(img,img,-1,kernel);
    cv::imshow("Apenas com Laplace", img);
    cv::waitKey(0);
}
void agucamento(cv::Mat img, cv::Mat kernel, float sigma){
    cv::Mat temp;
    cv::GaussianBlur(img,temp,cv::Size(3,3),sigma);
    
    cv::filter2D(temp,temp,-1,kernel);
    cv::imshow("Gauss e Laplace", temp);
    cv::waitKey(0);
}
int main(){
    lerYuV("./imagens/foreman.yuv",352,288,10);
    cv::imshow("Imagem para Filtrar", cv::imread("./imagens/moire.tif"));
    cv::waitKey(0);
    filtrarNaFrequencia();

    
    cv::waitKey(0);

}





