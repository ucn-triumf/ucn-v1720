

int HadEvent_m0[8]={0,0,0,0,0,0,0,0};
int HadEvent_m1[8]={0,0,0,0,0,0,0,0}
int verb=1;
int BaselineSetValue;
int Base[8];
int EventCount=0;


void DCOFFSETCALC(int sum1[8], int sum0[8], int moduleID, int NumEvents[8])
{
  if (moduleID==0)////////////////////////////added May 27, 2016
    {
      int returnval;
      if (verb ==1)
	{
	  std::cout<<"ModuleID: "<<moduleID<<endl;
	  std::cout<<"EventCount: "<<EventCount<<endl;
	}
   for (ch=0;ch<MaxNChannels; ch++)////////added May 26, 2016
    {
      // std::cout<<"NumEvents: "<<NumEvents[ch]<<endl;
      if (NumEvents[ch]!=0) HadEvent_m0[ch]++;
      if (verb ==1) if (NumEvents[ch] != 0) std::cout<<"Base: "<<sum0[ch]/NumEvents[ch]<<endl;
      if(NumEvents[ch]!=0 && HadEvent_m0[ch]==1)
	{
	  Base[ch]=(int)(sum0[ch]/NumEvents[ch]);//gets the average basline of the channel
	                                        //and sets that as Base[ch]
	  if (verb ==1) std::cout<<"Channel: "<<ch<<" Base: "<<Base[ch]<<endl;
	}
    else if (NumEvents[ch]!=0 && abs(BaselineSetValue - ((int)(sum0[ch]/NumEvents[ch]))) >= 20)
	{
	  //std::cout<<"Channel "<<ch<<" of board "<<moduleID<<" was weird"<<endl;
	  Base[ch]=(int)((sum0[ch]/NumEvents[ch]));//gets the average basline of the channel
	                                         //and sets that as Base[ch]
	  if (verb ==1) std::cout<<"Channel: "<<ch<<" Base: "<<Base[ch]<<endl;
	   config.DCoffset[ch] =(int)(13.37*(Base[ch]-BaselineSetValue)+32767.5);
	   returnval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, ch, config.DCoffset[ch]);
	    
	}
      if (ch==0 && HadEvent_m0[0]==1)//sets the target baseline for all offsets to adjust to
	{
	      BaselineSetValue = Base[ch];
	      if (verb==1)
		{
		  std::cout<<"BaselineSetValue: "<<BaselineSetValue<<endl;
		  std::cout<<"Module ID: "<<moduleID<<endl;
		}
	}
    }
    int z=0;
    for (z=0; z<MaxNChannels; z++)//////////added May 26, 2016
    {
      if (Base[z]!=0 && HadEvent_m0[z]==2)
	{ 
	  config.DCoffset[z] =(int)(13.37*(Base[z]-BaselineSetValue)+32767.5);//derived equation for calculating DC Offset
	  returnval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, z, config.DCoffset[z]);//this is where the DC Offset is actually changed
	}
      if (verb ==1) std::cout<<"Channel: "<<z<<" DC Offset: "<<config.DCoffset[z]<<endl;
    }
    }

  if (this->_moduleID==1)//added May 27, 2016 //have separate if statements for each module allows the offsets for one module to change
                                              //while on the other module they stay the same
    {
 int returnval;
 if (verb ==1)
   {
     std::cout<<"ModuleID: "<<this->_moduleID<<endl;
     std::cout<<"EventCount: "<<EventCount<<endl;
   }
   for (ch=0;ch<MaxNChannels; ch++)////////added May 26, 2016
    {
      // std::cout<<"NumEvents: "<<NumEvents[ch]<<endl;
      if (NumEvents[ch]!=0) HadEvent_m1[ch]++;
      if (verb ==1)
	{
	  if (NumEvents[ch]!=0) std::cout<<"BaselineSetValue: "<<BaselineSetValue<<endl;
	  if (NumEvents[ch] != 0) std::cout<<"Base: "<<sum1[ch]/NumEvents[ch]<<endl;
	}
      if((NumEvents[ch]!=0 && HadEvent_m1[ch]==1))
	{
	  Base[ch]=(int)((sum1[ch]/NumEvents[ch]));//gets the average basline of the channel
	                                         //and sets that as Base[ch]
	  if (verb ==1)  std::cout<<"Channel: "<<ch<<" Base: "<<Base[ch]<<endl;
	}
      else if (NumEvents[ch]!=0 && abs(BaselineSetValue - ((int)(sum1[ch]/NumEvents[ch]))) >= 20)
	{
	  Base[ch]=(int)((sum1[ch]/NumEvents[ch]));//gets the average basline of the channel
	                                         //and sets that as Base[ch]
	  if (verb ==1) std::cout<<"Channel: "<<ch<<" Base: "<<Base[ch]<<endl;
	   config.DCoffset[ch] =(int)(13.37*(Base[ch]-BaselineSetValue)+32767.5);
	   returnval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, ch, config.DCoffset[ch]);
	    
	}
    }

    int z=0;
    for (z=0; z<MaxNChannels; z++)//////////added May 26, 2016
    {
      if ((Base[z]!=0 && HadEvent_m1[z]==1))
	{ 
	  config.DCoffset[z] =(int)(13.37*(Base[z]-BaselineSetValue)+32767.5);
	  returnval |= CAEN_DGTZ_SetChannelDCOffset(_device_handle, z, config.DCoffset[z]);
	}
      if (verb ==1) std::cout<<"Channel: "<<z<<" DC Offset: "<<config.DCoffset[z]<<endl;
    }
    }
  EventCount++;/////////added May 26, 2016
  if(verb ==1)
    {
      if (this->_moduleID==0)
	{
	  for (ch=0; ch<8; ch++)
	    {
	      std::cout<<"had event "<<ch<<": "<<HadEvent_m0[ch]<<endl;
	    }
	}
      if (this->_moduleID==1)
	{
	  for (ch=0; ch<8; ch++)
	    {
	      std::cout<<"had event "<<ch<<": "<<HadEvent_m1[ch]<<endl;
	    }
	}
    }
}
