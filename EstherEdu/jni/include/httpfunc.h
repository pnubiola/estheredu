/*
 * httpfunc.h
 *
 *  Created on: 09/08/2014
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

#ifndef HTTPFUNC_H_
#define HTTPFUNC_H_

typedef struct _httpmem httpmem;
#include <sslfunc.h>

int getHttpPostHeader(EEssl *bio,char *path);
int getHttpConnectionHeader(EEssl *bio,int close);
int getHttpDateHeader(EEssl *bio);
int getHttpTransferEncodingHeader(EEssl *bio,int chunked);
int getHttpAceptHeader(EEssl *bio);
int getHttpAceptCharsetHeader(EEssl *bio);
int getHttpAuthorizacion(EEssl *bio,char *user , char *passw);
int getHttpHost(EEssl *bio,char *domain, int port);
int getHttpProxyAuth(EEssl *bio,char *user , char *password );
int getHttpContentType(EEssl *bio,char* boundary);
int getHttpContentEncoding(EEssl *bio);
int getHttp100Continue(EEssl *bio);

int getboundary(EEssl *bio,httpmem *mem, char *boundary, int end , int comp );
int getboundaryHeaderQuery(EEssl *bio,httpmem *mem, char *boundary, char *name , int comp);
int getboundaryHeaderFile(EEssl *bio,httpmem *mem, char *boundary, char *name , int comp);

httpmem * setChunkSize(int size );

void freeChunk(httpmem * m);
int getChunk(EEssl *bio,httpmem *mem, char *addBuffer , int addBufferLen, int comp , int b64);

#endif /* HTTPFUNC_H_ */
