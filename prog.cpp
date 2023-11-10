#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

int main(int argc, char **argv) {
  namedWindow("TP1");                        // crée une fenêtre
  Mat f = imread(argv[1], IMREAD_GRAYSCALE); // lit l'image
  imshow("TP1", f);                          // l'affiche dans la fenêtre
  // affiche l'histogramme
  while (waitKey(50) < 0) {}
}

// Fonction qui prend une image et renvoie un tableau représentatn un histogramme
double* histo(Mat f) {
  double *hist = new double[256];
  int nbPixel = f.rows * f.cols;
  for (int i = 0; i < 256; i++) {
    hist[i] = 0;
  }
  for (int i = 0; i < f.rows; i++) {
    for (int j = 0; j < f.cols; j++) {
      hist[f.at<uchar>(i, j)] = 1.0 / nbPixel;
    }
  }
  return hist;
}



