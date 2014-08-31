/*
 * httpfunc.c
 *
 *  Created on: 09/08/2014
 *      Author: Pere Nubiola Radigales
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

#include "httpfunc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <math.h>
#include <zlib.h>

struct _httpmem {
	int chunkSize ;
	char *chunkBuffer ;
	int chunkOutPos ;
	int chunkInPos ;
	int chunkBufferSize ;
};

int getHttpPostHeader(EEssl *bio ,char *path){
	int rtc ;
	char *text = "Post %s HTTP/1.1\r\n";
	char *ret = malloc(strlen(text) + strlen(path) + 1);
	sprintf(ret , text, path);
	rtc = eewriteb(bio , ret , strlen(ret), NULL , 0);
	free(ret);
	return rtc;
}

int getHttpConnectionHeader(EEssl *bio, int close){
	char *text = "Connection: close\r\n";
	if (close) {
		return eewriteb(bio , text , strlen(text) , NULL , 0);
	}
	return 0;
}

int getHttpDateHeader(EEssl *bio){
	int rtc;
	struct timespec t;
	struct tm *tc;
	char *mon[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	char *wkday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	char *text = "Date: %s, %.2d %s %.4d %.2d;&.2d%.2d GMT\r\n";
	char *ret = (char *) malloc(35);
	if (clock_gettime((clockid_t) CLOCK_REALTIME , &t)) return NULL;
	tc = gmtime( &(t.tv_sec));
	sprintf(ret , text, wkday[tc->tm_wday] , tc->tm_mday, mon[tc->tm_mon],tc->tm_year,tc->tm_hour, tc->tm_min , tc->tm_sec );
	rtc = eewriteb(bio , ret , strlen(ret) , NULL , 0);
	free(ret)
	return rtc;
}

int getHttpTransferEncodingHeader(EEssl *bio,int chunked){
	char *ret = "Transfert-Encoding: chunked\r\n";
	if (chunked){
		return eewriteb(bio ,ret , strlen(ret) , NULL , 0);
	}
	return 0;
}

int getHttpAceptHeader(EEssl *bio){
	char *text = "Acept: text/plain, text/html\r\n";
	return eewriteb(bio , text , strlen(text) , NULL , 0);
}

int getHttpAceptCharsetHeader(EEssl *bio){
	char *text = "Accept-Charset: utf-8";
	return eewriteb(bio , text , strlen(text) , NULL , 0);
}

int base64encodeStart(BIO *bio[]){
	bio[0] = BIO_new(BIO_f_base64());
	bio[1] = BIO_new(BIO_s_mem());
	bio[0] = BIO_push(bio[0], bio[1]);
	//BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
	return 0;
}
int base64encodeWrite(BIO *bio[] ,const char *message , size_t len , char **buffer){
	size_t outlen;
	BIO_write(bio[0], message, len);
	outlen = BIO_ctrl_pending(bio[1]);
	if (outlen>0) {
		*buffer = (char *) calloc(outlen + 1, 1);
		BIO_read(bio[1], *buffer ,outlen);
	}
	return outlen;
}
int base64encodeEnd(BIO *bio[] ,char **buffer){
	BUF_MEM *ptr;
	BIO_flush(bio[0]);
	BIO_get_mem_ptr(bio[0] , &ptr);
	(*buffer) = (char *) calloc((size_t) ptr->length  , (size_t) sizeof(char));
	memcpy(*buffer , ptr->data , ptr->length -1 );

	BIO_free_all(bio[0]);
	return ptr->length -1;
}

int base64encode(const char * message , char **buffer){
	BIO *bio, *b64;
	BUF_MEM *ptr;
	int encodedSize = 4*ceil((double) strlen(message));

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bio);
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
	BIO_write(b64, message, strlen(message));
	BIO_flush(b64);
	BIO_get_mem_ptr(b64 , &ptr);

	(*buffer) = (char *) calloc((size_t) ptr->length  , (size_t) sizeof(char));
	memcpy(*buffer , ptr->data , ptr->length -1 );

	BIO_free_all(bio);

	return (0); //success

}

int getHttpAuthorizacion(EEssl *bio, char *user , char *passw){
	int rtc;
	char * up ;
	char *upo = (char *) malloc(strlen(user) + strlen(passw) + 2);

	base64encode(upo , &up);

	char * ret = (char *) malloc( strlen(up) + 23);
	sprintf(ret , "Authorization: Basic %s\r\n",up);
	rtc = eewriteb(bio , ret , strlen(ret), NULL , 0);
	free(upo);
	free(up);
	free(ret);
	return rtc;
}

int getHttpHost(EEssl *bio,char *domain, int port){
	int rtc;
	char *ret = (char *) malloc(strlen(domain) + 14);
	if (port != 0 ) sprintf(ret , "Host: %s:%d\r\n",domain , port);
	else sprintf(ret , "Host: %s\r\n",domain);
	rtc = eewriteb(bio , ret , strlen(ret), NULL , 0);
	free(ret);
	return rtc;
}

int getHttpProxyAuth(EEssl * bio,char *user , char *password ) {
	char * up ;
	char *upo = (char *) malloc(strlen(user) + strlen(password) + 2);

	base64encode(upo , &up);

	char * ret = (char *) malloc( strlen(up) + 30);
	sprintf(ret , "PROXY-Authorization: Basic %s\r\n",up);
	rtc = eewriteb(bio , ret , strlen(ret), NULL , 0);
	free(upo);
	free(up);
	free(ret);
	return rtc;

}

int getHttpContentType(EEssl *bio, char* boundary){
	char *text="Content-Type: multipart/form-data; boundary=%s\r\n";
	char *ret = (char *) malloc(strlen(text) + strlen(boundary));
	sprintf(ret ,text,boundary);
	rtc = eewriteb(bio , ret , strlen(ret), NULL , 0);
	free(ret);
	return rtc;
}
int getboundary(EEssl *bio,httpmem *mem, char *boundary, int end , int comp ){
	char *buff = char* malloc(strlen(boundary + 5 + (end) ? 2 : 0));
	sprintf(buf ,"--%s%s\r\n",boundary , end ? "--" : "");
	int rtc = getChunk(bio,mem, buf , stgrlen(buf), comp , 0);
	free(buf);
	return rtc;
}

int getHttp100Continue(EEssl *bio){
	char *text="Expect: 100-continue\r\n";
	return eewriteb(bio , text , strlen(text), NULL , 0);
}
int getHttpContentEncoding(EEssl *bio){
	char *text="Content-Encoding: gzip\r\n";
	return eewriteb(bio , text , strlen(text), NULL , 0);
}

httpmem * setChunkSize(int size){
	struct _httpmem * ret = (struct _httpmem *) calloc( 1,sizeof(struct _httpmem));
	ret->chunkSize = size;
	ret->chunkBuffer = (char *) malloc(size);
	ret->chunkBufferSize = size;
	return ret
}
void freeChunck(httpmem * m){
	free(m);
}
#define windowBits 15
#define GZIP_ENCODING 16

static int strm_init (z_stream * strm){
    strm->zalloc = Z_NULL;
    strm->zfree  = Z_NULL;
    strm->opaque = Z_NULL;
    if (deflateInit2 (strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) < 0) return -1 ;
    return 0;
}

static int strm_next (z_stream * strm , char **ret , int end , int chunkSize){
	char *buffer ;
	int rtc , flush, len = 0;
	buffer = (char *) malloc(chunkSize);
	*ret = NULL;
	flush = end ? Z_FINISH : Z_NO_FLUSH;
	do {
		strm->avail_out = chunkSize;
		strm->next_out = buffer;
        rtc = deflate(strm, Z_NO_FLUSH);    /* no bad return value */
        if (rtc == Z_STREAM_ERROR) return -1;
        len += chunkSize - strm->avail_out;
        *ret = realloc(ret , len);
        memcpy(*ret ,strm->next_out ,  chunkSize - strm->avail_out);
	}while(end ? (rtc != Z_STREAM_END) : (strm->avail_in != 0));
	free(buffer);
	if (end) deflateEnd(strm);
	return len;
}

