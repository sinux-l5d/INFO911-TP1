#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

// Fonction qui prend une image et renvoie un tableau représentant un histogramme
double *histo(Mat f)
{
  double *hist = new double[256];
  double nbPixel = f.rows * f.cols;
  for (int i = 0; i < 256; i++)
  {
    hist[i] = 0;
  }
  for (int i = 0; i < f.rows; i++)
  {
    for (int j = 0; j < f.cols; j++)
    {
      hist[f.at<uchar>(i, j)] += 1.0 / nbPixel;
    }
  }
  return hist;
}

double *histoCumule(double *hist)
{
  double *histCumule = new double[256];
  histCumule[0] = hist[0];
  for (int i = 1; i < 256; i++)
  {
    histCumule[i] = histCumule[i - 1] + hist[i];
  }
  return histCumule;
}

Mat histoToMat(double *hist)
{
  double max = *std::max_element(hist, hist + 256);
  Mat histView = Mat::zeros(256, 256, CV_8UC1);
  for (int i = 0; i < 256; i++)
  {
    line(histView, Point(i, 255), Point(i, 255 - (hist[i] / max) * 255), 255);
  }
  return histView;
}

Mat histogrammes(Mat f)
{
  double *hist = histo(f);
  double *histCumule = histoCumule(hist);
  Mat histView = histoToMat(hist);
  Mat histCumuleView = histoToMat(histCumule);
  Mat histView2 = Mat::zeros(256, 512, CV_8UC1);
  histView.copyTo(histView2(Rect(0, 0, 256, 256)));
  histCumuleView.copyTo(histView2(Rect(256, 0, 256, 256)));
  return histView2;
}

void egalise(Mat f)
{
  double *H = histoCumule(histo(f));
  f.forEach<uchar>([&](uchar &p, const int *position) -> void
                   { p = H[p] * 255; });
}

void egaliseCouleur(Mat f)
{
  Mat hsv;
  cvtColor(f, hsv, COLOR_BGR2HSV);
  std::vector<Mat> hsvChannels;
  split(hsv, hsvChannels);
  Mat v = hsvChannels[2];
  double *H = histoCumule(histo(v));
  v.forEach<uchar>([&](uchar &p, const int *position) -> void
                   { p = H[p] * 255; });
  hsvChannels[2] = v;
  merge(hsvChannels, hsv);
  cvtColor(hsv, f, COLOR_HSV2BGR);
}

Mat tramage_floyd_steinberg_grayscale(Mat input)
{
  Mat output;
  input.convertTo(output, CV_32FC1);

  for (int y = 0; y < output.rows; y++)
  {
    for (int x = 0; x < output.cols; x++)
    {
      float oldPixel = output.at<float>(y, x);
      float newPixel = round(oldPixel / 255) * 255;
      output.at<float>(y, x) = newPixel;
      float error = oldPixel - newPixel;
      if (x < output.cols - 1)
      {
        output.at<float>(y, x + 1) += error * 7 / 16;
      }
      if (x > 0 && y < output.rows - 1)
      {
        output.at<float>(y + 1, x - 1) += error * 3 / 16;
      }
      if (y < output.rows - 1)
      {
        output.at<float>(y + 1, x) += error * 5 / 16;
      }
      if (x < output.cols - 1 && y < output.rows - 1)
      {
        output.at<float>(y + 1, x + 1) += error * 1 / 16;
      }
    }
  }

  output.convertTo(output, CV_8UC1);
  return output;
}

Mat tramage_floyd_steinberg_BGR(Mat input)
{
  Mat output;
  input.convertTo(output, CV_32FC3); // Conversion en virgule flottante pour des valeurs de couleur

  for (int y = 0; y < output.rows; y++)
  {
    for (int x = 0; x < output.cols; x++)
    {
      cv::Vec3f oldPixel = output.at<cv::Vec3f>(y, x); // Récupération du pixel en couleur (trois canaux)
      cv::Vec3f newPixel;

      for (int c = 0; c < 3; c++)
      { // Boucle sur chaque canal de couleur
        float channelValue = oldPixel[c];
        float newChannelValue = round(channelValue / 255) * 255; // Arrondi à la valeur la plus proche
        newPixel[c] = newChannelValue;                           // Mise à jour du canal

        float error = channelValue - newChannelValue;

        if (x < output.cols - 1)
        {
          output.at<cv::Vec3f>(y, x + 1)[c] += error * 7 / 16; // Erreur propagée à droite
        }
        if (x > 0 && y < output.rows - 1)
        {
          output.at<cv::Vec3f>(y + 1, x - 1)[c] += error * 3 / 16; // Erreur propagée en bas à gauche
        }
        if (y < output.rows - 1)
        {
          output.at<cv::Vec3f>(y + 1, x)[c] += error * 5 / 16; // Erreur propagée en bas
        }
        if (x < output.cols - 1 && y < output.rows - 1)
        {
          output.at<cv::Vec3f>(y + 1, x + 1)[c] += error * 1 / 16; // Erreur propagée en bas à droite
        }
      }

      output.at<cv::Vec3f>(y, x) = newPixel; // Mettre à jour le pixel avec les nouvelles valeurs
    }
  }

  output.convertTo(output, CV_8UC3); // Conversion en image 8 bits par canal
  return output;
}

float distance_color_l2(Vec3f bgr1, Vec3f bgr2)
{
  // Calcule la distance L2 (Euclidienne) entre deux couleurs
  float distance = 0.0;
  for (int i = 0; i < 3; ++i)
  {
    float diff = bgr1[i] - bgr2[i];
    distance += diff * diff;
  }
  return std::sqrt(distance);
}

