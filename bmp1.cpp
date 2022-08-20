#include<cstdio>
#include<iostream>
#include<cstring>
#include<fstream>
#include<vector>
#include<cstdlib>
#include<cmath>

#define file_header_size 14
#define info_header_size 40

using namespace std;

typedef struct FileHeader{
    uint8_t data[file_header_size];
    string fileType;
    int filesize;
}FileHeader;//ファイルヘッダー


typedef struct InfoHeader{
    uint8_t data[info_header_size];
    int infoHeaderSize;
    int width;
    int height;
    int clrPerPixel;
    int datasize;
} InfoHeader; //情報ヘッダー

typedef struct Color{
    int r;
    int g;
    int b;
} Color; //色情報

int bit2Integer(uint8_t b1,uint8_t b2,uint8_t b3, uint8_t b4);

class BitMapProcessor{
    FILE *fp;
    uint8_t *img;
    uint8_t *org;
    FileHeader fHeader;
    InfoHeader iHeader;

    public:
      BitMapProcessor(){
          fp = NULL;
          img = NULL;
          org = NULL;
      };

      ~BitMapProcessor(){
          fclose(fp);
          delete []img;
          delete []org;
      }
      void loadData(string filename);
      void dispBmpInfo();
      void writeData(string filename);
      Color getColor(int row,int col);
      void setColor(int row,int col,int r,int g,int b);
      void restore();
      int height(){return iHeader.height;};
      int width(){return iHeader.width;};
      private:
      void readFileHeader();
      void readInfoHeader();
      void ReadBmpData();
};

int bit2Integer(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4){
    return b1 + b2*256 + b3*256*256 + b4*256*256*256;
}

void BitMapProcessor::loadData(string filename){
    if(fp != NULL) fclose(fp);

    fp = fopen(filename.c_str(),"rb");
    if (fp == NULL) cout << "faile open file" << endl;
    readFileHeader();
    readInfoHeader();
    ReadBmpData();
}

void BitMapProcessor::readFileHeader(){
    uint8_t data[file_header_size];
    size_t size = fread(data,sizeof(uint8_t),file_header_size,fp);

    memcpy(fHeader.data,data,sizeof(data));
    fHeader.fileType = "";
    fHeader.fileType += data[0];
    fHeader.fileType += data[1];
    fHeader.filesize = bit2Integer(data[2],data[3],data[4],data[5]);
}

void BitMapProcessor::readInfoHeader(){
    uint8_t data[info_header_size];
    size_t size = fread(data,sizeof(uint8_t),info_header_size,fp);

    memcpy(iHeader.data,data,sizeof(data));
    iHeader.infoHeaderSize = bit2Integer(data[0],data[1],data[2],data[3]);
    iHeader.width = bit2Integer(data[4],data[5],data[6],data[7]);
    iHeader.height = bit2Integer(data[8],data[9],data[10],data[11]);
    iHeader.clrPerPixel = bit2Integer(data[14],data[15],0,0);
    iHeader.datasize = bit2Integer(data[20],data[21],data[22],data[23]);
}

void BitMapProcessor::ReadBmpData(){
    if(img != NULL) delete []img;

    int sz = iHeader.datasize;
    img = new uint8_t [sz];
    size_t size = fread(img,sizeof(uint8_t),sz,fp);
    if(size != sz) cout << "size error" << endl;
    if(org != NULL) delete []org;
    org = new uint8_t [sz];
    memcpy(org,img,sz);
}

void BitMapProcessor::dispBmpInfo(){
    cout << "file header info" << endl;
    cout << "File type:" << fHeader.fileType << endl;
    cout << "file size:" << fHeader.filesize << endl;

    cout << "info header info" << endl;
    cout << "info header size:" << iHeader.infoHeaderSize << endl;
    cout << "image wide:" << iHeader.width << endl;
    cout << "image high:" << iHeader.height << endl;
    cout << "color of 1 pixel" << iHeader.clrPerPixel << endl;
    cout << "image data size" << iHeader.datasize << endl;
}

