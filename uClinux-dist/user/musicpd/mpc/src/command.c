/* mpc
 * (c) 2003-2004 by normalperson and Warren Dukes (shank@mercury.chem.pitt.edu)
 *              and Daniel Brown (danb@cs.utexas.edu)
 * This project's homepage is: http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "libmpdclient.h"
#include "list.h"
#include "charConv.h"
#include "password.h"
#include "util.h"
#include "status.h"
#include "command.h"
#include "mpc.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>

#define DIE(args...) do { fprintf(stderr, ##args); return -1; } while(0)

#define SIMPLE_CMD(funcname, libmpdclient_funcname, ret) \
int funcname ( int argc, char ** argv, mpd_Connection * conn) { \
        libmpdclient_funcname(conn); \
	printErrorAndExit(conn); \
        my_finishCommand(conn); \
	printErrorAndExit(conn); \
        return ret; \
}

#define SIMPLE_ONEARG_CMD(funcname, libmpdclient_funcname, ret) \
int funcname ( int argc, char ** argv, mpd_Connection * conn) { \
        libmpdclient_funcname(conn, toUtf8(argv[0])); \
	printErrorAndExit(conn); \
        my_finishCommand(conn); \
	printErrorAndExit(conn); \
        return ret; \
}

static void my_finishCommand(mpd_Connection * conn) {
	printErrorAndExit(conn);
	mpd_finishCommand(conn);
	printErrorAndExit(conn);
}

SIMPLE_CMD(cmd_next, mpd_sendNextCommand, 0)
SIMPLE_CMD(cmd_prev, mpd_sendPrevCommand, 0)
SIMPLE_CMD(cmd_stop, mpd_sendStopCommand, 0)
SIMPLE_CMD(cmd_clear, mpd_sendClearCommand, 0)
SIMPLE_CMD(cmd_shuffle, mpd_sendShuffleCommand, 0)
SIMPLE_CMD(cmd_kill, mpd_sendKillCommand, 0)

SIMPLE_ONEARG_CMD(cmd_save, mpd_sendSaveCommand, 0)
SIMPLE_ONEARG_CMD(cmd_rm, mpd_sendRmCommand, 0)

static mpd_Status * getStatus(mpd_Connection * conn) {
	mpd_Status * ret;

	mpd_sendStatusCommand(conn);
	printErrorAndExit(conn);

	ret = mpd_getStatus(conn);
	printErrorAndExit(conn);

	mpd_finishCommand(conn);
	printErrorAndExit(conn);

	return ret;
}

int isUrl(char * s) {
	char * t;

	t = strstr(s,"://");
	if(!t) return 0;

	while(s!=t) {
		if(!(*s >= 'A' && *s <= 'z')) return 0;
		s++;
	}

	return 1;
}

int cmd_add (int argc, char ** argv, mpd_Connection * conn ) 
{
	List ** lists;
	mpd_InfoEntity * entity;
	ListNode * node;
	mpd_Song * song;
	char * sp;
	char * dup;
	int i,j=0;
	int * arglens;
	int len;
	int ret;

        int lent,v=0;
        char * para=argv[0];
        char ** dest;
        char * dt; 
/*  *************************************/
		char tmpstr[2048], str[2048];
		int tlen=0;
        lent = strlen(argv[0]);
        
	    strcpy(str, argv[0]);
        tlen = strlen(str);
           for(i=0; i< tlen; i++)
               {
                   if(str[i] == '%')
                         tmpstr[j++] = ' ';
                   else if(str[i] == '@')
                         continue;
                   else
                         tmpstr[j++] = str[i];
 
               }
               
         tmpstr[j] = '\0';
         para = &tmpstr[0];

		/*****************************/

        for(i=0;i<lent;i++){
        if(para[i]==']')v++;
        }
	dest=malloc(sizeof(char **)*v);

        for (i=0;i<v;i++){
        dest[i] = strtok_r(para,"]",&dt);
        para=NULL;
        }

	lists = malloc(sizeof(List *)*v);
	arglens = malloc(sizeof(int)*v);

	/* convert ' ' to '_' */
	for(i=0;i<v;i++) {
		lists[i] = makeList(free);
		sp = dest[i];
		while((sp = strchr(sp,' '))) *sp = '_';
		arglens[i] = strlen(dest[i]);
	}

	/* get list of songs to add */
	mpd_sendListallCommand(conn,"");
	printErrorAndExit(conn);

	while((entity = mpd_getNextInfoEntity(conn))) {
		if(entity->type==MPD_INFO_ENTITY_TYPE_SONG) {
			song = entity->info.song;
			sp = dup = strdup(fromUtf8(song->file));
			while((sp = strchr(sp,' '))) *sp = '_';
			len = strlen(dup);
			for(i=0;i<v;i++) {
				if(len<arglens[i]) continue;
				ret = strncmp(dest[i],dup,
						arglens[i]);
				if(ret==0 && (dest[i][arglens[i]-1] == '/' ||
						dup[arglens[i]] == '\0' || 
						dup[arglens[i]] == '/')) 
				{
					insertInListWithoutKey(
							lists[i],
							strdup(fromUtf8(
									song->file)));
				}
			}
			free(dup);
		}
		mpd_freeInfoEntity(entity);
	}
	my_finishCommand(conn);

	/* send list of songs */
	mpd_sendCommandListBegin(conn);
	printErrorAndExit(conn);
	for(i=0;i<v;i++) {
		if(0 == lists[i]->numberOfNodes && isUrl(dest[i])) {
			printf("adding: %s\n",dest[i]);
			mpd_sendAddCommand(conn,toUtf8(dest[i]));
			printErrorAndExit(conn);
			continue;
		}	

		node = lists[i]->firstNode;
		while(node) {
			printf("adding: %s\n",(char *)node->data);
			mpd_sendAddCommand(conn,toUtf8(node->data));
			printErrorAndExit(conn);
			node = node->nextNode;
		}
		freeList(lists[i]);
	}
	mpd_sendCommandListEnd(conn);
	my_finishCommand(conn);

	/* clean up */
	free(lists);
	free(dest);
	return 0;
}

