#include<iostream>
#include<fstream>


struct yarquivo
{
    char colunas[352];
    char linhas[288];
};
struct uarquivo{
    char colunas[176];
    char linhas[144];
};
struct varquivo{
    char colunas[176];
    char linhas[144];
};
static unsigned short image[3];
void lerYuV(std::string nome, int linhas,int colunas,int frame=0){
    frame--;

    std::ifstream fin;//cria o objeto para leitura do arquivo.
    fin.open(nome,std::ios_base::in|std::ios_base::binary);
    yarquivo y;
    uarquivo u;
    varquivo v;
    fin.ignore(linhas*colunas*frame);
    if(fin.is_open()){
        std::cout<<"Aqui está o conteúdo do arquivo"<<std::endl;
        char * ptr = (char * )&y;
        fin.read(ptr,sizeof(yarquivo));
        std::cout<<y.colunas<<std::endl;
        std::cout<<y.linhas<<std::endl;
        image[0]=(short)y;

        fin.read(ptr,sizeof(uarquivo));
        std::cout<<u.colunas<<std::endl;
        std::cout<<u.linhas<<std::endl;
        fin.read(ptr,sizeof(varquivo));
        std::cout<<v.colunas<<std::endl;
        std::cout<<v.linhas<<std::endl;
        
        fin.close();
    }
}

int main(){
    lerYuV("foreman.yuv",352,288);
}