void BitMapProcessor::writeData(string filename){
    FILE *out = fopen(filename.c_str(),"wb");

    if(out == NULL) cout << "Don't open file" << endl;

    fwrite(fHeader.data,sizeof(uint8_t),file_header_size,out);
    fwrite(iHeader.data,sizeof(uint8_t),info_header_size,out);
    fwrite(img,sizeof(uint8_t),iHeader.datasize,out);

    fclose(out);
}

Color BitMapProcessor::getColor(int row,int col){
    if(row < 0 || row >= iHeader.height) printf("getcolor() : error row\n");
    if(col < 0 || col >= iHeader.width) printf("getcolor: error col\n");

    int width = 3*iHeader.width;
    while (width % 4) ++width;

    int bPos = row*width + 3*col;
    int gPos = bPos + 1;
    int rPos = bPos + 2;

    Color color;
    color.r = img[rPos];
    color.g = img[gPos];
    color.b = img[bPos];

    return color;
    
}

void BitMapProcessor::setColor(int row,int col,int r,int g,int b){
    if(row < 0 || row >= iHeader.height) printf("getColor(): error row\n");
    if(col < 0 || col >= iHeader.width) printf("getColor(): error col\n");

    int width = 3*iHeader.width;
    while(width % 4) ++width;
    int bPos = row*width + 3*col;
    int gPos = bPos + 1;
    int rPos = bPos + 2;

    img[rPos] = r;
    img[gPos] = g;
    img[bPos] = b;
}

void BitMapProcessor::restore(){   
    memcpy(img,org,iHeader.datasize);
}

void half(BitMapProcessor *fp){
    for(int i = 0; i < fp->height();i++){
        for(int j = 0; j < fp->width();j++){
            int hr=0,hg=0,hb=0;
            hr = fp->getColor(i,j).r;
            hg = fp->getColor(i,j).g;
            hb = fp->getColor(i,j).b;
            hr = hr/2;
            hg = hg/2;
            hb = hb/2;
            

            fp->setColor(i,j,hr,hg,hb);
        }
    }
} //明るさ半分の関数

void gray(BitMapProcessor *fp)
{
    for (int i = 0; i < fp->height(); i++)
    {
        for (int j = 0; j < fp->width(); j++)
        {
            int ave = 0;
            ave += fp->getColor(i, j).r;
            ave += fp->getColor(i, j).g;
            ave += fp->getColor(i, j).b;
            ave /= 3;

            fp->setColor(i, j, ave, ave, ave);
        }
    }
} //グレイスケール画像*/

/*void spin90(BitMapProcessor *fp){
    int
}*/

void area(BitMapProcessor *fp,int r0,int r1,int c0,int c1){
    for(int i = 0; i < fp->height();i++){
        for(int j = 0; j < fp->width();j++){
            if(r0 <= i && i <= r1 && c0 <= j && j <= c1) continue;
            fp -> setColor(i,j,255,255,255);
        }
    }
}

int main(){
    BitMapProcessor fp;
    fp.loadData("begijnhof.bmp");
    fp.dispBmpInfo();

    half(&fp);
    fp.writeData("half.bmp");
    fp.restore();

    gray(&fp);
    fp.writeData("gray.bmp");
    fp.restore();

    /*spin90(&fp);
    fp.writeData("spin90.bmp");
    fp.restore();*/
    
    int top1,top2,top3,top4;
    cout << "Choice top measure" << endl;
    cout << "top1:" ; cin >> top1;
    cout << "top2:" ; cin >> top2;
    cout << "top3:" ; cin >> top3;
    cout << "top4:" ; cin >> top4;

    area(&fp,top1,top2,top3,top4);
    fp.writeData("are.bmp");
    fp.restore();

    return 0;
}