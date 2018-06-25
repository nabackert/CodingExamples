*
 * SlidingWindow.h
 *
 *  Created on: Mar 9, 2014
 *      Author: Draco
 */
#include <cnet.h>
static  CnetTimerID	timer1		= NULLTIMER;
static  CnetTimerID	timer2		= NULLTIMER;
void send_data(CnetEvent ev, CnetTimerID timer, CnetData data) {
size_t size;
size = MAX_MESSAGE_SIZE;
CHECK(CNET_read_application(&destination, buffer, &data_size));

  printf("We've sent %ld bytes bound for %d.\n", data_size, destination);



}

  void receive_data(CnetEvent ev, CnetTimerID timer, CnetData data) {
    size_t data_size;
    int link;

    data_size = MAX_MESSAGE_SIZE;
    CNET_read_physical(&link, buffer, &data_size);

    printf("We've read %ld bytes from link %d.\n", data_size, link);

    CNET_write_application(buffer, &data_size);

  }

  static void transmit_frame(MSG *msg, FRAMEKIND kind, size_t length, int seqno)
  {
      FRAME       f;
      int		link = 1;

      f.kind      = kind;
      f.seq       = seqno;
      f.checksum  = 0;
      f.len       = length;

      switch (kind) {
      case DL_ACK :
          printf("ACK transmitted, seq=%d\n", seqno);
  	break;

      case DL_DATA: {
  	CnetTime	timeout;

          printf(" DATA transmitted, seq=%d\n", seqno);
          memcpy(&f.msg, msg, (int)length);

  	timeout = FRAME_SIZE(f)*((CnetTime)8000000 / linkinfo[link].bandwidth) +
  				linkinfo[link].propagationdelay;

  	if(timer1 != NULLTIMER){
  	 timer2 = CNET_start_timer(EV_TIMER1, 3 * timeout, 0);
  	  }else{
  		timer1  = CNET_start_timer(EV_TIMER1, 3 * timeout, 0);
  	  }
  	break;
        }
      }
      length      = FRAME_SIZE(f);
      f.checksum  = CNET_ccitt((unsigned char *)&f, (int)length);
      CHECK(CNET_write_physical(1, buffer, &data_size));
  }

  static EVENT_HANDLER(application_ready)
  {
      CnetAddr destaddr;

      lastlength  = sizeof(MSG);
      CHECK(CNET_read_application(&destaddr, lastmsg, &lastlength));
      CNET_disable_application(ALLNODES);

      printf("down from application, seq=%d\n", nextframetosend);
      transmit_frame(lastmsg, DL_DATA, lastlength, nextframetosend);
      nextframetosend = 1-nextframetosend;
  }
  //
  static EVENT_HANDLER(physical_ready)
  {
      FRAME        f;
      size_t	 len;
      int          link, checksum;

      len         = sizeof(FRAME);
      CHECK(CNET_read_physical(&link, &f, &len));

      checksum    = f.checksum;
      f.checksum  = 0;
      if(CNET_ccitt((unsigned char *)&f, (int)len) != checksum) {
          printf("\t\t\t\tBAD checksum - frame ignored\n");
          return;           // bad checksum, ignore frame
      }

      switch (f.kind) {
      //handles the recieved acknowledgements
      case DL_ACK :
          if(f.seq == ackexpected) {
              printf("\t\t\t\tACK received, seq=%d\n", f.seq);
              CNET_stop_timer(timer1);
              CNET_stop_timer(timer2);
              ackexpected = 1-ackexpected;
              CNET_enable_application(ALLNODES);
          }
  	break;
//handles the recieved data
      case DL_DATA :
          printf("\t\t\t\tDATA received, seq=%d, ", f.seq);
          if(f.seq == frameexpected) {
              printf("up to application\n");
              len = f.len;
              CHECK(CNET_write_application(&f.msg, &len));
              frameexpected = 1-frameexpected;
          }
          else
              printf("ignored\n");
          transmit_frame(NULL, DL_ACK, 0, f.seq);
  	break;
      }
  }
//resends packet if the timer for that packet runs out
  static EVENT_HANDLER(timeouts)  {
      printf("timeout, seq=%d\n", ackexpected);
      transmit_frame(lastmsg, DL_DATA, lastlength, ackexpected);
  }
//starts the nodes and gets them ready to send and recieve data
  void reboot_node(CnetEvent ev, CnetTimerID timer, CnetData data) {

    CNET_set_handler(EV_APPLICATIONREADY, send_data, 0);
    CNET_set_handler(EV_PHYSICALREADY, receive_data, 0);
    CNET_enable_application(ALLNODES);
  }
