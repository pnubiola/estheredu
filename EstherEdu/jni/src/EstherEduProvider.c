/*
 * EstherEduProvider.c
 *
 *  Created on: 11/08/2014
 *      Author: Pere Nubiola
 */


/* This file is part of EstherEdu.

    EstherEdu is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EstherEdu is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EstherEdu.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <jni.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <httpfunc.h>
#include <sys/ioctl.h>

#define BUFFERSIZE 0x4000

typedef struct {
	int popen;
	int mypipe[2];
	int bopen;
	int udp;
	int http;
	httpmem * mem;
	EEssl *bio;
	char *cProtocol;
	char *cDomain;
	int iPort;
	char *cPath;
	char *cQuery;
	char *cUser;
	char *cPassw;
	char *cIdent;
	char boundary[15];
} photoData;

static photoData * tmp = NULL;

typedef enum { http , https ,ftp , ftps , sftp , estheredu} protocols;
long getyearseconds(){
	struct tm *tc;
	struct timespec t;

	if (clock_gettime((clockid_t) CLOCK_REALTIME , &t)) return 0;
	tc = gmtime( &(t.tv_sec));
	return (tc->tm_yday * 86400l) + (tc->tm_hour * 3600) + (tc->tm_min *  60) + tc->tm_sec;
}

char *copyjstring(jstring ori ,JNIEnv *env){
	const char *w;
	char * ret;
	size_t l;

	l = (size_t)(*env)->GetStringUTFLength(env , ori);
	if (!l) return NULL;

	w = (*env)->GetStringUTFChars(env , ori , 0);

	ret = (char *)malloc( l + 1);
	strcpy(ret , w );

	(*env)->ReleaseStringUTFChars(env , ori , w);
	return ret;

}

JNIEXPORT void JNICALL cat_nubiola_estheredu_initialize(JNIEnv *env, jobject obj,
		jstring jProtocol , jstring jDomain , jint port , jstring jPath , jstring jQuery,jstring jUser , jstring jPassw , jstring jIdent){
	tmp = (photoData*) calloc(1,sizeof(photoData));

	tmp->cProtocol = copyjstring(jProtocol , env);

	tmp->cDomain = copyjstring(jDomain , env);

	tmp->iPort = (int) port;

	tmp->cPath = copyjstring(jPath , env);

	tmp->cUser = copyjstring(jUser , env);

	tmp->cPassw = copyjstring(jPassw , env);
	tmp->cIdent = copyjstring(jIdent , env);
}

void freestaticvar(photoData * p){
	if (p->popen){
		close( p->mypipe[0]);
		close( p->mypipe[1]);
	}
	if (p->bopen){
		eeclose(p->bio);
	}
	free(p->cProtocol);
	free(p->cDomain);
	free(p->cPath);
	free(p->cUser);
	free(p->cPassw);
	free(p);
}

int initHttp(photoData *p){
	char *crln = "\r\n";
	int len , rtc;
	char *buf;

	sprintf(p->boundary,"$&_%.7lx",getyearseconds());

	if (getHttpPostHeader(p->bio,p->cPath) == -1) return -1;
	if (getHttpConnectionHeader(p->bio , 1) == -1) return -1 ;
	if (getHttpDateHeader(p->bio) == -1) return -1;
	if (getHttpTransferEncodingHeader(p->bio,1) == -1) return -1;
	if (getHttpAceptHeader(p->bio) == -1) return -1;
	if (getHttpAceptCharsetHeader(p->bio) == -1) return -1;
	if (getHttpHost(p->bio,p->cDomain, p->iPort) == -1) return -1;
	if (getHttpContentType(p->bio,p->boundary) == -1) return -1;
	if (getHttpContentEncoding(p->bio) == -1) return -1;
	if (getHttp100Continue(p->bio) == -1) return -1;
	if (eewriteb(p->bio , crln , strlen(crln) , NULL ,0) == -1) return -1;

	rtc = eeread(p->bio , &buf , &len, 5);
	if (rtc == 0){
		if ( len == 0) return 0;
		char tk[2] = "-" , *pos;
		//response format  HTTP-Version SP Status-Code SP Reason-Phrase CRLF any digits
		pos = strtok(buf , tk);
		pos = strtok(NULL, tk);
		if (!strcmp(pos , "100")){
			free(buf);
			return 0;
		}
		free(buf);
		freestaticvar(p);
	}
	return -1;
}

void *readloop(void * dummy){
    fd_set rfds;
    int rtc , init , prot ;
    char buf[BUFFERSIZE];
    httpmem *mem;
    photoData *p = (photoData *) dummy;

    if (!strcmp(p->cProtocol, "http")) prot = 1;
    else if (!strcmp(p->cProtocol, "https")) prot = 2;
    else if (!strcmp(p->cProtocol, "ftp")) prot = 3;
    else if (!strcmp(p->cProtocol, "ftps")) prot = 4;
    else if (!strcmp(p->cProtocol, "sftp")) prot = 5;
    else prot = 6;

    FD_ZERO(&rfds);
    FD_SET(p->mypipe[0], &rfds);

    while (1){
    	rtc = select(1, &rfds, NULL, NULL, 0);
    	if (rtc < 0){
    		freestaticvar(p);
        	return NULL;
    	}
    	if (!init){
    		if (prot > 0 && prot < 6) {
    			p->bio = initssl(prot == 2 || prot == 4 ? 1 : 0 , p->cDomain, p->iPort);
    			if (!p->bio) {
    	    		freestaticvar(p);
    				return NULL;
    			}
    			p->bopen = 1;
    		}
    		if (prot > 0 && prot < 3){
    			if (initHttp(p)) {
    	    		freestaticvar(p);
    			};
    			mem = setChunkSize(16384);
    			if (p->cQuery && strlen(p->cQuery)){

					if (getboundaryHeaderQuery(p->bio,mem, p->boundary, p->cQuery , 1) < 0) {
						freestaticvar(p);
						freeChunk(mem);
						return NULL;
					}
    			}
				if (getboundaryHeaderFile(p->bio,mem, p->boundary, p->cIdent ,1 ) < 0) {
					freestaticvar(p);
					freeChunk(mem);
					return NULL;
				}
    		}
    		init = 1;
    	}
    	if (!(rtc = read(p->mypipe[0] , buf, BUFFERSIZE))){
    		if (getboundary(p->bio,mem, p->boundary, 1 , 0 ) < 0){
				freestaticvar(p);
				freeChunk(mem);
				return NULL;

    		}
    		while((rtc = getChunk(p->bio,mem, NULL , 0 , 1 , 0)) > 0) ;
			freestaticvar(p);
			freeChunk(mem);
			return NULL;
    	}
    	if ((rtc = getChunk(p->bio,mem, buf , rtc , 1 , 1)) < 0){
			freestaticvar(p);
			freeChunk(mem);
			return NULL;
    	}
    }

    return NULL;

}

JNIEXPORT jint JNICALL cat_nubiola_estheredu_getFd(JNIEnv *env, jobject obj){

	int err , s;
	char sport[10];
	jclass IOException ;
    pthread_t thr;
	pthread_attr_t attr , *attrp;

	IOException = (*env)->FindClass(env ,"java.io.IOException");

	if (tmp->iPort < 1) {
		if (!strcmp(tmp->cProtocol , "http")) tmp->iPort = 80;
		else if (!strcmp(tmp->cProtocol , "https")) tmp->iPort = 443;
		else if (!strcmp(tmp->cProtocol , "ftp")) tmp->iPort = 21;
		else if (!strcmp(tmp->cProtocol , "ftps")) tmp->iPort = 21;
		else if (!strcmp(tmp->cProtocol , "sftp")) tmp->iPort = 22;
		else if (!strcmp(tmp->cProtocol , "estheredu")) tmp->iPort = 22355;
	}

	if (strcmp(tmp->cProtocol , "estheredu")){
		//protocol is connection oriented
		tmp->udp = 1;
	}


    if (pipe2(tmp->mypipe , O_CLOEXEC | O_CLOEXEC)){
 		freestaticvar(tmp);
 		(*env)->ThrowNew(env ,IOException, strerror(errno) );
 		return 0;
    }
    tmp->popen = 1;
    s = pthread_attr_init(&attr);
    attrp= &attr;
    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if ((s = pthread_create(&thr, &attr , readloop , tmp))){
		freestaticvar(tmp);
    	(*env)->ThrowNew(env ,IOException, "Can not create thread" );
    	if (attrp != NULL) s = pthread_attr_destroy(attrp);
 		return 0;
    }
    if (attrp != NULL) s = pthread_attr_destroy(attrp);
    s = tmp->mypipe[1];
    tmp = NULL;
    return s;
}

JNIEXPORT void JNICALL cat_nubiola_estheredu_freeMem(JNIEnv *env, jobject obj){
	if (tmp) freestaticvar(tmp);
}
