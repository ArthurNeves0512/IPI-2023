#include<iostream>
#include<fstream>
#include<opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
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

void interPololacaoMedia(unsigned char * imagemParaInterpolar,int quantColunaMatrizMaior =352 ,int quantLinhaMatrizMaior = 288){
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
    //1.1
    std::ifstream fin;//cria o objeto para leitura do arquivo.
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


    cv::Mat imageAntes = cv::Mat(linhas/2,colunas/2,CV_8U,v);
    cv::namedWindow("original",cv::WINDOW_FULLSCREEN|cv::WINDOW_AUTOSIZE);
    
    dobrarImagem((unsigned char *)v,*vMaior,colunas/2,linhas/2,colunas,linhas);
    interPololacaoMedia(*vMaior,colunas,linhas);
    dobrarImagem((unsigned char *)u,*uMaior,colunas/2,linhas/2,colunas,linhas);
    interPololacaoMedia(*uMaior,colunas,linhas);
    
    std::vector<cv::Mat> imagemReconstruída;
    imagemReconstruída.push_back(cv::Mat(linhas,colunas,CV_8U,y));
    imagemReconstruída.push_back(cv::Mat(linhas,colunas,CV_8U,uMaior));
    imagemReconstruída.push_back(cv::Mat(linhas,colunas,CV_8U,vMaior));

    cv::Mat imagemJunta;
    cv::Mat imagemColorida=cv::Mat(linhas,colunas,CV_8UC3) ;


    cv::merge(imagemReconstruída,imagemJunta);
    cv::cvtColor(imagemJunta,imagemColorida,cv::COLOR_YUV2BGR);
    // cv::imshow("antes", cv::Mat(linhas/2,colunas/2,CV_8U, v));
    //cv::imshow("original",imagemColorida);
    cv::Mat imagemComFiltragemNoDominioEspacial;
    cv::waitKey(0);
}

cv::Mat agucamentoLaplaciano(std::string nomeDaImagem,cv::Mat_<int> filtro){
    cv::Mat imgLida = cv::imread(nomeDaImagem);
    cv::Mat imagemFiltrada;
    cv::filter2D(imgLida,imagemFiltrada,-1,filtro);
    return imagemFiltrada;
}


int main(){

    //lerYuV("foreman.yuv",352,288,10);
    cv::Mat_<int> kernel(3,3);
    kernel<<1,1,1,1,-8,1,1,1,1;
    // cv::Mat img = cv::imread("Image1.pgm");
    // cv::imshow("aaaaaa", img);
    cv::Mat result;
    cv::GaussianBlur(cv::imread("Image1.pgm",cv::IMREAD_GRAYSCALE),result,cv::Size(3,3),1.0);
    cv::Mat_<float> kernel2(3,3);
    kernel2<< 0, 1, 0, 1, -4, 1, 0, 1, 0;
    cv::Mat laplacian;
    cv::filter2D(result,laplacian,-1,kernel2);
    cv::imshow("meu deus", laplacian);
    // cv::waitKey(0);
    // result= agucamentoLaplaciano("Image1.pgm",kernel);
    // cv::imshow("meu deus", result);
    cv::waitKey(0);

}





