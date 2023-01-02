//#include <SDL.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <jsoncpp/json/json.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <ctime>
#include <thread>
#include <unistd.h> //UNIX standard function definitions
#include <fcntl.h>  //File control definitions
#include <errno.h>  //Error number definitions
#include <termios.h>
#include <string.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <stdlib.h>
using namespace cv;
using namespace std;

const int SCREEN_WIDTH = 3840;
const int SCREEN_HEIGHT = 2160;
const float INTERVAL_CONST = 0.04; //= 0.04;
const int fps = 24;

//function to convert an openCV Mat to a SDL_Texture
SDL_Texture *TexFromCV(const Mat &mat, SDL_Renderer *renderer);

int main(int argc, char *args[])
{
  
  float min = 100;
  float max = 0;

  struct termios tty;
  struct termios tty_old;
  memset(&tty, 0, sizeof tty);



  /* Save old tty parameters */
  tty_old = tty;

  /* Set Baud Rate */
  cfsetospeed(&tty, (speed_t)B9600);
  cfsetispeed(&tty, (speed_t)B9600);

  /* Setting other Port Stuff */
  tty.c_cflag &= ~PARENB; // Make 8n1
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  tty.c_cflag &= ~CRTSCTS;       // no flow control
  tty.c_cc[VMIN] = 1;            // read doesn't block
  tty.c_cc[VTIME] = 5;           // 0.5 seconds read timeout
  tty.c_cflag |= CREAD | CLOCAL; // turn on READ & ignore ctrl lines

  /* Make raw */
  cfmakeraw(&tty);




  int n = 0,
      spot = 0;
  char buf = '\0';

  /* Whole response*/
  char response[1024];
  memset(response, '\0', sizeof response);

  int aveWindow = 3;
  int rollingAverage[aveWindow];
  rollingAverage[0] = 82;
  rollingAverage[1] = 82;
  rollingAverage[2] = 82;
  int scriptPos = 0;
  int data_index = 0;
  bool increasing = true; //false for dec

  int averageCount = 0;
  int lf_count = 0;
  float MAX_INTERVAL = 15;
  float audio = 0;
  float audioSpeed = 0;
  char charAudio[10];
  char *audioCommand = (char *)malloc(100 * sizeof(char));
  float interval = 0;
  SDL_Window *window = NULL;
  SDL_Surface *screenSurface = NULL;
  SDL_Renderer *renderer = NULL;

  FILE *bluetoothFile;
  char crankValueStr[3] = "";

  //initialize SDL Things
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    printf("SDL_ERROR: %s\n", SDL_GetError());
  window = SDL_CreateWindow("HDRTZ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);

  if (window == NULL)
    printf("Window not created! SDL_ERROR: %s\n", SDL_GetError());
  else
  {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
      printf("Surface not created! SDL_ERROR: %s\n", SDL_GetError());
    else
    {
      SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
  }
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
    printf("SDL Image not initialized");
  SDL_Surface *maskSurf = IMG_Load("test.png");
  SDL_Texture *maskTex = SDL_CreateTextureFromSurface(renderer, maskSurf);
  SDL_Surface *xSurf = IMG_Load("xhair.png");
  SDL_Texture *xTex = SDL_CreateTextureFromSurface(renderer, xSurf);

  //Initialize openCV camera and set up image capture
  VideoCapture cap;
  cap.open(2);
  cap.set(CAP_PROP_FRAME_WIDTH, 1920);
  cap.set(CAP_PROP_FRAME_HEIGHT, 1080);
  cap.set(CAP_PROP_FPS, fps);
  cout << "setting fps supported" << endl;
  if (!cap.isOpened())
  {
    cout << "error opening stream" << endl;
    return -1;
  }

  SDL_Point p;
  double angle = 0;
  SDL_Event e;
  bool quit = false;

  clock_t startTime, currentTime;
  Uint32 start_tick;

  while (!quit)
  {
    std::ifstream coords("output.json", std::ifstream::binary);
    //Handle the input from the website
    ifstream ifs("output.json");
    Json::Reader reader;
    Json::Value obj;
    bool errs;
    

    errs = reader.parse(coords,obj,false);
    cout << obj;
    int crosshair;
    int mask;
    float scaleFactor = 0.5 + 0.5 * obj["zoom"].asInt() / 100;

    //Resize the capture to be the size of the screen
    SDL_Rect videostream;

    videostream.w = 3840;
    videostream.h = 2160;

    videostream.x = obj["xPos"].asInt();
    p.x = 0;
    videostream.y = obj["yPos"].asInt();
    p.y = 0;
    crosshair = obj["crosshair"].asInt();
    mask = obj["mask"].asInt();


    
    while (SDL_PollEvent(&e) != 0)
    {
      //User requests quit
      if (e.type == SDL_QUIT)
      {
        quit = true;
      }

      //Debug features for testing without crank
      else if (e.type == SDL_KEYDOWN)
      {
        switch (e.key.keysym.sym)
        {
        case SDLK_RIGHT:
          cout << "Hello" << endl;
          interval -= .5;

          break;

        case SDLK_LEFT:
          interval += .5;
          break;
        case SDLK_UP:
          std::cout << interval << std::endl;
          break;
        case SDLK_DOWN:
          quit = true;
          return 0;
        default:
          break;
        }
      }
    }

    // do
    // {
    //   n = read(USB, &buf, 1);
    //   sprintf(&response[spot], "%c", buf);
    //   spot += n;
    // } while (buf != '\r' && n > 0);

    // tcflush(USB, TCIFLUSH);

    // if (n < 0)
    // {
    //   std::cout << "Error reading: " << strerror(errno) << std::endl;
    //   //interval = 0;
    // }
    // else if (n == 0)
    // {
    //   std::cout << "Read nothing!" << std::endl;
    // }

   //handle crank data
    bluetoothFile = fopen("bluetooth_output.txt", "r");
    fscanf(bluetoothFile, "%s", crankValueStr);
    fclose(bluetoothFile);
    int raw_crank_data = atoi(crankValueStr);
    std::cout << "rawCrankVal = " << raw_crank_data << " interval = " << interval << "\n";
    spot = 0;
    Mat frame;
    SDL_Texture *tex = NULL;

    //get the latest frame from openCV
    cap >> frame;
    waitKey(1);

    //convert that frame to a texture
    tex = TexFromCV(frame, renderer);

    if (tex != NULL)
    {
      //Render the frame if it exists
      SDL_RenderClear(renderer);
      SDL_RenderCopyEx(renderer, tex, NULL, &videostream, angle, NULL, SDL_FLIP_NONE);

      //SDL_RenderCopyEx(renderer, tex, NULL, &videostream, angle, &p, SDL_FLIP_NONE);
      SDL_RenderSetScale(renderer, scaleFactor, scaleFactor);
      cout << "mask: " << mask << " crosshair: " << crosshair << endl;

      //render the image mask
      if (mask == 1)
      {
        SDL_RenderCopy(renderer, maskTex, NULL, NULL);

      }

      //render the crosshair
      if (crosshair == 1)
      {
        SDL_RenderCopy(renderer, xTex, NULL, NULL);
      }

      //present the texutre and prepare for the next frae
      SDL_RenderPresent(renderer);
      SDL_DestroyTexture(tex);

      if (raw_crank_data > 180){
        interval = -1*(256-raw_crank_data);
      }
      else{
        interval = raw_crank_data;
      }

      if (interval > MAX_INTERVAL){
        interval = MAX_INTERVAL;
      }
      else if (interval < -1*MAX_INTERVAL){
        interval = -1*MAX_INTERVAL;
      }
      
      angle += interval;



      audio = interval / (MAX_INTERVAL);
      ofstream outputFile("/tmp/pitch");


      outputFile << to_string(audio);
      outputFile.close();
    }

  }

}

//Function to convert openCV mat to SDL_Texture
SDL_Texture *TexFromCV(const Mat &mat, SDL_Renderer *renderer)
{

  IplImage cvFrame = cvIplImage(mat);
  IplImage *cvFramePtr = &cvFrame;

  //create a surface first, luckily IplImage and surface seem to play nice
  SDL_Surface *frameData = SDL_CreateRGBSurfaceFrom(
      (void *)cvFramePtr->imageData,
      cvFramePtr->width, cvFramePtr->height,
      cvFramePtr->depth * cvFramePtr->nChannels,
      cvFramePtr->widthStep,
      0xff0000, 0x00ff00, 0x0000ff, 0);

  if (frameData == NULL)
  {
    printf("Failed to convert Mat to Surface");
    return NULL;
  }

  //if the mat converts to surface nicely, then it will convert the surface to a texture
  SDL_Texture *frameTex = NULL;
  frameTex = SDL_CreateTextureFromSurface(renderer, frameData);

  //  SDL_FreeSurface(frameData);
  if (frameTex == NULL)
    return NULL;
  else
  {
    SDL_FreeSurface(frameData);

   
    return frameTex;
  }
}
