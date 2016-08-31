

    #include <highgui.h>  
    #include <cv.h>  
    #include <opencv2/core/core.hpp>    
    #include<opencv2/highgui/highgui.hpp>     
    #include <iostream>   
    #include <fstream>
    #define DIM1 17     //高斯滤波核　　列数
    #define DIM2 17     //高斯滤波核　行数
    #define VALUE 90   //高频图像增加的量    
    
    using namespace cv;
    using namespace std;  
    
    //需要调的参数　DIM1,DIM2　两个图像的sigma 高频图像增加的value 两个图像相加时的权重　
    /***代码的运行需要　ansHigh.txt ansLow.txt　两个存放滤波核数据的文件，这两个数据由matlab下的fspecial()生成***/
    int main(int argc, char** argv)  
   {  
    void myFilter(CvMat* A,CvMat* B,char s[]); //自定义的滤波函数
    void ImagesAdd(double alpha);              //将高通与低通结果混合的函数，参数alpha为高通图像的权重，1-alpha为低通图像的权重
    void MyMerge(char name1[],char name2[],char name3[],char mameMerge[]);
    
    if(argc!=3)
     {
      cout<<"We need two images!\n";
      return -1;
     }
    
    IplImage* imgHigh = cvLoadImage(argv[1],CV_LOAD_IMAGE_ANYCOLOR);
    IplImage* imgLow = cvLoadImage(argv[2],CV_LOAD_IMAGE_ANYCOLOR);
    
    
    /*******该if语句处理三通道图像（彩色）******/
    if(imgHigh->nChannels==3 && imgLow->nChannels==3)
    {
    IplImage* imgHigh1=cvCreateImage(cvGetSize(imgHigh),imgHigh->depth,1);
    IplImage* imgHigh2=cvCreateImage(cvGetSize(imgHigh),imgHigh->depth,1);
    IplImage* imgHigh3=cvCreateImage(cvGetSize(imgHigh),imgHigh->depth,1);
    cvSplit( imgHigh, imgHigh1, imgHigh2, imgHigh3, NULL );
    
    CvMat* AHigh1=cvCreateMat(imgHigh1->height,imgHigh1->width,CV_64FC1);   
    CvMat* AHigh2=cvCreateMat(imgHigh2->height,imgHigh2->width,CV_64FC1);
    CvMat* AHigh3=cvCreateMat(imgHigh3->height,imgHigh3->width,CV_64FC1);

    cvConvert(imgHigh1,AHigh1);        //将输入图像的IplImage*变量转换为CvMat*类型　　　　        
    cvConvert(imgHigh2,AHigh2); 
    cvConvert(imgHigh3,AHigh3); 
   
    IplImage* imgLow1=cvCreateImage(cvGetSize(imgLow),imgLow->depth,1);
    IplImage* imgLow2=cvCreateImage(cvGetSize(imgLow),imgLow->depth,1);
    IplImage* imgLow3=cvCreateImage(cvGetSize(imgLow),imgLow->depth,1);
    cvSplit( imgLow, imgLow1, imgLow2, imgLow3, NULL );
        
    CvMat* ALow1=cvCreateMat(imgLow1->height,imgLow1->width,CV_64FC1);   
    CvMat* ALow2=cvCreateMat(imgLow2->height,imgLow2->width,CV_64FC1);
    CvMat* ALow3=cvCreateMat(imgLow3->height,imgLow3->width,CV_64FC1);

    cvConvert(imgLow1,ALow1);        //将输入图像的IplImage*变量转换为CvMat*类型　　　　        
    cvConvert(imgLow2,ALow2); 
    cvConvert(imgLow3,ALow3); 
  
    double aHigh[DIM1*DIM2];    //将matlab　fspecial()函数生成的高斯滤波核矩阵赋给数组aHigh[]
    ifstream fin;
    fin.open("ansHigh.txt");
    for(int i=0;i<DIM1*DIM2;i++)
     fin>>aHigh[i];
    for(int i=0;i<DIM1*DIM2;i++)  //为得到高通滤波核，将高通滤波核矩阵的数据除中心外取反，中心值取反并加１
     {                            //详细见项目报告书上的说明
      if(i==((DIM1*DIM2-1)/2))
      aHigh[i]=1-aHigh[i];
      else
        aHigh[i]=-aHigh[i];
      }
    fin.clear();
    fin.close();

    CvMat CHigh=cvMat(DIM2,DIM1,AHigh1->type,aHigh);
    CvMat* BHigh=&CHigh;   //BHigh为高通滤波核矩阵
    
    double aLow[DIM1*DIM2];   //将matlab　fspecial()函数生成的另一个高斯滤波核矩阵赋给数组aHigh[]
    fin.open("ansLow.txt");
    for(int i=0;i<DIM1*DIM2;i++)
     fin>>aLow[i];
    fin.clear();
    fin.close();

    CvMat CLow=cvMat(DIM2,DIM1,ALow1->type,aLow);
    CvMat* BLow=&CLow;   //BLow为低通滤波核矩阵
    
    char nameHigh1[]="high1.bmp";
    char nameHigh2[]="high2.bmp";
    char nameHigh3[]="high3.bmp";
    char nameLow1[]="low1.bmp";
    char nameLow2[]="low2.bmp";
    char nameLow3[]="low3.bmp";
    
    /****三个通道的数据分别高通滤波***/
    myFilter(AHigh1,BHigh,nameHigh1);
    myFilter(AHigh2,BHigh,nameHigh2);
    myFilter(AHigh3,BHigh,nameHigh3);
    
    char nameMergeHigh[]="high.bmp";
    MyMerge(nameHigh1,nameHigh2,nameHigh3,nameMergeHigh);
    
    IplImage* initialHigh = cvLoadImage(nameMergeHigh,CV_LOAD_IMAGE_ANYCOLOR);
    IplImage* addValueHigh = cvCreateImage(cvGetSize(initialHigh),initialHigh->depth,3);
    
    cvConvertScale(initialHigh,addValueHigh,1.0,VALUE);  //将高通滤波的图像增加VALUE大小
    cvSaveImage("high.bmp",addValueHigh);
    
    /****三个通道的数据分别低通滤波***/
    myFilter(ALow1,BLow,nameLow1);
    myFilter(ALow2,BLow,nameLow2);
    myFilter(ALow3,BLow,nameLow3);
    
    char nameMergeLow[]="low.bmp";
    MyMerge(nameLow1,nameLow2,nameLow3,nameMergeLow);
    
    ImagesAdd(0.5);                  //参数为addWeighted函数的alpha，对应于高频图像的权重
    
    /****下面是将图像黑色边框内的图像取出，使得最终的图像和源图像的尺寸相同****/
    IplImage* pSrc = cvLoadImage("Result.bmp",CV_LOAD_IMAGE_ANYCOLOR);
    CvSize size= cvGetSize(imgHigh);//区域大小
    cvSetImageROI(pSrc,cvRect((pSrc->width-size.width)/2,(pSrc->height-size.height)/2,size.width, size.height)); //设置源图像ROI
    IplImage* pDest = cvCreateImage(size,pSrc->depth,pSrc->nChannels);//创建目标图像
    cvCopy(pSrc,pDest); //复制图像
    cvResetImageROI(pSrc);//源图像用完后，清空ROI
    cvSaveImage("ResultROI.bmp",pDest);//保存目标图像
    
    return 0;
    } 
     
     /*******该else if 分支处理单通道图像，如灰度图像*********/
    else if(imgHigh->nChannels==1 && imgLow->nChannels==1)
    {
     CvMat* AHigh=cvCreateMat(imgHigh->height,imgHigh->width,CV_64FC1);   //先转换为灰度图像
     cvConvert(imgHigh,AHigh);        //将输入图像转换　　　　        
    
     CvMat* ALow=cvCreateMat(imgLow->height,imgLow->width,CV_64FC1);   //先转换为灰度图像
     cvConvert(imgLow,ALow);  
    
     double aHigh[DIM1*DIM2];   //将matlab　fspecial()函数生成的高斯滤波核矩阵赋给数组aHigh[]
     ifstream fin;
     fin.open("ansHigh.txt");
     for(int i=0;i<DIM1*DIM2;i++)
     fin>>aHigh[i];
     for(int i=0;i<DIM1*DIM2;i++)  //为得到高通滤波核，将高通滤波核矩阵的数据除中心外取反，中心值取反并加１
     {                             //详细见项目报告书
      if(i==((DIM1*DIM2-1)/2))
      aHigh[i]=1-aHigh[i];
      else
        aHigh[i]=-aHigh[i];
      }
     fin.clear();
     fin.close();
     CvMat CHigh=cvMat(DIM2,DIM1,AHigh->type,aHigh);
     CvMat* BHigh=&CHigh;
   
     double aLow[DIM1*DIM2];     //将matlab　fspecial()函数生成的另一个高斯滤波核矩阵赋给数组aHigh[]
     fin.open("ansLow.txt");
     for(int i=0;i<DIM1*DIM2;i++)
      fin>>aLow[i];
     fin.clear();
     fin.close();
     CvMat CLow=cvMat(DIM2,DIM1,ALow->type,aLow);
     CvMat* BLow=&CLow;

     char nameHigh[]="high.bmp";
     char nameLow[]="low.bmp";

     myFilter(AHigh,BHigh,nameHigh);
     myFilter(ALow,BLow,nameLow);
    
     IplImage* initialHigh = cvLoadImage(nameHigh,CV_LOAD_IMAGE_ANYCOLOR);
     IplImage* addValueHigh = cvCreateImage(cvGetSize(initialHigh),initialHigh->depth,1);
    
     cvConvertScale(initialHigh,addValueHigh,1.0,VALUE);
     cvSaveImage("high.bmp",addValueHigh);
    
     ImagesAdd(0.5);                  //参数为addWeighted函数的alpha，对应于高频图像的权重

     /****同三通道图像的处理，下面是将图像黑色边框内的图像取出，使得最终的图像和源图像的尺寸相同****/
     IplImage* pSrc = cvLoadImage("Result.bmp",CV_LOAD_IMAGE_ANYCOLOR);
     CvSize size= cvGetSize(imgHigh);//区域大小
     cvSetImageROI(pSrc,cvRect((pSrc->width-size.width)/2,(pSrc->height-size.height)/2,size.width, size.height)); //设置源图像ROI
     IplImage* pDest = cvCreateImage(size,pSrc->depth,pSrc->nChannels);//创建目标图像
     cvCopy(pSrc,pDest); //复制图像
     cvResetImageROI(pSrc);//源图像用完后，清空ROI
     cvSaveImage("ResultROI.bmp",pDest);//保存目标图像
    
     return 0;  
    } 
     
    else
     {
      cout<<"ERROR! We can only tackle images with 1 or 3 channels!\n";
      return -1;
     }
     
   } 
    
    
    /********自定义的滤波函数，频域操作**********/
    void myFilter(CvMat* A,CvMat* B,char s[])   
   {   
    //创建矩阵存放卷积结果have only abs(M2-M1)+1×abs(N2-N1)+1 part of the full convolution result  
    CvMat* convolution = cvCreateMat( A -> rows + B -> rows - 1,  
        A -> cols + B -> cols - 1, A -> type );  
    int dft_M = cvGetOptimalDFTSize( A -> rows + B -> rows - 1 );  //对于给定的矢量尺寸返回最优DFT尺寸  
    int dft_N = cvGetOptimalDFTSize( A -> cols + B -> cols - 1 );  
    CvMat* dft_A = cvCreateMat( dft_M, dft_N, A -> type );  
    CvMat* dft_B = cvCreateMat( dft_M, dft_N, B -> type );  
    CvMat tmp;  
  
    // copy A to dft_A and pad dft_A with zeros  
    cvGetSubRect( dft_A, &tmp, cvRect( 0, 0, A -> cols, A -> rows ) );  //返回输入的图像或矩阵的矩形数组子集的矩阵头   
    cvCopy( A, &tmp );  
    cvGetSubRect( dft_A, &tmp, cvRect( A -> cols, 0, dft_A -> cols - A -> cols, A -> rows ) );  
    cvZero( &tmp );  
 
    // no need to pad bottom part of dft_A with zeros because of  
    // use nonzero_rows parameter in cvDFT() call below  
    cvDFT( dft_A, dft_A, CV_DXT_FORWARD, A -> rows );  //执行一维或者二维浮点数组的离散傅立叶正变换或者离散傅立叶逆变换  
   
    // repeat the same with the second array B  
   
    cvGetSubRect( dft_B, &tmp, cvRect( 0, 0, B -> cols, B -> rows ) );  //返回输入的图像或矩阵的矩形数组子集的矩阵头   
    cvCopy( B, &tmp );  
    cvGetSubRect( dft_B, &tmp, cvRect( B -> cols, 0, dft_B -> cols - B -> cols, B -> rows ) );  
    cvZero( &tmp );  
    cvDFT( dft_B, dft_B, CV_DXT_FORWARD, B -> rows );  
  
    cvMulSpectrums( dft_A, dft_B, dft_A, 0 );  //两个傅立叶频谱的每个元素的乘法  
  
    cvDFT( dft_A, dft_A, CV_DXT_INV_SCALE, convolution -> rows );  
    cvGetSubRect( dft_A, &tmp, cvRect( 0, 0, convolution -> cols, convolution -> rows ) );  
    cvCopy( &tmp, convolution );  
  
    cvSaveImage(s,convolution);
    cvReleaseMat( &dft_A );  
    cvReleaseMat( &dft_B ); 
   }
       
       
       
       
    /*******将高通与低通结果混合，alpha为高频图像的系数******/   
    void ImagesAdd(double alpha)  
   {
    double beta;  
      
    IplImage* src1 = cvLoadImage("high.bmp",CV_LOAD_IMAGE_COLOR);  
    IplImage* src2 = cvLoadImage("low.bmp",CV_LOAD_IMAGE_COLOR);  
    IplImage* dst=cvCreateImage(cvGetSize(src1),src1->depth,3);
    
    beta = (1.0 - alpha);  
    cvAddWeighted(src1, alpha, src2, beta, 0.0, dst); //这里调用了addWeighted函数，得到的结果存储在dst中  
      
    cvSaveImage("Result.bmp",dst);
   
    cvNamedWindow("Result", CV_WINDOW_AUTOSIZE);
    cvShowImage("Result", dst);
    cvWaitKey(0);
    cvReleaseImage(&dst);
    cvDestroyWindow("Result");
   }
    
    
    
    
    /******将三个单通道图像merge为单个三通道图像*******/
    void MyMerge(char name1[],char name2[],char name3[],char nameMerge[])  
   {
     IplImage* filterResult1 = cvLoadImage(name1,CV_LOAD_IMAGE_ANYCOLOR);
     IplImage* filterResult2 = cvLoadImage(name2,CV_LOAD_IMAGE_ANYCOLOR);
     IplImage* filterResult3 = cvLoadImage(name3,CV_LOAD_IMAGE_ANYCOLOR);
     
     IplImage* filterMerge = cvLoadImage(name3,CV_LOAD_IMAGE_COLOR);
     cvMerge(filterResult1,filterResult2,filterResult3,NULL,filterMerge);
     cvSaveImage(nameMerge,filterMerge);
   }
    
