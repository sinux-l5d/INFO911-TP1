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
  // createTrackbar( "track", "TP1", nullptr, 255, nullptr);
  // setTrackbarPos( "track", "TP1", value );
  Mat f = imread(argv[1]);        // lit l'image "lena.png"
  // Si l'image n'est pas en niveau de gris, on la convertit
  if (f.type() == CV_8UC3) cvtColor( f, f, COLOR_BGR2GRAY );
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