int cmd_del ( int argc, char ** argv, mpd_Connection * conn )
{
	int i,j;
	char * s;
	char * t;
	char * t2;
	int range[2];
	int songsDeleted = 0;
	int plLength = 0;
	char * songsToDel;
	mpd_Status * status;
	int len,v=0;
	char * para=argv[0];
	char ** dest;
	char * dt;
	len = strlen(argv[0]);
	
	for(i=0;i<len;i++){
	if(para[i]=='i')v++;
	}
	dest=malloc(sizeof(char **)*v);

	for (i=0;i<v;i++){
	dest[i] = strtok_r(para,"i",&dt);
	para=NULL;
	}
	
	status = getStatus(conn);

	plLength = status->playlistLength;

	songsToDel = malloc(plLength);
	memset(songsToDel,0,plLength);

	for(i=0;i<v;i++) {
		if(dest[i][0]=='#') s = &(dest[i][1]);
		else s = dest[i];

		range[0] = strtol(s,&t,10);
		if(s==t)
			DIE("error parsing song numbers from: %s\n",dest[i]);
		else if(*t=='-') {
			range[1] = strtol(t+1,&t2,10);
			if(t+1==t2 || *t2!='\0')
				DIE("error parsing range from: %s\n",dest[i]);
		}
		else if(*t==')' || *t=='\0') range[1] = range[0];
		else
			DIE("error parsing song numbers from: %s\n",dest[i]);

		if(range[0]<=0 || range[1]<=0 || range[1]<range[0])
			DIE("song numbers must be positive: %i-%i\n",range[0],range[1]);

		if(range[1]>plLength)
			DIE("song number does not exist: %i\n",range[1]);

		for(j=range[0];j<=range[1];j++) songsToDel[j-1] = 1;
	}

	mpd_sendCommandListBegin(conn);
	printErrorAndExit(conn);
	for(i=0;i<plLength;i++) {
		if(songsToDel[i]) {
			mpd_sendDeleteCommand(conn,i-songsDeleted);
			printErrorAndExit(conn);
			songsDeleted++;
		}
	}
	mpd_sendCommandListEnd(conn);
	my_finishCommand(conn);

	free(songsToDel);
	free(dest);
	return 0;
}

