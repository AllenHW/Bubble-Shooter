/*
Summative: Megaman
Allen Wang & Nasir Bin Reza
ICS3U, Jan 13 2012
*/

//This is the recreation of a game called Bubble Trouble, althought we used megaman as our character.
//The player has to shoot laser to break bouncing balls on the screen before getting hit any of them
//ball will keep dividing until it reaches a certain size. If the user breaks all the balls, he/she wins
//The game also has a navigation system which allows the user to use menu screem, select levels, see instrctions,
//pause the game, etc

/*------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------BEGINNING OF GAME PROGRAMMING--------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------*/

#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>

using namespace std;

//global variables
int i;
int j;

//setting screen width and length
int screen_width = 800;
int screen_height = 480;

/*-------------------CREATING STRUCTURES FOR RELATED VARIABLES-----------------*/

//Structure for balls
struct game_balls{
       BITMAP *image;
       float x;
       float y;
       float movement_x;
       float movement_y;
       bool activation;
       };

//structure for the laser
struct game_laser {
       BITMAP *image;
       float x;
       float y;
       float movement;
       bool on;
};

//structure for the megaman
struct game_megaman {
       BITMAP *image;
       float x;
       float y;
       float movement;
};

//creating variable for the following function needed for the timer
volatile long speed_counter = 0;

//function for the basic timer
void increment_speed_counter() {
	speed_counter++;
}
END_OF_FUNCTION(increment_speed_counter);

//function that detects a collision
bool checkCollision (float object1_x, float object1_y, BITMAP* object1, float object2_x, float object2_y, BITMAP* object2, BITMAP* megaman_image) {

     //updating the boundary
     float object1_left = object1_x;
     float object1_top = object1_y;
     float object1_right = object1_x + object1->w;
     float object1_bottom = object1_y + object1->h;

     float object2_left = object2_x;
     float object2_top = object2_y;
     float object2_right = object2_x + object2->w;
     float object2_bottom = object2_y + object2->h;

     //changing the boundary a bit for images of megaman
     if (object1 == megaman_image) {
         object1_left += 20;
         object1_right -= 15;
     }
     if (object2 == megaman_image) {
         object2_left += 20;
         object2_right -= 15;
     }

     //collision detection
     if (object1_top < object2_bottom && object1_bottom > object2_top && object1_right > object2_left && object1_left < object2_right)
         return (true);
     else
          return (false);
}
END_OF_FUNCTION(checkCollision);

//determines the movement of the balls
void moveBall (struct game_balls &ball, float gravity, BITMAP* large, BITMAP* medium, BITMAP *small, char level[11]) {
     ball.movement_y += gravity;    //the speed of the ball increases, creating a falling motion

     if (ball.y + ball.image->h > screen_height - 50){
         if (ball.image == large){
             ball.movement_y = -2.6;                //balls of different sizes bounce to different heights
             if (strcmp(level, "Extreme") == 0)    //For extreme level, ball bounces faster
                ball.movement_y = -3;
         }
         if (ball.image == medium) {
             ball.movement_y = -2.2;
             if (strcmp(level, "Extreme") == 0)
                ball.movement_y = -2.6;
         }
         if (ball.image == small) {
             ball.movement_y = -2;
             if (strcmp(level, "Extreme") == 0)
                ball.movement_y = -2.3;
         }
     }

     if (ball.x <= 0 || ball.x + ball.image->w >= screen_width){  //if ball hits the walls, direction of the movement changes
         ball.movement_x *= -1;
     }

     ball.x += ball.movement_x;      //ball position changes by its movement speed
     ball.y += ball.movement_y;
}
END_OF_FUNCTION(moveBall);

//function that controls the player with the press of the keyboard
void moveMan (struct game_megaman &megaman, BITMAP* frontSide, BITMAP* leftSide1, BITMAP* rightSide1, BITMAP* leftSide2, BITMAP* rightSide2, BITMAP* Laser, int frame_counter) {
     if (key[KEY_RIGHT]) {
         megaman.y = screen_height - 120;       //move the megaman to the right
         megaman.x += megaman.movement;
         if (frame_counter <=15)             //switch to different pictures
            megaman.image = rightSide1;
         if (frame_counter > 15)
            megaman.image = rightSide2;
     }
     else if(key[KEY_LEFT]) {
         megaman.y = screen_height - 120;        //move the megaman to the right
         megaman.x -= megaman.movement;
         if (frame_counter <= 15)
            megaman.image = leftSide1;
         if (frame_counter > 15)
            megaman.image = leftSide2;
     }
     else if (key[KEY_UP] || key[KEY_SPACE]) {           //switch to a different sprite when shooting laser
          megaman.image = Laser;
          megaman.y = screen_height - 140;
     }
     else {
          megaman.y = screen_height - 120;
          megaman.image = frontSide;
     }

     if (megaman.x < 0)                                        //megaman can't go beyond the wall
         megaman.x= 0;
     else if (megaman.x > screen_width - megaman.image->w)
          megaman.x= screen_width - megaman.image->w ;
}
END_OF_FUNCTION(moveMan);

