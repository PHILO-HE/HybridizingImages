Hybridizing Low-frequency Image and High-frequency Image with OpenCV

1. 对彩色图像，将三个通道分别进行傅里叶变换，在频域与高通或低通滤波核相乘，再反变换实现滤波。然后将三个通道的结果合并到一幅图像上，得到一幅图像的高通或低通滤波结果。最后将两幅高频和低频图像以0.5的权重加在一起合成一幅图像。
     
2. 由于OpenCV中没有产生滤波核的函数，我利用MatLab中的fspecial()产生滤波核，将生成的高斯滤波矩阵数据存放在txt文件中，来用于OpenCV中。
  
3. 若要运行，需要有保存高斯滤波核矩阵的两个文件ansHigh.txt ansLow.txt，分别用于高通与低通