int getChunk(EEssl *bio,httpmem *mem, char *addBuffer , int addBufferLen, int comp , int b64){
	int ret = -1 , lenb ;
	static int isb64 = 0, compinit = 0;
	static BIO *bio[2];
	static z_stream strm;
	char * buff, *final;

	//conver to b64 , gzip and add to buffer
	if (addBufferLen){
		buff = addBuffer;
		lenb = addBufferLen;
		if (b64 && !isb64){
			//init b64
			base64encodeStart(bio);
			isb64 = 1;
		}
		if (isb64 && b64){
			// must be convert to base64
			base64encodeWrite(bio ,addBuffer , addBufferLen , &buff);
			lenb = strlen(buff);
		}else if(isb64){
			//end base 64
			base64encodeEnd(bio ,&buff);
			lenb = strlen(buff);
			if(addBufferLen){
				buff = (char *) realloc(buff , lenb + addBufferLen);
				memcpy(buff + lenb , addBuffer , addBufferLen);
				lenb += addBufferLen;
			}
		}
		//prepare  to gzip
		final = buff;
		if (comp) {
			if (!compinit){
				strm_init(&strm);
				compinit = 1;
			}
			strm.avail_in = lenb;
			strm.next_in = buff;
			lenb = strm_next (&strm , &final , 0 , mem->chunkSize );
			if (lenb < 0){
				if (isb64){
					free(buff);
					isb64 = 0;
				}
				return -1;
			}

		}
		//increment buffer size if is necesari
		if (mem->chunkBufferSize < (mem->chunkInPos + addBufferLen)){
			mem->chunkBufferSize = (int) ceil((mem->chunkInPos + addBufferLen)/mem->chunkSize);
			mem->chunkBuffer = (char *) realloc(mem->chunkBuffer,mem->chunkBufferSize);
		}
		//lenb add to buffer
		if (lenb > 0) {
			memcpy(mem->chunkBuffer + mem->chunkInPos , final , lenb);
			mem->chunkInPos += lenb;
		}
		//free
		if (isb64){
			free(buff);
			isb64 = b64;
		}
	} else if(compinit){
		lenb = strm_next (&strm , &final , 1);
		if (lenb < 0){
			return -1;
		}
		compinit = 0;
	}
	//free final if compressed
	if (comp && lenb){
		free(final);
	}
	char londes[15];
	if (!addBufferLen || (mem->chunkInPos - mem->chunkOutPos) < mem->chunkSize){
		ret = (mem->chunkInPos - mem->chunkOutPos) < mem->chunkSize ? mem->chunkInPos - mem->chunkOutPos : mem->chunkSize;
		sprintf(londes,"%z\r\n",ret);
		eewriteb(bio , londes , strlen(londes), mem->chunkBuffer , ret );
		if (ret) {
			memmove(mem->chunkBuffer , mem->chunBuffer + ret ,  mem->chunkBufferSize - ret );
			l = (int) ceil((mem->chunkBufferSize - ret) / mem->chunkSize) * mem->chunkSize;
			if (l < mem-chunkVufferSize){
				mem->chunkBufer = (char *) realloc(mem->chunkBufer , l);
			}
			mem->chunkBufferSize -= ret;
			mem->chunkInPos -= ret;
		}else{
			dest = NULL;
			free(chunkBuffer);
			free(mem)
			eeclose(bio);
		}
		return ret;
	}
	return -1;
}

