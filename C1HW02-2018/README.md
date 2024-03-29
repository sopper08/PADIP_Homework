# README  

## Problem  
*more detail, please see [C1HW02-2018.pdf](./C1HW02-2018.pdf).*

* Convert a color image into a grayscale image, and display its histogram.
* Implement
  * a manual **threshold** function.
  * a function to **adjust the spatial resolution** and grayscale levels of an image.
  * a function to adjust the **brightness** and **constrast** of an image.
  * a **histogram equalization** function for automatic constrast adjustment.

## Program
### Enlarge  
![](./imgs/enlarge_equation.png)
<!-- 
\left\{\begin{matrix}
x_1 = \left \lfloor \frac{u}{s} \right \rfloor, x_2=x_1+1
\\ 
y_1 = \left \lfloor \frac{v}{s} \right \rfloor, y_2=y_1+1
\\
a = u-x_1, c=1-a
\\
b = v-x_1, d=1-b
\end{matrix}\right.
\\
f(u, y_1)=\frac{c}{a+c}f(x_1, y_1)+\frac{a}{a+c}f(x_2, y_1)=cf(x_1, y_1)+af(x_2, y_1)\\
f(u, y_2)=cf(x_1, y_2)+af(x_2, y_2)\\
\\
f(u, v)=\frac{d}{b+d}f(u, y_1)+\frac{b}{b+d}f(u, y_2)=dcf(x_1, y_1)+daf(x_2, y_1)+bcf(x_1, y_2)+baf(x_2, y_2)
 -->
```cpp
QImage enlarge(QImage &img, int scaled) {
    int width, height;
    QImage s_img;

    width = img.width();
    height = img.height();
    s_img = QImage(width*scaled, height*scaled, QImage::Format_Grayscale8);

    for (int v=0; v<s_img.height(); ++v) {
        for (int u=0; u<s_img.width(); ++u) {
            int x, y, pixel;
            float a, b, c, d;

            x = int(u/scaled); y = int(v/scaled);
            a = float(u)/float(scaled)-x; c = 1-a;
            b = float(v)/float(scaled)-y; d = 1-b;

            if(((x+1)==width)||((y+1)==height)) {
                pixel = 0;
            }
            else {
                pixel = c*d*qGray(img.pixel(  x,   y)) +
                        a*d*qGray(img.pixel(x+1,   y)) +
                        c*b*qGray(img.pixel(  x, y+1)) +
                        a*b*qGray(img.pixel(x+1, y+1));
            }
            s_img.setPixel(u, v, QRgb(pixel));
        }
    }
    return s_img;
}
```

### Histogram equalization  
![](./imgs/he_equation.png)
<!-- 
$$
p_r(r_k)=\frac{n_k}{MN}, k=0,1,2,...,L-1\\
s_k=T(r_k)=(L-1)\sum_{j=0}^{k}p_r(r_j)=\frac{L-1}{MN}\sum_{j=0}^{k}n_j, k=0,1,2,...,L-1\\
r_k=T^-1(s_k), k=0,1,2,...,L-1\\
$$
 -->
```cpp
QImage histogram_equalization(QImage &img) {
    int width, height, pixel_sum;
    int gray_array[256] = {0};
    int accumulate_gray_array[256] = {0};
    int pixel_table[256] = {0};
    QImage he_img;

    width = img.width();
    height = img.height();
    he_img = QImage(width, height, QImage::Format_Grayscale8);

    for(int i=0; i<height; ++i) {
        for(int j=0; j<width; ++j) {
            int pixel;
            pixel = qGray(img.pixel(j, i));
            ++gray_array[pixel];
        }
    }

    pixel_sum = width*height;
    accumulate_gray_array[0] = gray_array[0];
    pixel_table[0] = round(255.0*float(accumulate_gray_array[0])/pixel_sum);
    for(int i=1; i<256; ++i) {
        accumulate_gray_array[i] = accumulate_gray_array[i-1]+gray_array[i];
        pixel_table[i] = round(255.0*float(accumulate_gray_array[i])/pixel_sum);
    }

    for(int i=0; i<height; ++i) {
        for(int j=0; j<width; ++j) {
            int pixel;
            pixel = qGray(img.pixel(j, i));
            pixel = pixel_table[pixel];
            // cout << pixel << ": " << pixel_table[pixel] << endl;
            he_img.setPixel(j, i, qRgb(pixel, pixel, pixel));
        }
    }
    return he_img;
}
```

## Usage  
```sh
$ cd hw2_cli
$ qmake -makefile
$ make

# enlarge
$ ./hw2_cli enlarge <image_file> <ratio>

# histogram equalization
$ ./hw2_cli he <image_file>
```

## Results  
### Histogram equalization  
* before/after
    ![](./imgs/1_hq.jpg)
    ![](./imgs/2_hq.jpg)
