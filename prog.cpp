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

double* histoCumule(double *hist) {
  double *histCumule = new double[256];
  histCumule[0] = hist[0];
  for (int i = 1; i < 256; i++) {
    histCumule[i] = histCumule[i - 1] + hist[i];
  }
  return histCumule;
}

Mat histoToMat(double *hist) {
  double max = *std::max_element(hist, hist + 256);
  Mat histView = Mat::zeros(256, 256, CV_8UC1);
  for (int i = 0; i < 256; i++) {
    line(histView, Point(i, 255), Point(i, 255 - (hist[i] / max)*255), 255);
  }
  return histView;
}

Mat histogrammes(Mat f) {
  double *hist = histo(f);
  double *histCumule = histoCumule(hist);
  Mat histView = histoToMat(hist);
  Mat histCumuleView = histoToMat(histCumule);
  Mat histView2 = Mat::zeros(256, 512, CV_8UC1);
  histView.copyTo(histView2(Rect(0, 0, 256, 256)));
  histCumuleView.copyTo(histView2(Rect(256, 0, 256, 256)));
  return histView2;
}

void egalise(Mat f, double* H) {
  f.forEach<uchar>([&](uchar &p, const int *position) -> void {
    p = H[p] * 255;
  });
}

int main(int argc, char **argv) {
  namedWindow("TP1");                        // crée une fenêtre
  Mat fOrigine = imread(argv[1], IMREAD_GRAYSCALE); // lit l'image en niveau de gris
  Mat f = fOrigine.clone();
  // egalise(f, histoCumule(histo(f)));
  imshow("TP1", f);                          // l'affiche dans la fenêtre
  // affiche l'histogramme dans une autre fenêtre
  namedWindow("Histogramme");
  imshow("Histogramme", histogrammes(f));
  int key;
  // unless q pressed
  while ((key = waitKey(50)) != 113) {
    switch (key)
    {
    case 101: // e
      egalise(f, histoCumule(histo(f)));
      std::cout << "egalise" << std::endl;
      imshow("TP1", f);
      imshow("Histogramme", histogrammes(f));
      break;
    case 114:
      f = fOrigine.clone();
      std::cout << "reset" << std::endl;
      imshow("TP1", f);
      imshow("Histogramme", histogrammes(f));
      break;
    default:
      if (key != -1) std::cout << "key pressed: " << key << std::endl;
      break;
    }
  }
}