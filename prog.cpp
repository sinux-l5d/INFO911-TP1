#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

// Fonction qui prend une image et renvoie un tableau représentant un histogramme
double* histo(Mat f) {
  double *hist = new double[256];
  double nbPixel = f.rows * f.cols;
  for (int i = 0; i < 256; i++) {
    hist[i] = 0;
  }
  for (int i = 0; i < f.rows; i++) {
    for (int j = 0; j < f.cols; j++) {
      hist[f.at<uchar>(i, j)] += 1.0 / nbPixel;
    }
  }
  return hist;
}

Mat histoToMat(double *hist) {
  double max = *std::max_element(hist, hist + 256);
  Mat histView = Mat::zeros(256, 256, CV_8UC1);
  for (int i = 0; i < 256; i++) {
    line(histView, Point(i, 255), Point(i, 255 - (hist[i] / max)*255), 255);
  }
  return histView;
}


int main(int argc, char **argv) {
  namedWindow("TP1");                        // crée une fenêtre
  Mat f = imread(argv[1], IMREAD_GRAYSCALE); // lit l'image
  imshow("TP1", f);                          // l'affiche dans la fenêtre
  // affiche l'histogramme dans une autre fenêtre
  namedWindow("Histogramme");
  double *h = histo(f);
  // affiche l'histogramme
  std::cout << "\nHistogramme :" << std::endl;
  for (int i = 0; i < 256; i++) {
    std::cout << h[i] << " ";
  }
  std::cout << std::endl;

  Mat histView = histoToMat(h);
  imshow("Histogramme", histView);
  while (waitKey(50) < 0) {}
}