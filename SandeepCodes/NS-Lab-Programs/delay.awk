BEGIN {
   FS="[ /]+";
packet[packet_id]=0;
Packet_count = 0;
Aggergate_delay=0;
count1=0;
Average_delay=0;
count=0;
start_time[packet_id]=0;
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
   

#   printf("%s %s %s %s %s\n", action,time,type,pktsize,packet_id);

 if(type == "17") #type==6; 6 means TCP packet  17 means UDP packet
   {

   if ( packet[packet_id] != 1 )
      {
        packet[packet_id] = 1;
        Packet_count=Packet_count+1;
      }


   if ( start_time[packet_id] == 0 ) 
        start_time[packet_id] = time;

   if (  action != "d" )
     {

      if ( action == "r" )
         {
         end_time[packet_id] = time;
         }
     }


   }

}
END {

    for ( packet_id = 0; packet_id <= Packet_count; packet_id++ )

    {
       start = start_time[packet_id];
       end = end_time[packet_id];
       delay = end - start;
      # printf("%f \n", delay);
       if(delay >0.000000)
       {
         Aggergate_delay=Aggergate_delay+delay;
         count=count+1;
       }
   }
   # printf("%f \n", count);
     Average_delay=(Aggergate_delay/count);
     printf("Average_delay=%f Seconds \n", Average_delay);
 
	
}




