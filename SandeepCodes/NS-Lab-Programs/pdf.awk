BEGIN {
      FS="[ /]+";
packet_id=0;
Packet_count=0;
Packet_drop_count=0;
Packet_reached_count=0;
pdf=0;
packet[packet_id]=0;
}

{
   action = $1;
   time = $2;
   if(action == "r"){
      type = $41;
   pktsize = $48;
   packet_id = $39;
   }
   else{
      type = $41;
   pktsize = $48;
   packet_id = $39;
   }

   if ( packet[packet_id] == 0 )
      {
        packet[packet_id] = 1;
        Packet_count=Packet_count+1;
      }

   if (action == "d" )
      {
        Packet_drop_count=Packet_drop_count+1;
      }
}

END {
     #printf("%d \n",count2);
     Packet_reached_count=Packet_count - Packet_drop_count;
     pdf=(Packet_reached_count/Packet_count)*100;

     printf("\nNumber of Packet Sent=%f  \n",Packet_count);
     printf("Number of Packet Dropped=%f  \n",Packet_drop_count);
     printf("Number of Packet Received=%f  \n",Packet_reached_count);
     printf("Packet Delivery Fraction (PDF)=%f percent  \n\n",pdf);	
}