//function for determining the posintion of the laser
void moveLaser (struct game_laser &laser, float megaman_x, float megaman_y) {

     if (laser.on == false){
        laser.x = megaman_x+25;                   //laser is always at the smae x position as the character
        laser.y = megaman_y+2;
     }

     if ((key[KEY_UP] || key[KEY_SPACE]))
        laser.on = true;

     if (laser.on == true)
         laser.y -= laser.movement;     //laser moves

     if (laser.y <= 0)                    //laser disappears when it hits the top of the screen
         laser.on = false;
}
END_OF_FUNCTION(moveLaser);

//splits the ball into two more balls or completely destroys them
int splitBall (struct game_balls ball[][3], int i, int j, BITMAP *largeBall, BITMAP *mediumBall, BITMAP *smallBall) {

    if (ball[i][j].image == largeBall) {             //for a large ball,
        ball[i+1][j].activation = true;              //activate another ball
        ball[i+1][j].image = mediumBall;             //the new ball is medium sized
        ball[i+1][j].x = ball[i][j].x;
        ball[i+1][j].y = ball[i][j].y;
        ball[i+1][j].movement_x = ball[i][j].movement_x * -1;      //the new ball moves to the opposite way
        ball[i+1][j].movement_y = - 0.6*fabs(ball[i][j].movement_y);    //when the laser hits the ball, the balls bounce up a bit

        ball[i][j].image = mediumBall;                               //the ball becomes meidum sized
        ball[i][j].movement_y = -0.6*fabs(ball[i][j].movement_y);    //bounces up a bit
        return(0);           //cut off opperation
    }

    if (ball[i][j]. image == mediumBall){   //for a medium ball, do the same things except that the balls become small sized
        ball[i+2][j].activation = true;
        ball[i+2][j].image = smallBall;
        ball[i+2][j].x = ball[i][j].x;
        ball[i+2][j].y = ball[i][j].y;
        ball[i+2][j].movement_x = ball[i][j].movement_x * -1;
        ball[i+2][j].movement_y = -0.6*fabs(ball[i][j].movement_y);

        ball[i][j].image = smallBall;
        ball[i][j].movement_y = -0.6*fabs(ball[i][j].movement_y);
        return(0);
    }

    if (ball[i][j]. image == smallBall){     //if the ball is small, it doesn't split but disappears
        ball[i][j].activation = false;
     }
}
END_OF_FUNCTION(splitBall);

//timer function
bool Timer(int &counter, int set_value) {

    counter++;                   //counter increases

    if (counter >= set_value){     //if counter reaches to a set value, return true
        counter = 0;
        return(true);
    }
    else
         return(false);

}
END_OF_FUNCTION(Timer);

//function that resets variables and refresh many things
void resetGame (struct game_balls ball[][3], int ballNumber, char level[11], float &gravity, struct game_megaman &megaman, bool &death, int &time_bar, struct game_laser &laser, BITMAP* largeBall, int &ballSplit, BITMAP* laser_image) {

     for (j=0; j<ballNumber; j++){
         for (i=0; i<4; i++){                 //set the speeds  of the balls
             ball[i][j].y = -40;
             ball[i][j].movement_x = 0.7;
             if (strcmp(level, "Extreme") == 0)
                ball[i][j].movement_x = 1;      //for extreme level, the speed is faster
             ball[i][j].movement_y= 0;
             ball[i][j].activation= false;
     }
         ball[0][j].activation = true;       //only activate the firt ball
         ball[0][j].image = largeBall;
     }

     ball[0][0].x = 1;
     if (ballNumber == 2)
        ball[0][1].x = 600;      //reset the positions of the ball
     if (ballNumber == 3)
        ball[0][1].x = 300;
        ball[0][2].x = 600;

     ballSplit = 0;

     gravity = 0.014;                        //set various values
     if (strcmp(level, "Extreme") == 0)
        gravity = 0.02;

     megaman.x = 400;                 //movement/placement of the megamen
     megaman.y = screen_height-120;
     megaman.movement = 1.2;
     death = false;

     laser.movement = 1.5;           //initialize the laser
     laser.on = false;
     laser.image = laser_image;
     time_bar = 0;
}
END_OF_FUNCTION(resetGame);

//change the colour of the buttons
void showButtons (BITMAP* buffer, BITMAP* buttonImage, BITMAP* buttonImage2, int button_x, int button_y, int mouse_x, int mouse_y){

     draw_sprite(buffer, buttonImage, button_x, button_y);

     //if the mouse touches the button, load another image
     if (mouse_x > button_x && mouse_x < button_x+buttonImage->w && mouse_y > button_y && mouse_y < button_y+buttonImage->h){
        draw_sprite(buffer, buttonImage2, button_x, button_y);
     }
}
END_OF_FUNCTION(showButtons);

