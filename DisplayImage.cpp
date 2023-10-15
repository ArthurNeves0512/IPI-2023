#include<iostream>
#include<fstream>
#include<opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
#include<vector>


/*
        openCV confia que colocaremos o tipo certo
        Nomes dos tipos seguem esse padrao:
CV_<bit_count><itentifier><num_of_channels>
    exemplo:
RGB image 8 bit-unsined is CV_8UC3
gray scale 8 bit-unsined is CV_8UC1
better to use DataType
example         - DataType<uint>::type =CV8UC1
toda imagem é um cv::Mat -- Matriz



*/

void dobrarImagem(unsigned char * imagemNormal,unsigned char * imagemAumentada){
    unsigned char *ptrImagemNormal = imagemNormal;
    unsigned char *ptrImagemAumentada = imagemAumentada;
    int quantColunaMatrizMaior =352;
    int quantLinhaMatrizMaior = 288;
    int a =0;
    //colocando zeros nas linhas e colunas
    for(unsigned int i =0;i<quantLinhaMatrizMaior/2;i++){//linhas menor
        for(unsigned int j=0;j<quantColunaMatrizMaior/2;j++){//colunas Menor
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

    ptrImagemAumentada=imagemAumentada;
    unsigned char superior;
    unsigned char esquerdo;
    for(int linhaAtual =0;linhaAtual<quantLinhaMatrizMaior;linhaAtual++){
        for( int colunaAtual =0;colunaAtual<quantColunaMatrizMaior;colunaAtual++){
            if(linhaAtual%2==0){
                if(colunaAtual%2!=0){
                    esquerdo = ptrImagemAumentada[linhaAtual*quantColunaMatrizMaior+colunaAtual -1];
                    ptrImagemAumentada[linhaAtual*quantColunaMatrizMaior+colunaAtual]=esquerdo;
                }
            }
            else{
                superior = ptrImagemAumentada[linhaAtual*quantColunaMatrizMaior+colunaAtual-quantColunaMatrizMaior];
                ptrImagemAumentada[linhaAtual*quantColunaMatrizMaior+colunaAtual]=superior;
            }
        }
    }

}


void lerYuV(std::string nome, int linhas,int colunas,int frame=0){
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

    cv::Mat imagemJunta;
    cv::Mat imagemColorida=cv::Mat(colunas,linhas,CV_8UC3) ;

    cv::Mat imageAntes = cv::Mat(colunas/2,linhas/2,CV_8U,v);
    cv::namedWindow("original",cv::WINDOW_AUTOSIZE);
    cv::imshow("original",cv::Mat(colunas/2,linhas/2,CV_8U, v));
    dobrarImagem((unsigned char *)v,*vMaior);
    dobrarImagem((unsigned char *)u,*uMaior);
    cv::Mat image= cv::Mat(colunas,linhas,CV_8U,vMaior);

    std::vector<cv::Mat> imagemReconstruída;
    imagemReconstruída.push_back(cv::Mat(colunas,linhas,CV_8U,y));
    imagemReconstruída.push_back(cv::Mat(colunas,linhas,CV_8U,uMaior));
    imagemReconstruída.push_back(cv::Mat(colunas,linhas,CV_8U,vMaior));
    cv::merge(imagemReconstruída,imagemJunta);
    cv::cvtColor(imagemJunta,imagemColorida,cv::COLOR_YUV2BGR);
    cv::imshow("COLORIDAAA",imagemColorida);
    cv::waitKey(0);
}

int main(){

    lerYuV("foreman.yuv",352,288,10);
}





