//basic game of life in C++
//Made by T&R (@Mecknavorz)
//to compile:
//g++ BasicConways.cpp -L/usrX11r6/lib -lX11


//includes
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
using namespace std;

//global variables
Display *dis;
int screen;
Window win;
GC gc;
//the gameboard
bool board[90][90];
bool nboard[90][90];


//---------
//game code
//---------
//figure out how many living cells are nearby a cell at coords (x, y)
int count_nearby(int x, int y){
    int c = 0;
    //scan nearby
    for(int i = -1; i < 2;  i++){
        for(int j = -1; j < 2; j++){
            if((i == 0) && (j ==0)) continue; //do nothing for the center
            if((i+x)<0 || (i+x >= sizeof(board))) continue; //skip for rows out of range
            if((j+y)<0 || (j+y >= sizeof(board[0]))) continue; //skip for coulmns out of range
            if(board[i+x][j+y] == true){ //increment c for neighbours
                c++;
            }
        }
    }
    return c;
}

//to iterate the board
void iterate_board(){
    for(int i=0; i < 90; i++){
        for(int j=0; j < 90; j++){
            int x = count_nearby(i, j);
            //function for debugging
            /*if(x> 0){
                cout << "[Near (" << i << ", " << j << "): " << x << "] ";
            }*/
            if(board[i][j] == true){ //conditions for living neighbors
                if(x < 2){ //if there are less than two live neighours, die
                    nboard[i][j] = false;
                } 
                else if(x > 3){ //if there are more than three live neighbors, die
                    nboard[i][j] = false;
                }
                else{ //if there's the right number of neighbors then make sure the cell is alive
                    nboard[i][j] = true;
                }
            }
            else { //condition for the dead cells
                if(x == 3){ //spawn a new cell if there are more than three living neighbors
                    nboard[i][j] = true;
                }
            }
        }
    }

    //actually iterate the board
    for(int i=0; i < 90; i++){
        for(int j=0; j < 90; j++){
            board[i][j] = nboard[i][j];
        }
    }

    //cout << "\n";
}

void spawnRand(int liv){
    for(int i=0; i < 90; i++){
        for(int j=0; j < 90; j++){
            int x = std::rand()%100; //generate a random number between 0 and 99
            if(x <= liv){ //if out number is less than the desired % alive, make alive
                board[i][j] = true;
            }
            else{ //otherwise set to dead
                board[i][j] = false;
            }
        }
    }
}


//--------------
//graphics stuff
//--------------
void init_X(){
    //get the colors
    unsigned long black, white;

    //set up screen stuff
    dis = XOpenDisplay((char*)0);
    screen = DefaultScreen(dis);
    //actually load in black and white
    black = BlackPixel(dis, screen);
    white = WhitePixel(dis, screen);

    //After display is initialized open a window
    win=XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0, 900, 900, 5, white,black);
    //standard window properties
    XSetStandardProperties(dis, win, "Conway's Game of Life", "H-Hewwo?", None, NULL, 0, NULL);
    //what inputs are allowed
    XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);
    //create the graphics context
    gc = XCreateGC(dis, win, 0, 0);
    //set the foreground and background colors
    XSetBackground(dis, gc, white);
    XSetForeground(dis, gc, black);
    //clear the window and bring it on top of other windows
    XClearWindow(dis, win);
    XMapWindow(dis, win);
};

void close_x(){
    //waste management
    XFreeGC(dis, gc);
    XDestroyWindow(dis, win);
    XCloseDisplay(dis);
    exit(1);
}

void redraw(){
    XClearWindow(dis, win);
}

int main() {
    cout << "Running Basic Conway's Game of Life... \n";
    
    //actual code to do things
    XEvent event; //Xevent declaration
    KeySym key; //for keypresses
    char text[255]; //a char buffer for keypress

    init_X(); //initialize the display

    while(1){
        //draw the cells
        //I tried having it pull off the size of the array but that caused a segfault
        for (int i=0; i<90; i++){
            for(int j=0; j<90; j++){
            if(board[i][j] == true){
                    XFillRectangle(dis, win, gc, (i*10), (j*10), 10, 10);
                }
                else{
                    XClearArea(dis, win, (i*10), (j*10), 10, 10, 0);
                }
            }
        }

        XNextEvent(dis, &event);
        if(event.type==Expose && event.xexpose.count==0){
            redraw(); //if the window is exposed redraw
        }

        if(event.type==KeyPress&& XLookupString(&event.xkey, text, 255, &key, 0)==1){
            if(text[0]=='q'){
                close_x(); //quite the game if q is pressed
            }
            if(text[0]==' '){ //iterate when we press space, continuous if held
                iterate_board();
            }
            if(text[0]=='r'){
                spawnRand(50); // spawn rate in 50%
                //iterate_board();
                redraw();
            }
        }

        //add or remove a cell where we click and add it to the gameboard
        if(event.type==ButtonPress){
            //figure out where we clicked
            int x = event.xbutton.x,
                y = event.xbutton.y;
            //get the coords for the board not the screen
            int bx = floor(x/10);
            int by = floor(y/10);
            //cout << "x: " << bx << ", y: " << by << "\n";
            //cout << "nearby: " << count_nearby(bx, by) << "\n";
            XSetForeground(dis, gc, 255); //this might need to go in the for loops below
            //if there's no cell there put one there
            board[bx][by] = !board[bx][by]; ///this might be broken?
            //cout << "(" << bx << ", " << by << ") set to " << board[bx][by] << "\n";
            //cout << "nearby: " << count_nearby(bx, by) << "\n";
        }
    }

    //close the display
    close_x();
    return 0;
}