//button action when clicked on
bool clickButtons (BITMAP* buttonImage,int button_x, int button_y, int mouse_x, int mouse_y){

     // if the button is clicked, return true
     if (mouse_x > button_x && mouse_x < button_x+buttonImage->w && mouse_y > button_y && mouse_y < button_y+buttonImage->h){
        if (mouse_b & 1)
           return(true);
     }
     else
         return(false);

}
END_OF_FUNCTION(clickButtons);


//main function
int main(int argc, char *argv[]){

    allegro_init();
	install_keyboard();
	install_timer();
	install_mouse();

	//timer initializing for the basic timer
	LOCK_VARIABLE(speed_counter);
	LOCK_FUNCTION(increment_speed_counter);
	install_int_ex(increment_speed_counter, BPS_TO_TIMER(150));

	set_color_depth(desktop_color_depth());
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, screen_width, screen_height, 0, 0);
    install_sound (MIDI_AUTODETECT, MIDI_AUTODETECT , 0);

    //inititallize the structures
    game_balls ball[4][3];
    game_laser laser;
    game_megaman megaman;

    //loading images for megaman
	BITMAP *megamanFront = load_bitmap("Sprites/Megaman Front.bmp", NULL);
    BITMAP *megamanLaser = load_bitmap("Sprites/Megaman Laser.bmp", NULL);
    BITMAP *megamanRight1 = load_bitmap("Sprites/Megaman Right1.bmp", NULL);
    BITMAP *megamanLeft1 = load_bitmap("Sprites/Megaman Left1.bmp", NULL);
    BITMAP *megamanRight2 = load_bitmap("Sprites/Megaman Right2.bmp", NULL);
    BITMAP *megamanLeft2 = load_bitmap("Sprites/Megaman Left2.bmp", NULL);
    //loading image for heart
    BITMAP *Heart = load_bitmap("Sprites/Heart.bmp", NULL);

    //error checking to see if any bitmap is not loaded
    if (( megamanFront== NULL) || ( megamanLaser== NULL) || ( megamanRight1== NULL) || ( megamanLeft1== NULL) || ( megamanRight2== NULL) || ( megamanLeft2== NULL) || ( Heart== NULL) ){
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Error: Could not load megaman and heart sprites");
		exit(EXIT_FAILURE); //Display error message
	}

    //loading images for balls
    BITMAP *largeBall = load_bitmap("Sprites/Ball Large.bmp", NULL);
    BITMAP *mediumBall = load_bitmap("Sprites/Ball Medium.bmp", NULL);
    BITMAP *smallBall = load_bitmap("Sprites/Ball Small.bmp", NULL);

    if (( largeBall== NULL) || ( mediumBall== NULL) || ( smallBall== NULL)){
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Error: Could not load ball images");
		exit(EXIT_FAILURE); //Display error message
	}

	//image of the laser
	BITMAP *laser_image = load_bitmap("Sprites/Laser.bmp", NULL);

    if (laser_image == NULL){
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Error: Could not load laser images");
		exit(EXIT_FAILURE); //Display error message
	}

    //loading platform
    BITMAP *platform = load_bitmap("Sprites/Platform.bmp", NULL);
    //loading backgrounds
    BITMAP *Background = load_bitmap("Sprites/Background.bmp", NULL);
    BITMAP *Menu = load_bitmap("Sprites/Megaman Title Screen.bmp", NULL);
    BITMAP *CreditsMenu= load_bitmap("Sprites/Credits Screen.bmp", NULL);
    BITMAP *InstructionsMenu= load_bitmap("Sprites/Instructions Screen.bmp", NULL);
    //loading popups
    BITMAP *game_pause = load_bitmap("Sprites/GamePauseTitle.bmp", NULL);
    BITMAP *game_won = load_bitmap("Sprites/GameWonTitle.bmp", NULL);
    BITMAP *game_end = load_bitmap("Sprites/GameEndTitle.bmp", NULL);

    if (( platform== NULL) || ( Background== NULL) || ( Menu== NULL) || ( CreditsMenu== NULL) || ( InstructionsMenu== NULL)  || ( game_pause== NULL) || ( game_won== NULL) || ( game_end== NULL)){
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Error: Could not load backgrounds images");
		exit(EXIT_FAILURE); //Display error message
	}

    //creating buffer
    BITMAP *buffer = create_bitmap(screen_width, screen_height);

    //loading buttons
    BITMAP *play_button = load_bitmap("Sprites/Play Button.bmp", NULL);
	BITMAP *play_button2 = load_bitmap("Sprites/Play Button2.bmp", NULL);
	BITMAP *easy_button = load_bitmap("Sprites/Easy Button.bmp", NULL);
	BITMAP *easy_button2 = load_bitmap("Sprites/Easy Button2.bmp", NULL);
	BITMAP *normal_button = load_bitmap("Sprites/Normal Button.bmp", NULL);
	BITMAP *normal_button2 = load_bitmap("Sprites/Normal Button2.bmp", NULL);
	BITMAP *hard_button = load_bitmap("Sprites/Hard Button.bmp", NULL);
	BITMAP *hard_button2 = load_bitmap("Sprites/Hard Button2.bmp", NULL);
	BITMAP *extreme_button = load_bitmap("Sprites/Extreme Button.bmp", NULL);
	BITMAP *extreme_button2 = load_bitmap("Sprites/Extreme Button2.bmp", NULL);
    BITMAP *instructions_button = load_bitmap("Sprites/Instructions Button.bmp", NULL);
	BITMAP *instructions_button2 = load_bitmap("Sprites/Instructions Button2.bmp", NULL);
    BITMAP *credits_button = load_bitmap("Sprites/Credits Button.bmp", NULL);
	BITMAP *credits_button2 = load_bitmap("Sprites/Credits Button2.bmp", NULL);
   	BITMAP *exit_button = load_bitmap("Sprites/Exit Button.bmp", NULL);
    BITMAP *exit_button2 = load_bitmap("Sprites/Exit Button2.bmp", NULL);
    BITMAP *back_to_menu_button = load_bitmap("Sprites/Back to Menu Button.bmp", NULL);
    BITMAP *back_to_menu_button2 = load_bitmap("Sprites/Back to Menu Button2.bmp", NULL);
   	BITMAP *retry_button = load_bitmap("Sprites/Retry Button.bmp", NULL);
    BITMAP *retry_button2 = load_bitmap("Sprites/Retry Button2.bmp", NULL);
    BITMAP *resume_button = load_bitmap("Sprites/Resume Button.bmp", NULL);
    BITMAP *resume_button2 = load_bitmap("Sprites/Resume Button2.bmp", NULL);
    BITMAP *menu_button = load_bitmap("Sprites/Menu Button.bmp", NULL);
    BITMAP *menu_button2 = load_bitmap("Sprites/Menu Button2.bmp", NULL);
    BITMAP *next_button = load_bitmap("Sprites/Next Button.bmp", NULL);
    BITMAP *next_button2 = load_bitmap("Sprites/Next Button2.bmp", NULL);

    //error checking to see if any bitmap is not loaded
    if ( (play_button== NULL) || (play_button2== NULL) || (easy_button== NULL) || (easy_button2== NULL) || (normal_button== NULL) || (normal_button2== NULL) || (hard_button== NULL) || (hard_button2== NULL) || (extreme_button== NULL) || (extreme_button2 == NULL) || (instructions_button== NULL) || (instructions_button2== NULL) || (credits_button== NULL) || (credits_button2== NULL) || (exit_button== NULL) || (exit_button2== NULL) || (back_to_menu_button== NULL) || (back_to_menu_button2== NULL) || (retry_button== NULL) || (retry_button2== NULL) || (resume_button== NULL) || (resume_button2== NULL) || (menu_button== NULL) || (menu_button2== NULL)  || (next_button== NULL) || (next_button2== NULL) ){
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Error: Could not load all button images");
		exit(EXIT_FAILURE); //Display error message
	}

    //loading music files
    MIDI *musicTitle = load_midi("Music/idiotseag.mid");
    bool musicPlaying_Title = false;
    MIDI *musicPlay = load_midi("Music/32-WilyTheme.mid");
    bool musicPlaying_Play = false;

    if ( (musicTitle == NULL) || (musicPlay == NULL)){
		set_gfx_mode(GFX_TEXT,0,0,0,0);
		allegro_message("Error: Could not load music files");
		exit(EXIT_FAILURE); //Display error message
	}


    //creating file pointer and opening textfile to store data
	FILE *fptr;
    fptr = fopen("High Score.txt", "r");
    int HighScore[4];

    //defining and initializing variables
    float gravity;      //value used to create the faling motion of the ball
    int time_bar = 0;    //used to as a timer
    int frame_counter = 0;
    int pause_counter = 0;
    char DoThis[21] = "ShowMenu";    //represents which screen the game is at
    char level[11] = "";      //represent which level one's playing
    for (i=0; i<4; i++)
        fscanf( fptr, "%d", &HighScore[i]);         //read high score from file
    fclose(fptr);

    int ballNumber;     //number of large balls to start with
    int currentScore;     //score
    int ballSplit = 0;     //counts how many times user breaks the ball
    int timeLeft;
    int LevelNumber;
    int ball_counter;        //counts how many balls are on screen

    bool death = false;         //checks if the player is alive or dead
    bool pauseGame = false;      //checks if the game is paused
    bool wonGame = false;       //checks if user win the game

    int iLife;         //represents the number of lives the user has left



    while (!(strcmp(DoThis, "ExitGame") == 0)){

        /*-------------------Beginning of the logical loop-----------------*/
        while (speed_counter > 0){

            if (strcmp(DoThis, "ShowMenu") == 0) {    //if user is at the menu screen

                if (musicPlaying_Title == false){        //play title music
                play_looped_midi(musicTitle, 1,-1);
                musicPlaying_Title = true;
                musicPlaying_Play = false;
            }
                //goes to different screen when the buttons are clicked
                if (clickButtons(play_button, 30, 50, mouse_x, mouse_y) == true) {
                    strcpy(DoThis, "ShowLevels");
                }
                if (clickButtons(instructions_button, 30, 150, mouse_x, mouse_y) == true) {
                    strcpy(DoThis, "ShowInstructions");
                }
                if (clickButtons(credits_button, 30, 250, mouse_x, mouse_y) == true) {
                    strcpy(DoThis, "ShowCredits");
                }
                if (clickButtons(exit_button, 30, 350, mouse_x, mouse_y) == true) {
                    strcpy(DoThis, "ExitGame");
                }
            }

            //if the user selects to play
            if (strcmp(DoThis, "ShowLevels") == 0) {
                if (clickButtons(easy_button, 230, 180, mouse_x, mouse_y) == true) {
                    strcpy(level, "Easy");      //if the user selects easy mode
                    ballNumber = 1;             //there will be 1 large ball
                    resetGame(ball, ballNumber, level, gravity, megaman, death, time_bar, laser, largeBall, ballSplit, laser_image);
                    iLife = 3;
                    strcpy(DoThis, "PlayGame");    //goes to play screen
                }
                if (clickButtons(normal_button, 470, 180, mouse_x, mouse_y) == true) {
                    strcpy(level, "Normal");     //normal mode
                    ballNumber = 2;              //2 large balls
                    resetGame(ball, ballNumber, level, gravity, megaman, death, time_bar, laser, largeBall, ballSplit, laser_image);
                    iLife = 3;
                    strcpy(DoThis, "PlayGame");
                }
                if (clickButtons(hard_button, 230, 290, mouse_x, mouse_y) == true) {
                    strcpy(level, "Hard");       //hard mode
                    ballNumber = 3;              //3 balls
                    resetGame(ball, ballNumber, level, gravity, megaman, death, time_bar, laser, largeBall, ballSplit, laser_image);
                    iLife = 3;
                    strcpy(DoThis, "PlayGame");
                }
                if (clickButtons(extreme_button, 470, 290, mouse_x, mouse_y) == true) {
                    strcpy(level, "Extreme");      //extreme level
                    ballNumber = 3;                //3 balls
                    resetGame(ball, ballNumber, level, gravity, megaman, death, time_bar, laser, largeBall, ballSplit, laser_image);
                    iLife = 3;
                    strcpy(DoThis, "PlayGame");
                }
                if (clickButtons(back_to_menu_button, 520, 400, mouse_x, mouse_y) == true){
                    strcpy(DoThis, "ShowMenu");
                }
            }
            //if the user selects instructions
            if (strcmp(DoThis, "ShowInstructions") == 0) {
                if (clickButtons(back_to_menu_button, 520, 400, mouse_x, mouse_y) == true){
                    strcpy(DoThis, "ShowMenu");     //this is a button to go back to the main menu
                }
            }
            //if the user selects credits
            if (strcmp(DoThis, "ShowCredits") == 0) {
                if (clickButtons(back_to_menu_button, 520, 415, mouse_x, mouse_y) == true){
                    strcpy(DoThis, "ShowMenu");
                }
            }

            //if the user has selected to play the game
            if (strcmp(DoThis, "PlayGame") == 0) {

                if (musicPlaying_Play == false){
                   play_looped_midi(musicPlay, 1,-1);        //play game music
                   musicPlaying_Play = true;
                   musicPlaying_Title = false;
                }

                if (!(death == true || pauseGame == true)) {  // unless the player is dead or the the game is paused
                    //move the megaman
                    moveMan(megaman, megamanFront, megamanLeft1, megamanRight1, megamanLeft2, megamanRight2, megamanLaser, frame_counter);
                    //move the laser
                    moveLaser(laser, megaman.x, megaman.y);

                    ball_counter = 0;

                    for (i=0; i<4; i++){
                        for (j=0; j<ballNumber; j++){
                            if (ball[i][j].activation == true){   //if the ball is activated
                               //move the ball
                                moveBall (ball[i][j], gravity, largeBall, mediumBall, smallBall, level);
                                ball_counter ++;     //count how many ball is activated

                                if (laser.on == true) {    //check the collisoin between laser and the ball
                                    if (checkCollision(laser.x, laser.y, laser.image, ball[i][j].x, ball[i][j].y, ball[i][j].image, megaman.image) == true) {
                                        splitBall(ball, i, j, largeBall, mediumBall, smallBall);   //split the ball
                                        ballSplit++;       //counts the number of ball splitting, which is used to calculate the score
                                        laser.on = false;    //once laser hits the ball, it disappears
                                    }
                                }
                                //check collison between the ball and the megaman
                                if (checkCollision(ball[i][j].x, ball[i][j].y, ball[i][j].image, megaman.x, megaman.y, megaman.image, megaman.image) == true){
                                    death = true;
                                }
                            }

                        }
                    }

                    if (ball_counter == 0 )   //if there is no more ball left
                        wonGame = true;        //player wins

                    if (!(wonGame == true))
                        time_bar ++;

                    if (time_bar >= 12000)    //when time reaches to a certain amount,
                        death = true;            //player dies


                    if (key[KEY_ESC]){
                        pauseGame = true;     //pause the game if escape key is pressed
                    }
                }

                if (key[KEY_A] && key[KEY_D]){        //a cheat code to beat the game
                    for (i=0; i<4; i++){
                        for (j=0; j<ballNumber; j++)
                            ball[i][j].activation = false;
                    }
                }

                if (key[KEY_Q] && key[KEY_E]){       //a cheat code to reset the highcores
                    for (i=0; i<4; i++){
                        HighScore[i] = 0;
                    }
                }
                //when the game is paused, user can click buttons
                if (pauseGame == true){
                    if (clickButtons(resume_button, 180, 210, mouse_x, mouse_y) == true) {
                        pauseGame = false;
                    }
                    if (clickButtons(retry_button, 180, 290, mouse_x, mouse_y) == true) {
                        pauseGame = false;
                        resetGame(ball, ballNumber, level, gravity, megaman, death, time_bar, laser, largeBall, ballSplit, laser_image);
                        iLife = 3;
                    }
                    if (clickButtons(menu_button, 530, 210, mouse_x, mouse_y) == true) {
                        pauseGame = false;
                        strcpy(DoThis, "ShowMenu");
                    }
                    if (clickButtons(exit_button, 530, 290, mouse_x, mouse_y) == true) {
                        strcpy(DoThis, "ExitGame");
                    }
                }
                //when the user beats the level, they get buttons to click
                if (wonGame == true) {
                    if (!(strcmp(level, "Extreme") == 0)) {
                        if (clickButtons(next_button, 530, 210, mouse_x, mouse_y) == true) {
                            wonGame = false;
                            if (strcmp(level, "Easy") == 0)        //if user selects next level, the level changes
                                strcpy(level, "Normal");
                            else if (strcmp(level, "Normal") == 0)
                                strcpy(level, "Hard");
                            else if (strcmp(level, "Hard") == 0)
                                strcpy(level, "Extreme");
                            if (ballNumber < 3)
                                ballNumber++;
                            resetGame(ball, ballNumber, level, gravity, megaman, death, time_bar, laser, largeBall, ballSplit, laser_image);
                            iLife = 3;
                        }
                        if (clickButtons(menu_button, 180, 210, mouse_x, mouse_y) == true) {
                            wonGame = false;           //go back to menu
                            strcpy(DoThis, "ShowMenu");
                        }
                    }
                    else {
                        if (clickButtons(menu_button, 355, 290, mouse_x, mouse_y) == true) {
                            wonGame = false;
                            strcpy(DoThis, "ShowMenu");
                        }
                    }
                    if (clickButtons(retry_button, 180, 290, mouse_x, mouse_y) == true) {
                        wonGame = false;
                        resetGame(ball, ballNumber, level, gravity, megaman, death, time_bar, laser, largeBall, ballSplit, laser_image);
                        iLife = 3;          //restart level
                    }

                    if (clickButtons(exit_button, 530, 290, mouse_x, mouse_y) == true) {
                        strcpy(DoThis, "ExitGame");
                    }
                }

                if (death == true && iLife <= 1) {    //if the user dies and have no life left
                    if (iLife > 0)
                        iLife--;
                    if (clickButtons(retry_button, 355, 290, mouse_x, mouse_y) == true) {
                        death = false;      //retry
                        resetGame(ball, ballNumber, level, gravity, megaman, death, time_bar, laser, largeBall, ballSplit, laser_image);
                        iLife = 3;
                    }
                    if (clickButtons(menu_button, 180, 290, mouse_x, mouse_y) == true) {
                        death = false;
                        strcpy(DoThis, "ShowMenu");      //go back to menu
                    }
                    if (clickButtons(exit_button, 530, 290, mouse_x, mouse_y) == true) {
                        strcpy(DoThis, "ExitGame");     //exit game
                    }
                }
                //if the user still has life left, pause the screen for a bit and resart the level, with one less life
                if (death == true && iLife > 1 && Timer(pause_counter, 200) == true){
                   resetGame(ball, ballNumber, level, gravity, megaman, death, time_bar, laser, largeBall, ballSplit, laser_image);
                   iLife--;
                }
            }

            speed_counter --;
            cout << speed_counter << endl;
		    frame_counter ++;

            if (frame_counter > 30)
                frame_counter = 0;
        }
        /*-------------------end of logical loop-----------------*/


        //printing buttons and background at main menu
        if (strcmp(DoThis, "ShowMenu") == 0) {
            show_mouse(screen);
            draw_sprite(buffer, Menu,0,0);
            showButtons(buffer, play_button, play_button2, 30, 50, mouse_x, mouse_y);
            showButtons(buffer, instructions_button, instructions_button2, 30, 150, mouse_x, mouse_y);
            showButtons(buffer, credits_button, credits_button2, 30, 250, mouse_x, mouse_y);
            showButtons(buffer, exit_button, exit_button2, 30, 350, mouse_x, mouse_y);
        }

        //printing buttons and background at level choosing menu
        if (strcmp(DoThis, "ShowLevels") == 0) {
            show_mouse(screen);
            showButtons(buffer, easy_button, easy_button2, 230, 180, mouse_x, mouse_y);
            showButtons(buffer, normal_button, normal_button2, 470, 180, mouse_x, mouse_y);
            showButtons(buffer, hard_button, hard_button2, 230, 290, mouse_x, mouse_y);
            showButtons(buffer, extreme_button, extreme_button2, 470, 290, mouse_x, mouse_y);
            showButtons(buffer, back_to_menu_button, back_to_menu_button2, 520, 400, mouse_x, mouse_y);
        }

        //printing buttons and background at instructions screen
        if (strcmp(DoThis, "ShowInstructions") == 0) {
            draw_sprite(buffer, InstructionsMenu,0,0);
            showButtons(buffer, back_to_menu_button, back_to_menu_button2, 520, 400, mouse_x, mouse_y);
        }

        //printing buttons and background at credits screen
        if (strcmp(DoThis, "ShowCredits") == 0){
            draw_sprite(buffer, CreditsMenu,0,0);
            showButtons(buffer, back_to_menu_button, back_to_menu_button2, 520, 415, mouse_x, mouse_y);
        }


        if (strcmp(DoThis, "PlayGame") == 0){

            draw_sprite(buffer, Background, 0, 0);

            for (i=0; i<4; i++){         //for all activated balls, draw images
                for (j=0; j<ballNumber; j++){
                    if (ball[i][j].activation == true)
                        draw_sprite(buffer, ball[i][j].image, ball[i][j].x, ball[i][j].y);
                }
            }

            //draw laser
            if (laser.on == true){
                draw_sprite(buffer, laser.image, laser.x, laser.y);
            }

            //drawing hearts that indicate how many lives the player has
            for (i=0; i<iLife; i++){
                draw_sprite(buffer, Heart, 690+i*35, 10);
            }

            //print which level user is playing
            textprintf_ex(buffer, font, 20,10, makecol(255,255,255), -1 , "Level: %s", level);

            //draw platform
            draw_sprite(buffer, platform, 0, screen_height - 50);

            //draw the time_bar
            rectfill(buffer, 100, 445, 700 -(time_bar/20), 455, makecol(255,0,0));
            timeLeft=(600 -(time_bar/20))/10;
            textprintf_ex(buffer, font, 370,465, makecol(255,255,255), -1 , "%ds left", timeLeft);

            //draw megaman
 			draw_sprite(buffer, megaman.image, megaman.x, megaman.y);

            //printing buttons and popup when game is paused
            if (pauseGame == true) {
                show_mouse(screen);
                draw_sprite(buffer, game_pause, 96, 75);
                showButtons(buffer, resume_button, resume_button2, 180, 210, mouse_x, mouse_y);
                showButtons(buffer, retry_button, retry_button2, 180, 290, mouse_x, mouse_y);
                showButtons(buffer, menu_button, menu_button2, 530, 210, mouse_x, mouse_y);
                showButtons(buffer, exit_button, exit_button2, 530, 290, mouse_x, mouse_y);
            }

            //score system
            currentScore = (iLife*50 + timeLeft*3)*ballSplit;

            //each level has a highscore
            if ((strcmp(level, "Easy") == 0))
                LevelNumber = 0;
            else if ((strcmp(level, "Normal") == 0))
                LevelNumber = 1;
            else if ((strcmp(level, "Hard") == 0))
                LevelNumber = 2;
            else if ((strcmp(level, "Extreme") == 0))
                LevelNumber = 3;

            //if the score is beaten, renew the highscore
            if (currentScore > HighScore[LevelNumber])
                HighScore[LevelNumber] = currentScore;

            //showing buttons and popup when level is beaten
            if (wonGame == true) {
                draw_sprite(buffer, game_won, 96, 75);
                if (!(strcmp(level, "Extreme") == 0)){
                    showButtons(buffer, next_button, next_button2, 530, 210, mouse_x, mouse_y);
                    showButtons(buffer, menu_button, menu_button2, 180, 210, mouse_x, mouse_y);
                }
                else {
                    showButtons(buffer, menu_button, menu_button2, 355, 290, mouse_x, mouse_y);
                }
                showButtons(buffer, retry_button, retry_button2, 180, 290, mouse_x, mouse_y);
                showButtons(buffer, exit_button, exit_button2, 530, 290, mouse_x, mouse_y);
            }

            //showing buttons and popup when game ends
            if (death == true) {
                if (iLife <= 1 ) {
                    show_mouse(screen);
                    draw_sprite(buffer, game_end, 96, 75);
                    showButtons(buffer, retry_button, retry_button2, 355, 290, mouse_x, mouse_y);
                    showButtons(buffer, menu_button, menu_button2, 180, 290, mouse_x, mouse_y);
                    showButtons(buffer, exit_button, exit_button2, 530, 290, mouse_x, mouse_y);
                }
            }

            //printf scores on the screen
            if ((death == true && iLife <= 1 ) || wonGame == true) {
                textprintf_ex(buffer, font, 335,230, makecol(255,255,255), -1 , "YOUR SCORE     %d", currentScore);
                textprintf_ex(buffer, font, 335,260, makecol(255,255,255), -1 , "HIGH SCORE     %d", HighScore[LevelNumber]);
                //if score = highscore
                if (currentScore == HighScore[LevelNumber] && currentScore > 0 )
                    if (strcmp(level, "Extreme") == 0 || death == true)
                        textprintf_ex(buffer, font, 325,200, makecol(255,255,255), -1 , "YOU GOT A HIGH SCORE!");
                    else
                        textprintf_ex(buffer, font, 325,300, makecol(255,255,255), -1 , "YOU GOT A HIGH SCORE!");
            }
        }

	    blit(buffer, screen, 0,0,0,0,screen_width, screen_height);
		clear_bitmap(buffer);
		clear_keybuf();
    }

    //printing the scores to the text file
    FILE *fwrite;
    fwrite = fopen("High Score.txt", "w");

    for (i=0; i<4; i++)
        fprintf(fwrite, "%d\n", HighScore[i]);

	//closing file
	fclose(fwrite);

    //destroying bitmaps and midi
	destroy_midi(musicTitle);
	destroy_midi(musicPlay);
    destroy_bitmap(megamanFront);
    destroy_bitmap(megamanLaser);
    destroy_bitmap(megamanRight1);
    destroy_bitmap(megamanLeft1);
    destroy_bitmap(megamanRight2);
    destroy_bitmap(megamanLeft2);
    destroy_bitmap(Heart);
    destroy_bitmap(largeBall);
    destroy_bitmap(mediumBall);
    destroy_bitmap(smallBall);
    destroy_bitmap(platform);
    destroy_bitmap(Background);
    destroy_bitmap(Menu);
    destroy_bitmap(CreditsMenu);
    destroy_bitmap(InstructionsMenu);
    destroy_bitmap(play_button);
	destroy_bitmap(play_button2);
	destroy_bitmap(easy_button);
	destroy_bitmap(easy_button2);
	destroy_bitmap(normal_button);
	destroy_bitmap(normal_button2);
	destroy_bitmap(hard_button);
	destroy_bitmap(hard_button2);
	destroy_bitmap(extreme_button);
	destroy_bitmap(extreme_button2);
    destroy_bitmap(instructions_button);
	destroy_bitmap(instructions_button2);
    destroy_bitmap(credits_button);
	destroy_bitmap(credits_button2);
   	destroy_bitmap(exit_button);
    destroy_bitmap(exit_button2);
    destroy_bitmap(back_to_menu_button);
    destroy_bitmap(back_to_menu_button2);
   	destroy_bitmap(retry_button);
    destroy_bitmap(retry_button2);
    destroy_bitmap(resume_button);
    destroy_bitmap(resume_button2);
    destroy_bitmap(menu_button);
    destroy_bitmap(menu_button2);
    destroy_bitmap(next_button);
    destroy_bitmap(next_button2);
    destroy_bitmap(game_pause);
    destroy_bitmap(game_won);
    destroy_bitmap(game_end);
    destroy_bitmap(buffer);
    return(0); // Exit with no errors
}
END_OF_MAIN();
