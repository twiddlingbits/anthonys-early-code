#ifndef CUELIST_H
#define CUELIST_H TRUE

#define MAX_CUES 100

struct time_code {
   short hours;
   short minutes;
   short seconds;
   short frames;
   };

struct cue_event {
   struct MinNode all_node;
   struct MinNode local_node;  /* all cue_events pointing to region */
   struct region *region;
   struct time_code start;
   short vol_left;
   short vol_right;
   int fade_in_length;           /* what about cross fade types???*/
   int fade_out_length;          /* how about making these aneffect??? **/
   int loop_count;
   unsigned long flags;
   char event_text[80];
   char description[40];
   };

#endif

