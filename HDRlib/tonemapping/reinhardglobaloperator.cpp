#include "reinhardglobaloperator.h"

ReinhardGlobalOperator::ReinhardGlobalOperator(HDRImage *image)
    :ToneMapping(image)
{

}

LDRImage * ReinhardGlobalOperator::process(){
    double count = image->getMat().cols * image->getMat().rows;
    double delta = 0.01;
    double Lw[image->getMat().channels()];
    memset(Lw,0,image->getMat().channels());

    int width = image->getMat().cols;
    int height = image->getMat().rows;
    int channels = image->getMat().channels();
    for(int x = 0; x < width;x++){
        for(int y = 0; y < height;y++){
            for(int ch = 0; ch < channels;ch++){
                Lw[ch] += log(image->getMat().at<Vec3f>(y,x)[ch] + delta);
                //cout << Lw[ch] << endl;
            }
        }
    }
    for(int i = 0; i < 3; i++){
        cout << Lw[i] << " " <<  1/count *Lw[i] << endl;
    }
    return NULL;
}

ReinhardGlobalOperator::~ReinhardGlobalOperator()
{

}

