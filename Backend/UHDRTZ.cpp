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

void setTermPorts() {

    /* creates a general terminal interface that is provided to control asynchronous communications ports*/
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
}

void initSDL(SDL_Renderer *renderer) {
    SDL_Window *window = NULL;
    SDL_Surface *screenSurface = NULL;

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
}

void openCam(VideoCapture cap) {
    //Initialize openCV camera and set up image capture
    for (int i = 1; i <= 5; i++) {
        cap.open(i);
        cap.set(CAP_PROP_FRAME_WIDTH, SCREEN_WIDTH);
        cap.set(CAP_PROP_FRAME_HEIGHT, SCREEN_HEIGHT);
        cap.set(CAP_PROP_FPS, fps);
        cout << "setting fps supported" << endl;
        if (!cap.isOpened() && (i == 5))
        {
            cout << "error opening stream" << endl;
            i++;
            exit;
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

int main(int argc, char *args[])
{
  
    setTermPorts();

    SDL_Renderer *renderer = NULL;
    initSDL(renderer);
    SDL_Surface *maskSurf = IMG_Load("test.png");
    SDL_Texture *maskTex = SDL_CreateTextureFromSurface(renderer, maskSurf);
    SDL_Surface *xSurf = IMG_Load("xhair.png");
    SDL_Texture *xTex = SDL_CreateTextureFromSurface(renderer, xSurf);

    VideoCapture cap;
    openCam(cap);
    
    /* Reading from json and running program */
    float MAX_INTERVAL = 15;
    float audio = 0;
    float interval = 0;
    FILE *bluetoothFile;
    char crankValueStr[3] = "";
    SDL_Point p;
    double angle = 0;
    SDL_Event e;
    bool quit = false;

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
        SDL_Rect videostream; // ***** WHY IS THIS HERE? SHOULD IT NOT BE AFTER SETTING ALL THE VIDEOSTREAM VALUES??

        videostream.w = SCREEN_WIDTH;
        videostream.h = SCREEN_HEIGHT;

        videostream.x = obj["xPos"].asInt();
        p.x = 0;
        videostream.y = obj["yPos"].asInt();
        p.y = 0;
        crosshair = obj["crosshair"].asInt();
        mask = obj["mask"].asInt();

        while (SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if (e.type == SDL_QUIT) // ***** NEED TO FIGURE OUT WHAT SDL_QUIT IS AND HOW IT WORKS
            {
                quit = true;
            }

            //Debug features for testing without crank
            else if (e.type == SDL_KEYDOWN) // ***** MAYBE THESE CAN BE REMOVED DUNNO
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

        //handle crank data
        bluetoothFile = fopen("bluetooth_output.txt", "r");
        fscanf(bluetoothFile, "%s", crankValueStr);
        fclose(bluetoothFile);
        int raw_crank_data = atoi(crankValueStr);
        std::cout << "rawCrankVal = " << raw_crank_data << " interval = " << interval << "\n";
        
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

            //present the texutre and prepare for the next frame
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
