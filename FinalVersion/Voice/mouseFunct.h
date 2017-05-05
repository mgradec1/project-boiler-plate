//  mouseFunct.h
//  Authored by Cody Fagley
//  February 13, 2017
//  Senior Design:  MouseVision

/*  
 *  Source Code file containing live speech recognition  
 *  and corresponding mouse event actions
 *
 *  It depends on the CMU Sphinx-Base and PocketSphinx
 *  external libraries for speech recognition and interfaces
 *  with xorg server and windows for mouse events
 *
 *  USAGE:  
 *  mouseFunct, at this time, can recognize the key strings:
 *      "left click", "right click", "close program"
 */



//  Standard Libraries
#include <stdio.h>
#include <stdbool.h>
#include <iostream>

//  CMU Sphinx/PocketSphinx Libraries
#include <pocketsphinx.h>
#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>

#define MODELDIR "C:/sphinx/pocketsphinx"
//  OS Specific Libraries
#include <windows.h>

//  Voice Recognition Reactions
//

//  Left Click
int leftClick()
{
    printf("Left Click\n");

    //  If Windows
    #if defined(_WIN32) && !defined(__CYGWIN__) 
	    INPUT Input = { 0 };

	    // left mouse button down
	    Input.type = INPUT_MOUSE;
	    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	    ::SendInput(1, &Input, sizeof(INPUT));

	    // left mouse button up
	    ::ZeroMemory(&Input, sizeof(INPUT));
	    Input.type = INPUT_MOUSE;
	    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	    ::SendInput(1, &Input, sizeof(INPUT));

    //  If Linux

    return 0;
}

//  Right Click
int rightClick()
{
    printf("Right Click\n");

    #if defined(_WIN32) && !defined(__CYGWIN__) 
	    INPUT Input = { 0 };

	    // left mouse button down
	    Input.type = INPUT_MOUSE;
	    Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	    ::SendInput(1, &Input, sizeof(INPUT));

	    // left mouse button up
	    ::ZeroMemory(&Input, sizeof(INPUT));
	    Input.type = INPUT_MOUSE;
	    Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	    ::SendInput(1, &Input, sizeof(INPUT));
    #else
        
        mouseClick(Button2);
        
    #endif

    return 0;
}




//  Taken from pocketsphinx_continuous
/* Sleep for specified msec */
static void
sleep_msec(int32 ms)
{
#if (defined(_WIN32) && !defined(GNUWINCE)) || defined(_WIN32_WCE)
    Sleep(ms);
#else
    /* ------------------- Unix ------------------ */
    struct timeval tmo;

    tmo.tv_sec = 0;
    tmo.tv_usec = ms * 1000;

    select(0, NULL, NULL, NULL, &tmo);
#endif
}


void mouseFunct()
{
    //  Variable Declaration
    bool quit;      //  Check for program ending
    ad_rec_t *ad;
    int16 adbuf[2048];
    bool utt_started, in_speech;
    int32 k;
    char const *hyp;
    ps_decoder_t *ps;
    cmd_ln_t *config;

    // Shut up E_INFO (Found on Stack Exchange)
    err_set_logfp(NULL);
    err_set_debug_level(0);

     //  Set Configuration
    config = cmd_ln_init(NULL, ps_args(), TRUE,
		"-hmm",MODELDIR "/model/en-us/en-us",
		"-lm", MODELDIR "/model/en-us/en-us.lm.bin",
		"-dict", MODELDIR "/model/en-us/cmudict-en-us.dict",
		NULL);
    
    //  Initialize Decoder
    ps = ps_init(config);
    if (ps == NULL) printf("Failed to create recognizer\n");


    //  Code taken from pocketsphinx_continuous model
    if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"), (int)cmd_ln_float32_r(config, "-samprate"))) == NULL)
        E_FATAL("Failed to open audio device\n");
	else {
		printf ("worked");
	}
        
    if (ad_start_rec(ad) < 0)
        E_FATAL("Failed to start recording\n");

    if (ps_start_utt(ps) < 0)
        E_FATAL("Failed to start utterance\n");
    utt_started = false;

    //  Until the Keyword "exit" is encountered, continue listening
    quit = false;
    while (!quit)
    {
        if ((k = ad_read(ad, adbuf, 2048)) < 0) E_FATAL("Failed to read audio\n");

        ps_process_raw(ps, adbuf, k, FALSE, FALSE);
        in_speech = ps_get_in_speech(ps);
        if (in_speech && !utt_started) 
        {
            utt_started = TRUE;
        }
        if (!in_speech && utt_started) {
            /* speech -> silence transition, time to start new utterance  */
            ps_end_utt(ps);
            hyp = ps_get_hyp(ps, NULL );
            
            //  Parse the Results
            if (hyp != NULL) {
				std::cout << hyp << std::endl;
                //
                //  Actual Program Functionality
                // 
                
                //  if left click
                if (!strcmp(hyp, "left click"))
                {
                    if (leftClick() != 0) printf("Left Click Failed\n");
                }

                //  if right click
                if (!strcmp(hyp, "right click"))
                {
                    if (rightClick() != 0) printf("Right Click Failed\n");
                }

                //  if exit
                if (!strcmp(hyp, "close program"))
                {
                    quit = true;
                }

            }

            if (ps_start_utt(ps) < 0) E_FATAL("Failed to start utterance\n");
            utt_started = FALSE;
        }
        sleep_msec(25);
        
    }

    //  Free Allocated Resources
    ad_close(ad);
    ps_free(ps);
    cmd_ln_free_r(config);

}


#endif