int cmd_play ( int argc, char ** argv, mpd_Connection * conn )
{
	int song;
	int i;
	if(0==argc) song = MPD_PLAY_AT_BEGINNING;
	else {
		for(i=0;i<argc-1;i++)
			printf("skipping: %s\n",argv[i]);

                if(!parse_songnum(argv[i], &song))
			DIE("error parsing song numbers from: %s\n",argv[i]);

		song--;
	}

	mpd_sendPlayCommand(conn,song);
	my_finishCommand(conn);

	return 0;
}

/* TODO: absolute seek times (normalperson) */
int cmd_seek ( int argc, char ** argv, mpd_Connection * conn )
{
	mpd_Status * status;
	char * arg = argv[0];
	char * test;
	int seekchange;
	char * last_char;
	int sec;
	double perc;
	int seekto;
        int rel = 0;

	status = getStatus(conn);
	
	if(status->state==MPD_STATUS_STATE_STOP)
		DIE("not currently playing\n");

        if(*arg == '+')
                rel = 1;
        else if(*arg == '-')
                rel = -1;

	last_char = &arg[strlen(arg)-1];

	if(*last_char == 's') {
		/* absolute seek (in seconds) */

		sec = strtol(arg,&test,10); /* get the # of seconds */

		if(*test!='s' || (!rel && sec<0))
			DIE("\"%s\" is not a positive number\n", arg);

		seekchange = sec;

	} else {
		/* percent seek */

		perc = strtod(arg,&test);

		if(*test!='\0' || (!rel && (perc<0 || perc>100)) ||
                                  (rel && perc>abs(100)))
			DIE("\"%s\" is not an number between 0 and 100\n",arg);

		seekchange = perc*status->totalTime/100+0.5;
	}

	seekto = (rel ? status->elapsedTime + seekchange : seekchange);

	if(seekto > status->totalTime)
		DIE("seek amount would seek past the end of the song\n");

	mpd_sendSeekIdCommand(conn,status->songid,seekto);
	printErrorAndExit(conn);
	my_finishCommand(conn);
	printErrorAndExit(conn);
	mpd_freeStatus(status);
	printErrorAndExit(conn);
	return 1;
}

int cmd_move ( int argc, char ** argv, mpd_Connection * conn )
{
	int from;
	int to;

	if(!parse_int(argv[0], &from) || from<=0)
		DIE("\"%s\" is not a positive integer\n",argv[0]);

	if(!parse_int(argv[1], &to) || to<=0)
		DIE("\"%s\" is not a positive integer\n",argv[1]);

	/* users type in 1-based numbers, mpd uses 0-based */
	from--;
	to--;

	mpd_sendMoveCommand(conn,from,to);
	my_finishCommand(conn);

	return 0;
}

int cmd_playlist ( int argc, char ** argv, mpd_Connection * conn )
{
	mpd_InfoEntity * entity;
	int count = 0;

	mpd_sendPlaylistInfoCommand(conn,-1);
	printErrorAndExit(conn);

	while((entity = mpd_getNextInfoEntity(conn))) {
		if(entity->type==MPD_INFO_ENTITY_TYPE_SONG) {
			mpd_Song * song = entity->info.song;

			printf("#%i) ", 1+count);
			pretty_print_song(song);
			//printf("\n");
			printf("|");

			count++;
		}
		mpd_freeInfoEntity(entity);
	}

	my_finishCommand(conn);

	return 0;
}

int cmd_listall ( int argc, char ** argv, mpd_Connection * conn )
{
	mpd_InfoEntity * entity;
	char * listall = "";
	int i=0;

	if(argc>0) listall = toUtf8(argv[i]);

	do {
		mpd_sendListallCommand(conn,listall);
		printErrorAndExit(conn);

		while((entity = mpd_getNextInfoEntity(conn))) {
			if(entity->type==MPD_INFO_ENTITY_TYPE_SONG) {
				mpd_Song * song = entity->info.song;
				printf("%s\n",fromUtf8(song->file));
			}
			mpd_freeInfoEntity(entity);
		}

		my_finishCommand(conn);

	} while(++i<argc&& (listall = toUtf8(argv[i])));

	return 0;
}

