#ifndef IMAGE_H
#define IMAGE_H
#include <video_io.h>
#include <exception>
#include <stdexcept>
#include <math.h>
#include <common_types.h>
//****** Simple class to manage generic 2D images, without the mayor libraries *******//

//#define SAFE_IMAGE_ACCESS         //Flag to check for index errors

template <typename DataType>
class Image{
protected:
    const Size2D size={0,0};    //Img size
    const uint bsize=0;         //Data size
    DataType *data=nullptr;     //Plain data
    bool data_owned=false;      //Flag to determine is the data is detroyed with the object
public:

    Image(const Size2D &i_size)                      //Initilization from frame, data owned
        : size(i_size),bsize(i_size.w*i_size.h),
          data(new DataType[i_size.w*i_size.h]),data_owned(true){}

    Image(DataType* i_data,const Size2D &i_size)     //Inialization from data, not owned
        : size(i_size),bsize(i_size.w*i_size.h),
          data(i_data),data_owned(false){}

    Image(const Image &img)                      //Initilization from other image, data not owned
        : size(img.size),bsize(img.bsize),
          data(img.data),data_owned(false){}


    ~Image(){
        if(data_owned && data!=nullptr)
            delete [] data;
    }

    void SetOwn(bool isOwn){data_owned=isOwn;}


    //Image indexing functions
    DataType & operator [](const uint inx){
#ifdef SAFE_IMAGE_ACCESS
        if(inx>=bsize) throw std::out_of_range("Image index error");
#endif
        return data[inx];
    }

    DataType & operator [](const uint inx) const {
#ifdef SAFE_IMAGE_ACCESS
        if(inx>=bsize) throw std::out_of_range("Image index error");
#endif
        return data[inx];
    }

    //Just get linear index
    uint GetIndex(const uint x,const uint y){
#ifdef SAFE_IMAGE_ACCESS
        if(x>=size.w || y>=size.h) throw std::out_of_range("Image index error");
#endif
        return y*size.w+x;
    }

    //Round real value position and check image boundaries
    inline int GetIndexRC(const float x,const float y){
        const int xi=round(x),yi=round(y);
        if(xi>=size.w || yi>=size.h || xi<0 || yi<0)
            return -1;
        return GetIndex(xi,yi);
    }

    DataType & operator ()(const uint x,const uint y){
        return data[GetIndex(x,y)];
    }

    //Data retreival
    DataType * Data(){return data;}
    const Size2D& Size() const {return size;}
    const uint& bSize()const {return bsize;}


    //data copy
    Image<DataType> & operator = (const Image<DataType> &img){

        if(&img==this)
            return *this;

        if(img.size.h!=size.h || img.size.w!=size.w || img.bsize!=bsize)
            throw std::length_error("Error on copy image, must be the same size!");

        memcpy(data,img.data,bsize*sizeof(DataType));
        return *this;

    }

    Image<DataType> & operator = (DataType *img){
        memcpy(data,img,bsize*sizeof(DataType));
        return *this;
    }

    void copyTo (DataType *img){
        memcpy(img,data,bsize*sizeof(DataType));
        return;
    }

    //Batch data set

    void Reset(DataType d){

        for(int i=0;i<bsize;i++)
            data[i]=d;

    }

    //Simple color convertion functions

    static void ConvertRGB2BW(Image<DataType> &to,Image<RGB24Pixel> &from){
        if(from.Size().h!=to.size.h || from.Size().w!=to.size.w || from.bSize()!=to.bsize)
            throw std::length_error("Error on convert image, must be the same size!");

        for(int i=0;i<to.bSize();i++)
            to[i]=from[i].pix.b+from[i].pix.g+from[i].pix.r;
    }

    static void ConvertBW2RGB(Image<RGB24Pixel> &to,Image<DataType> &from){
        if(from.Size().h!=to.size().h || from.Size().w!=to.size().w || from.bSize()!=to.bsize)
            throw std::length_error("Error on convert image, must be the same size!");

        for(int i=0;i<bSize();i++){
            to[i].pix.b=from[i]/3;
            to[i].pix.g=from[i]/3;
            to[i].pix.r=from[i]/3;
        }
    }


};



#endif // IMAGE_H
