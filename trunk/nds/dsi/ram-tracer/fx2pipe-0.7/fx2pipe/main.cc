/*
 * usb_io/main.cc
 * 
 * FX2 pipe program main routine. 
 * 
 * Copyright (c) 2006--2009 by Wolfgang Wieser ] wwieser (a) gmx <*> de [ 
 * 
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 as published by the Free Software 
 * Foundation. (See COPYING.GPL for details.)
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 */

#include "../oconfig.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sched.h>

#include "../fx2pipe/fx2pipe.h"

#include <assert.h>


volatile int caught_sigint=0;

static void SigIntHandler(int)
{
	++caught_sigint;
	if(caught_sigint>5)
	{
		const char *msg=
			"fx2pipe: caught too many SIGINT; exiting ungracefully\n";
		write(2,msg,sizeof(msg));
		_exit(1);
	}
}


static void PrintHelp()
{
	fprintf(stderr,
		"USAGE: fx2pipe [option|assignment]...\n"
		"Long options:\n"
		"  --help       print this\n"
		"  --version    print version information\n"
		"Short options: (Can be compined into single option)\n"
		"  -i           run in IN direction, i.e. read data from USB EP6 (default)\n"
		"  -o           run in OUT direction, i.e. write data to USB EP2\n"
		"  -0           no stdio; send NULs / throw away read data (for timing)\n"
		"  -8,-w        use 8bit / 16bit (default) wide fifo bus on FX2 side\n"
		"  -2,-3,-4     use double, triple or quad (default) buffered FX2 fifo\n"
		"  -s,-a        run in sync (default) / async slave fifo mode\n"
		"  -O,-I        shortcut for -o0, -i0, respectively\n"
		"Assignments: (Leading '-' can be left away)\n"
		"  -d=NN        use n-th (unconfigured) FX2 device (start with NN=0, default)\n"
//		"  -d=BBB.DDD   use device DDD attached to bus BBB\n"
		"  -n=NNN       stop after NNN bytes; suffix k,M,G for mult. with 2^10,20,30\n"
		"  -bs=NNN      set IO block size to NNN, max 16384 (default 16384)\n"
		"  -ps=NN       set pipeline size (number of URBs; default 16)\n"
		"  -sched=P[,N] set scheduling policy P (\"fifo\" or \"rr\") and prio N\n"
		"  -fw=PATH     use specified firmware IHX file instead of built-in one\n"
		"               omit path to not download any firmware (just reset device)\n"
		"  -ifclk=[x|30[o]|48[o]][i] specify interface clock:\n"
		"               x -> external; 30,48 -> internal clock 30/48MHz, suffix 'o'\n"
		"               to enable output to IFCLK pin, 'i' to invert IFCLK\n"
		"  -cko=[12|24|48][o|z][i] specify 8051 frequency in MHz (default: 48) and\n"
		"               CLKOUT pin: output 'o' (default), tristate 'z', invert 'i'\n"
		"\n"
		"fx2pipe - pipe data in or out of an Cypress FX2 device (CY7C6801x[A])\n"
		"          Copyright (c) 2006--2009 by Wolfgang Wieser; License: GPL\n");
}


