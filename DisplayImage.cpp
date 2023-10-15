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
            else{
                media = ptrImagemInterpolar[linhaAtual-1*quantColunaMatrizMaior +colunaAtual];
                superior = ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual-quantColunaMatrizMaior];
                ptrImagemInterpolar[linhaAtual*quantColunaMatrizMaior+colunaAtual]=superior;
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
    //cv::imshow("original",cv::Mat(linhas/2,colunas/2,CV_8U, v));
    
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
    cv::imshow("original",imagemColorida);
    cv::waitKey(0);
}

int main(){

    lerYuV("foreman.yuv",352,288,10);
}