int cmd_update ( int argc, char ** argv, mpd_Connection * conn) 
{
	char * update = "";
	int i = 0;

	mpd_sendCommandListBegin(conn);
	printErrorAndExit(conn);

	if(argc > 0) update = toUtf8(argv[i]);

	do {
		mpd_sendUpdateCommand(conn, update);
	} while(++i<argc && (update = toUtf8(argv[i])));

	mpd_sendCommandListEnd(conn);
	printErrorAndExit(conn);
	mpd_finishCommand(conn);
	printErrorAndExit(conn);

	return 1;
}

int cmd_ls ( int argc, char ** argv, mpd_Connection * conn )
{
	mpd_InfoEntity * entity;
	char * ls = "";
	int i = 0;
	char * sp;
	char * dp;
	char **argvc;
	argvc = argv;
	
	if(argc>0) ls = toUtf8(argvc[i]);

	do {
		mpd_sendListallCommand(conn,"");

		sp = ls;
		while((sp = strchr(sp,' '))) *sp = '_';

		while((entity = mpd_getNextInfoEntity(conn))) {
			if(entity->type==MPD_INFO_ENTITY_TYPE_DIRECTORY) {
				mpd_Directory * dir = entity->info.directory;
				sp = dp = strdup(dir->path);
				while((sp = strchr(sp,' '))) *sp = '_';
				if(strcmp(dp,ls)==0) {
					free(dp);
					ls = dir->path;
					break;
				}
				free(dp);
			}
			mpd_freeInfoEntity(entity);
		}

		my_finishCommand(conn);

		mpd_sendLsInfoCommand(conn,ls);
		printErrorAndExit(conn);

		while((entity = mpd_getNextInfoEntity(conn))) {
			if(entity->type==MPD_INFO_ENTITY_TYPE_DIRECTORY) {
				mpd_Directory * dir = entity->info.directory;
				//printf("%s\n",fromUtf8(dir->path));
				printf("%s##",fromUtf8(dir->path));
			}
			else if(entity->type==MPD_INFO_ENTITY_TYPE_SONG) {
				mpd_Song * song = entity->info.song;
				//printf("%s\n",fromUtf8(song->file));
			 	printf("%s@|",fromUtf8(song->file));
                                printf("%s@#",(song->artist)); //sivaraman
                                printf("%s#|",(song->title));  //sivaraman
                                printf("%s||",(song->album));  //sivaraman

			}
			mpd_freeInfoEntity(entity);
		}

		my_finishCommand(conn);

	} while(++i<argc && (ls = toUtf8(argvc[i])));

	return 0;
}

int cmd_lsplaylists ( int argc, char ** argv, mpd_Connection * conn )
{
	mpd_InfoEntity * entity;
	char * ls = "";
	int i = 0;

	if(argc>0) ls = toUtf8(argv[i]);

	do {
		mpd_sendLsInfoCommand(conn,ls);
		printErrorAndExit(conn);

		while((entity = mpd_getNextInfoEntity(conn))) {
			if(entity->type==
					MPD_INFO_ENTITY_TYPE_PLAYLISTFILE) {
				mpd_PlaylistFile * pl = entity->info.playlistFile;
				printf("%s|",fromUtf8(pl->path));
			}
			mpd_freeInfoEntity(entity);
		}

		my_finishCommand(conn);

	} while(++i<argc && (ls = toUtf8(argv[i])));
	return 0;
}