int main(int argc,char **arg)
{
	FX2Pipe p;
	
	int errors=0;
	int dir_spec=0;
	int fifo_width=2;  // 1 or 2 (8 or 16 bits)
	int fifo_nbuf=4;   // 2,3,4 for double, triple, quad buffered
	char fifo_mode='s';
	char ifclk_invert=0;
	char ifclk_speed=48;  // 0 -> external; 30,48 -> internal 30/48 MHz
	char ifclk_output=0;
	char cko_speed=48;
	char cko_output=1;
	char cko_invert=0;
	for(int i=1; i<argc; i++)
	{
		// First, check for long options. 
		if(*arg[i]=='-' && arg[i][1]=='-')
		{
			if(!strcmp(arg[i],"--help"))
			{  PrintHelp();  return(0);  }
			else if(!strcmp(arg[i],"--version"))
			{  fprintf(stderr,"fx2pipe version %s\n",VERSION);  return(0);  }
			else
			{  fprintf(stderr,"fx2pipe: unknown long option \"%s\"\n",arg[i]);
				++errors;  }
			
			continue;
		}
		
		// Next, check for assignments. One can leave away the leading '-'. 
		const char *ass_value=NULL;
		for(const char *c=arg[i]; *c; c++)
		{
			if(*c=='-')  continue;
			if(isalpha(*c) || isdigit(*c))  continue;
			if(*c=='_')  continue;
			if(*c=='=')
			{  ass_value=c+1;  break;  }
			break;
		}
		if(ass_value)
		{
			// This is an assignment. 
			const char *ass_name=arg[i];
			if(*ass_name=='-')  ++ass_name;
			
			if(!strncmp(ass_name,"n=",2))
			{
				char *endptr;
				p.transfer_limit=strtoll(ass_value,&endptr,0);
				switch(*endptr)
				{
					case 'k':  p.transfer_limit<<=10;  break;
					case 'M':  p.transfer_limit<<=20;  break;
					case 'G':  p.transfer_limit<<=30;  break;
					case '\0':  break;
					default:  fprintf(stderr,"fx2pipe: illegal byte limit spec "
						"\"%s\"\n",ass_value);  ++errors;  break;
				}
			}
			else if(!strncmp(ass_name,"d=",2))
			{
				if(strchr(ass_value,'.'))
				{
// FIXME: Handle BBB.DDD!
assert(0);
				}
				else
				{  p.n_th_usb_dev=strtol(ass_value,NULL,10);  }
			}
			else if(!strncmp(ass_name,"bs=",3))
			{
				char *endptr;
				p.io_block_size=strtoul(ass_value,&endptr,0);
				switch(*endptr)
				{
					case 'k':  p.io_block_size<<=10;  break;
					case '\0':  break;
					default:  fprintf(stderr,"fx2pipe: illegal block size spec "
						"\"%s\"\n",ass_value);  ++errors;  break;
				}
			}
			else if(!strncmp(ass_name,"ps=",3))
			{
				p.pipeline_size=strtol(ass_value,NULL,0);
				if(p.pipeline_size<1)
				{  p.pipeline_size=1;  }
			}
			else if(!strncmp(ass_name,"fw=",3))
			{
				p.firmware_hex_path=ass_value;
			}
			else if(!strncmp(ass_name,"ifclk=",6))
			{
				const char *s=ass_value;
				
				// Speed...
				if(*s=='x')
				{  ifclk_speed=0;  ++s;  }
				else if(*s=='3' && s[1]=='0')
				{  ifclk_speed=30;  s+=2;  }
				else if(*s=='4' && s[1]=='8')
				{  ifclk_speed=48;  s+=2;  }
				
				// Output? Only valid if not external. 
				if(*s=='o' && ifclk_speed!=0)
				{  ifclk_output=1;  ++s;  }
				
				// Invert?
				if(*s=='i')
				{  ifclk_invert=1;  ++s;  }
				
				if(*s)
				{  fprintf(stderr,"fx2pipe: invalid ifclk spec \"%s\".\n",
					ass_value);  ++errors;  }
			}
			else if(!strncmp(ass_name,"cko=",4))
			{
				const char *s=ass_value;
				
				// Speed...
				if(*s=='1' && s[1]=='2')
				{  cko_speed=12;  s+=2;  }
				else if(*s=='2' && s[1]=='4')
				{  cko_speed=24;  s+=2;  }
				else if(*s=='4' && s[1]=='8')
				{  cko_speed=48;  s+=2;  }
				
				// Output?
				if(*s=='o')
				{  cko_output=1;  ++s;  }
				else if(*s=='z')
				{  cko_output=0;  ++s;  }
				
				// Invert?
				if(*s=='i')
				{  cko_invert=1;  ++s;  }
				
				if(*s)
				{  fprintf(stderr,"fx2pipe: invalid cko spec \"%s\".\n",
					ass_value);  ++errors;  }
			}
			else if(!strncmp(ass_name,"sched=",6))
			{
				const char *s=ass_value;
				
				// Policy?
				if(!strncmp(s,"fifo",4))
				{  p.schedule_policy=SCHED_FIFO;  s+=4;  }
				else if(!strncmp(s,"rr",2))
				{  p.schedule_policy=SCHED_RR;  s+=2;  }
				
				// Priority?
				if(*s==',')
				{  p.schedule_priority=strtol(s,(char**)&s,0);  }
				
				if(*s)
				{  fprintf(stderr,"fx2pipe: invalid sched spec \"%s\".\n",
					ass_value);  ++errors;  }
			}
			else
			{  fprintf(stderr,"fx2pipe: unknown assignment \"%s\"\n",
				ass_name[i]);  ++errors;  }
			continue;
		}
		
		// Assume short option if it has one leading dash. 
		if(arg[i][0]=='-' && (isalpha(arg[i][1]) || isdigit(arg[i][1])) )
		{
			for(const char *c=&arg[i][1]; *c; c++)
			{
				switch(*c)
				{
					case 'i':  p.dir=-1;  ++dir_spec;  break;
					case 'o':  p.dir=+1;  ++dir_spec;  break;
					case '8':  fifo_width=1;  break;
					case 'w':  fifo_width=2;  break;
					case '0':  p.no_stdio=1;  break;
					case '2':  fifo_nbuf=2;   break;
					case '3':  fifo_nbuf=3;   break;
					case '4':  fifo_nbuf=4;   break;
					case 's':  fifo_mode='s'; break;
					case 'a':  fifo_mode='a'; break;
					case 'O':  p.dir=+1;  ++dir_spec;  p.no_stdio=1;  break;
					case 'I':  p.dir=-1;  ++dir_spec;  p.no_stdio=1;  break;
					default:
						fprintf(stderr,"fx2pipe: unknown option '%c' in "
							"\"%s\"\n",*c,arg[i]);  ++errors;
						break;
				}
			}
			
			continue;
		}
		
		// Finally, what's left over...
		fprintf(stderr,"fx2pipe: unrecognized argument \"%s\"\n",
			arg[i]);  ++errors;
	}
	if(dir_spec>1)
	{
		fprintf(stderr,"fx2pipe: more than one direction specification "
			"(-iIoO).\n");
		++errors;
	}
	
	// FIXME: If fifo_width is 2, force even sizes!
	if(p.io_block_size<1 || p.io_block_size>16384)
	{
		fprintf(stderr,"fx2pipe: IO block size (bs) must be in "
			"range 1..16384, bs=%u is invalid\n",p.io_block_size);
		++errors;
	}
	
	// Set up config for firmware: 
	p.fc.FC_DIR = p.dir<0 ? 0x12U : 0x21U;
	p.fc.FC_CPUCS = 
		((cko_speed==12 ? 0U : cko_speed==24 ? 1U : 2U)<<3) | 
		(cko_invert ? 0x04U : 0x00U) | 
		(cko_output ? 0x02U : 0x00U);
	p.fc.FC_IFCONFIG = 
		(ifclk_speed ? 0x80U : 0x00U) |      // Internal (1) / external?
		(ifclk_speed==30 ? 0x00U : 0x40U) |  // 30 (0) / 48 (1) MHz?
		(ifclk_output ? 0x20U : 0x00U) |     // Enable (1) output to IFCLK?
		(ifclk_invert ? 0x10U : 0x00U) |     // Invert (1) IFCLK?
		(fifo_mode=='a' ? 0x08U : 0x00U) |   // Async (1) or sync (0) FIFO mode?
		0x00U |   // bit2 irrelevant for us...
		0x03U;    // bits 1,0 = 11 for slave FIFO mode
	if(p.dir<0)
	{
		// INPUT: USB->HOST
		p.fc.FC_EPCFG = 0xe0U;  // 1110 00BB with BB = 2,3,4times buffered?
		p.fc.FC_EPFIFOCFG = 0x0cU;    // 0000 110W (W = wordwide)
	}
	else
	{
		// OUTPUT: HOST->USB
		p.fc.FC_EPCFG = 0xa0U;  // 1010 00BB with BB = 2,3,4times buffered?
		p.fc.FC_EPFIFOCFG = 0x10U;    // 0001 000W (W = wordwide)
	}
	switch(fifo_nbuf)
	{
		case 2:  p.fc.FC_EPCFG|=0x02U;  break;
		case 3:  p.fc.FC_EPCFG|=0x03U;  break;
		case 4:  break;  // Do nothing. 
		default:  assert(0);  break;
	}
	if(fifo_width==2)
	{  p.fc.FC_EPFIFOCFG|=0x01U;  }
	
	// Dump firmware config values: 
	fprintf(stderr,"Firmware config: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
		(unsigned int)p.fc.FC_DIR,   (unsigned int)p.fc.FC_IFCONFIG,
		(unsigned int)p.fc.FC_EPCFG, (unsigned int)p.fc.FC_EPFIFOCFG,
		(unsigned int)p.fc.FC_CPUCS );
	
	if(errors)
	{  return(1);  }
	
	// Install signal handler for SIGINT. 
	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler=&SigIntHandler;
	sigaction(SIGINT,&sa,NULL);
	
	int ecode=0;
	
	ecode=p.run();
	
	return(ecode);
}
