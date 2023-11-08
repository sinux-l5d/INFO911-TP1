#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
 
int main( int argc, char** argv ) 
{
  int old_value = 0;
  int value = 128;
  namedWindow( "TP1");               // crée une fenêtre
  createTrackbar( "track", "TP1", &value, 255, NULL); // un slider
  Mat f = imread(argv[1], IMREAD_GRAYSCALE);        // lit l'image
  imshow( "TP1", f );                // l'affiche dans la fenêtre
  while ( waitKey(50) < 0 )          // attend une touche
  { // Affiche la valeur du slider
    if ( value != old_value )
    {
      old_value = value;
      // int new_value = getTrackbarPos( "track", "TP1" );
      std::cout << "value=" << value << std::endl;
    }
  }
} 