int cmd_load ( int argc, char ** argv, mpd_Connection * conn )
{
	int i;
	char * sp;
	char * dp;
	mpd_InfoEntity * entity;
	mpd_PlaylistFile * pl;

	for(i=0;i<argc;i++) {
		sp = argv[i];
		while((sp = strchr(sp,' '))) *sp = '_';
	}

	mpd_sendLsInfoCommand(conn,"");
	printErrorAndExit(conn);
	while((entity = mpd_getNextInfoEntity(conn))) {
		if(entity->type==MPD_INFO_ENTITY_TYPE_PLAYLISTFILE) {
			pl = entity->info.playlistFile;
			dp = sp = strdup(fromUtf8(pl->path));
			while((sp = strchr(sp,' '))) *sp = '_';
			for(i=0;i<argc;i++) {
				if(strcmp(dp,argv[i])==0)
					strcpy(argv[i], fromUtf8(pl->path));
			}
			free(dp);
			mpd_freeInfoEntity(entity);
		}
	}
	my_finishCommand(conn);

	mpd_sendCommandListBegin(conn);
	printErrorAndExit(conn);
	for(i=0;i<argc;i++) {
		printf("loading: %s\n",argv[i]);
		mpd_sendLoadCommand(conn,toUtf8(argv[i]));
		printErrorAndExit(conn);
	}
	mpd_sendCommandListEnd(conn);
	my_finishCommand(conn);

	return 0;
}

int cmd_search ( int argc, char ** argv, mpd_Connection * conn ) 
{
	mpd_InfoEntity * entity;
	char * search;
	int table = -1;
	int i;

	struct search_types {
		const char * field;
		const int table;
	} mpc_search [] = {
		{"album", MPD_TABLE_ALBUM },
		{"artist", MPD_TABLE_ARTIST },
		{"title", MPD_TABLE_TITLE },
		{"filename", MPD_TABLE_FILENAME },
		{}
	};

	for(i=0;mpc_search[i].field;++i)
		if (! strcmp(mpc_search[i].field,argv[0]))
			table = mpc_search[i].table;
	if (-1==table) {
		fprintf(stderr,"\"%s\" is not one of: ", argv[0]);
		for(i=0;mpc_search[i].field;++i)
			fprintf(stderr,"%s%s%s",
					( !mpc_search[i+1].field ? "or " : ""),
					mpc_search[i].field,
					(  mpc_search[i+1].field ? ", "   : "\n")
			       );
		return -1;
	}

	for(i=1; i<argc && (search = toUtf8(argv[i])); i++)  {
		mpd_sendSearchCommand(conn,table,search);
		printErrorAndExit(conn);

		while((entity = mpd_getNextInfoEntity(conn))) {
			printErrorAndExit(conn);
			if(entity->type==MPD_INFO_ENTITY_TYPE_DIRECTORY) {
				mpd_Directory * dir = entity->info.directory;
				printf("%s\n",fromUtf8(dir->path));
			}
			else if(entity->type==MPD_INFO_ENTITY_TYPE_SONG) {
				mpd_Song * song = entity->info.song;
				printf("%s\n",fromUtf8(song->file));
			}
			mpd_freeInfoEntity(entity);
		}

		my_finishCommand(conn);
	}
	return 0;
}

int cmd_volume ( int argc, char ** argv, mpd_Connection * conn ) 
{
        struct int_value_change ch;

	if(argc==1) {
                if(!parse_int_value_change(argv[0], &ch))
			DIE("\"%s\" is not an integer\n", argv[0]);
	} else {
		mpd_Status *status;

		status = getStatus(conn);

		printf("volume:%3i%c   \n",status->volume,'%');

		mpd_freeStatus(status);

		return 0;
	}

	if (ch.is_relative)
		mpd_sendVolumeCommand(conn,ch.value);
	else 
		mpd_sendSetvolCommand(conn,ch.value);

	my_finishCommand(conn);
	return 1;
}

int cmd_pause ( int argc, char ** argv, mpd_Connection * conn )
{
	mpd_sendPauseCommand(conn,1);
	my_finishCommand(conn);

//	return 1;
	return 0;
}

int cmd_repeat ( int argc, char ** argv, mpd_Connection * conn )
{
	int mode;

	if(argc==1) {
		mode = get_boolean(argv[0]);
		if (mode < 0)
			return -1;
	}
	else {
		mpd_Status * status;
		status = getStatus(conn);
		mode = !status->repeat;
		mpd_freeStatus(status);
	}


	mpd_sendRepeatCommand(conn,mode);
	printErrorAndExit(conn);
	my_finishCommand(conn);
	printErrorAndExit(conn);

	return 1;
}