int best_color(Vec3f bgr, std::vector<cv::Vec3f> colors)
{
  // Trouve la couleur la plus proche dans le vecteur de couleurs donné
  int bestIndex = 0;
  float minDistance = distance_color_l2(bgr, colors[0]);

  for (int i = 1; i < colors.size(); ++i)
  {
    float distance = distance_color_l2(bgr, colors[i]);
    if (distance < minDistance)
    {
      minDistance = distance;
      bestIndex = i;
    }
  }

  return bestIndex;
}

Vec3f error_color(Vec3f bgr1, Vec3f bgr2)
{
  // Calcule le vecteur erreur entre deux couleurs
  Vec3f error;
  for (int i = 0; i < 3; ++i)
  {
    error[i] = bgr1[i] - bgr2[i];
  }
  return error;
}

Mat tramage_floyd_steinberg(Mat input, std::vector<Vec3f> colors)
{
  // Conversion de input en une matrice de 3 canaux flottants
  cv::Mat fs;
  input.convertTo(fs, CV_32FC3, 1.0 / 255.0);

  int height = fs.rows;
  int width = fs.cols;

  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      cv::Vec3f c = fs.at<cv::Vec3f>(y, x);
      int i = best_color(c, colors);
      cv::Vec3f e = error_color(c, colors[i]);
      fs.at<cv::Vec3f>(y, x) = colors[i]; // Affecte la couleur la plus proche

      // Propage l'erreur aux pixels voisins
      if (x + 1 < width)
        fs.at<cv::Vec3f>(y, x + 1) = fs.at<cv::Vec3f>(y, x + 1) + (7.0 / 16.0) * e;
      if (x > 0 && y + 1 < height)
        fs.at<cv::Vec3f>(y + 1, x - 1) = fs.at<cv::Vec3f>(y + 1, x - 1) + (3.0 / 16.0) * e;
      if (y + 1 < height)
        fs.at<cv::Vec3f>(y + 1, x) = fs.at<cv::Vec3f>(y + 1, x) + (5.0 / 16.0) * e;
      if (x + 1 < width && y + 1 < height)
        fs.at<cv::Vec3f>(y + 1, x + 1) = fs.at<cv::Vec3f>(y + 1, x + 1) + (1.0 / 16.0) * e;
    }
  }

  // On reconvertit la matrice de 3 canaux flottants en BGR
  cv::Mat output;
  fs.convertTo(output, CV_8UC3, 255.0);
  return output;
}

int img(int argc, char **argv)
{
  namedWindow("TP1");             // crée une fenêtre
  Mat fOrigine = imread(argv[1]); // lit l'image en niveau de gris
  Mat f = fOrigine.clone();
  // egalise(f, histoCumule(histo(f)));
  imshow("TP1", f); // l'affiche dans la fenêtre
  // affiche l'histogramme dans une autre fenêtre
  namedWindow("Histogramme");
  imshow("Histogramme", histogrammes(f));
  int key;
  // unless q pressed
  while ((key = waitKey(50)) != 113)
  {
    switch (key & 0xff)
    {
    case 'e': // (e)galise
      f.type() == CV_8UC1 ? egalise(f) : egaliseCouleur(f);
      break;
    case 'r': // (r)eset
      f = fOrigine.clone();
      std::cout << "reset" << std::endl;
      break;
    case 's': // (s)witch between BGR and greyscale (and reset)
      if (f.type() == CV_8UC1)
        f = fOrigine.clone();
      else
        cvtColor(fOrigine, f, COLOR_BGR2GRAY);
      break;
    case 't':                         // (t)ramage
      if (waitKey(200) & 0xff == 't') // tt (CMYK using generic)
      {
        std::vector<cv::Vec3f> cmyk = {
            {1.0, 1.0, 0},
            {0, 1.0, 1.0},
            {1.0, 0, 1.0},
            {0, 0, 0},
            {1.0, 1.0, 1.0}, // paper white
        };
        f = f.channels() != 1 ? tramage_floyd_steinberg(f, cmyk) : f;
      }
      else
      {
        f = f.channels() == 1 ? f = tramage_floyd_steinberg_grayscale(f) : tramage_floyd_steinberg_BGR(f);
      }
      break;
    default:
      if (key != -1)
        std::cout << "key pressed: " << key << std::endl;
      continue; // don't update each time
    }
    imshow("TP1", f);
    imshow("Histogramme", histogrammes(f));
  }
  return 0;
}

int cam(int argc, char **argv)
{
  VideoCapture cap(0);
  if (!cap.isOpened())
    return 1;
  Mat frame, edges;
  namedWindow("edges", WINDOW_AUTOSIZE);
  for (;;)
  {
    cap >> frame;
    cvtColor(frame, edges, COLOR_BGR2GRAY);
    edges = tramage_floyd_steinberg_grayscale(edges); // exemple
    imshow("edges", edges);
    int key_code = waitKey(30);
    int ascii_code = key_code & 0xff;
    if (ascii_code == 'q')
      break;
  }
  return 0;
}

int main(int argc, char **argv)
{
  // two subcommands: img and cam
  if (argc > 1 && strcmp(argv[1], "img") == 0)
    return img(argc - 1, argv + 1);
  else if (argc > 1 && strcmp(argv[1], "cam") == 0)
    return cam(argc - 1, argv + 1);
  else
    std::cout << "usage: " << argv[0] << " img|cam [args]" << std::endl;
  return 1;
}