int cmd_random ( int argc, char ** argv, mpd_Connection * conn )
{
	int mode;

	if(argc==1) {
		mode = get_boolean(argv[0]);
		if (mode < 0)
			return -1;
	}
	else {
		mpd_Status * status;
		status = getStatus(conn);
		mode = !status->random;
		mpd_freeStatus(status);
	}

	mpd_sendRandomCommand(conn,mode);
	my_finishCommand(conn);

	return 1;
}

int cmd_crossfade ( int argc, char ** argv, mpd_Connection * conn )
{
	int seconds;

	if(argc==1) {
                if(!parse_int(argv[0], &seconds) || seconds<0)
			DIE("\"%s\" is not 0 or positive integer\n",argv[0]);

		mpd_sendCrossfadeCommand(conn,seconds);
		my_finishCommand(conn);
	}
	else {
		mpd_Status * status;
		status = getStatus(conn);

		printf("crossfade: %i\n",status->crossfade);

		mpd_freeStatus(status);
		printErrorAndExit(conn);
	}
	return 0;

}

int cmd_version ( int argc, char ** argv, mpd_Connection * conn )
{
	printf("mpd version: %i.%i.%i\n",conn->version[0],
			conn->version[1],conn->version[2]);
	return 0;
}

int cmd_loadtab ( int argc, char ** argv, mpd_Connection * conn )
{
	mpd_InfoEntity * entity;
	char * sp;
	mpd_PlaylistFile * pl;

	mpd_sendLsInfoCommand(conn,"");
	printErrorAndExit(conn);

	while((entity = mpd_getNextInfoEntity(conn))) {
		if(entity->type==MPD_INFO_ENTITY_TYPE_PLAYLISTFILE) {
			pl = entity->info.playlistFile;
			sp = pl->path;
			while((sp = strchr(sp,' '))) *sp = '_';
			if(argc==1) {
				if(strncmp(pl->path,argv[0],
							strlen(argv[0]))==0) {
					printf("%s\n",
							fromUtf8(pl->path));
				}
			}
		}
		mpd_freeInfoEntity(entity);
	}

	my_finishCommand(conn);
	return 0;
}

int cmd_lstab ( int argc, char ** argv, mpd_Connection * conn )
{
	mpd_InfoEntity * entity;
	char * sp;
	mpd_Directory * dir;

	mpd_sendListallCommand(conn,"");
	printErrorAndExit(conn);

	while((entity = mpd_getNextInfoEntity(conn))) {
		if(entity->type==MPD_INFO_ENTITY_TYPE_DIRECTORY) {
			dir = entity->info.directory;
			sp = dir->path;
			/* replace all ' ' with '_' */
			while((sp = strchr(sp,' '))) *sp = '_';
			if(argc==1) {
				if(strncmp(dir->path,argv[0],
							strlen(argv[0]))==0) {
					printf("%s\n",
							fromUtf8(dir->path));
				}
			}
		}
		mpd_freeInfoEntity(entity);
	}

	my_finishCommand(conn);

	return 0;
}

int cmd_tab ( int argc, char ** argv, mpd_Connection * conn )
{
	mpd_InfoEntity * entity;
	char * sp;
	mpd_Song * song;

	mpd_sendListallCommand(conn,"");
	printErrorAndExit(conn);

	while((entity = mpd_getNextInfoEntity(conn))) {
		if(entity->type==MPD_INFO_ENTITY_TYPE_SONG) {
			song = entity->info.song;
			sp = song->file;
			/* replace all ' ' with '_' */
			while((sp = strchr(sp,' '))) *sp = '_';
			if(argc==1) {
				if(strncmp(song->file,argv[0],
							strlen(argv[0]))==0) {
					printf("%s\n",
							fromUtf8(song->file));
				}
			}
			else printf("%s\n",fromUtf8(song->file));
		}
		mpd_freeInfoEntity(entity);
	}

	my_finishCommand(conn);
	return 0;
}

int cmd_status ( int argc, char ** argv, mpd_Connection * conn )
{
	return 1;
}
