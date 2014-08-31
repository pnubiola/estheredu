/*
 * sslfunc.c
 *
 *  Created on: 24/08/2014
 *      Author: Pere Nubiola Radigales
 */


/* This file is part of EstherEdu.

    EstherEdu is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EstherEdu is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with EstherEdu.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <sslfunc.h>
#include <sys/select.h>

#define CA_PATH "/etc/security/cacerts/"


int verify_callback(int ok, X509_STORE_CTX *x509_ctx){
	return ok;
}

EEssl * initssl(int setssl, char *host, int port){
	SSL_CTX *ctx;
	SSL *ssl;
	BIO * bio;

	if (setssl){
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();
		//implementar nomes TLS version >= 1.0
		ctx = SSL_CTX_new(SSLv23_client_method());
		SSL_CTX_set_options(ctx ,SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
		SSL_CTX_load_verify_locations(ctx , NULL , CA_PATH);
		SSL_CTX_set_mode(ctx , SSL_MODE_AUTO_RETRY);
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER , verify_callback);
		bio = BIO_new_ssl_connect(ctx);
		BIO_get_ssl(bio , &ssl);
		SSL_set_tlsext_host_name(ssl , host);
	}else bio = BIO_new(BIO_s_connect());
	BIO_set_close(bio ,BIO_CLOSE);
	BIO_set_nbio(bio ,1);
	BIO_set_conn_hostname(bio, host);
	BIO_set_conn_int_port(bio , port);
	if (BIO_do_connect(bio) <= 1){
		BIO_free_all(bio);
		return (BIO *) 0;
	}

	if (setssl && BIO_do_handshake(bio) <= 0) {
		BIO_free_all(bio);
		return (BIO *) 0;
	}

	return bio;

}

int eewriteb(EEssl * bio ,char * buf1 , int len1 , char *buf2 , int len2){
	int ret = 0;
	if (BIO_eof(bio)) return -1;
	if (len1 > 0) ret += BIO_write(bio , buf1 , len1);
	if (len2 > 0) ret += BIO_write(bio , buf2 , len2);
	return ret;
}
int eeread(EEssl * bio , char **buf ,int *len, int timeout){
	struct timeval *t;
    fd_set rfds;
    int rtc;

    FD_ZERO(&rfds);
    FD_SET(BIO_get_fd(bio, NULL), &rfds);
    *len = 0;
    if (timeout = -1) t = 0 ;
    else{
    	t = calloc(1 , sizeof(struct timeval));
    	t->tv_sec = timeout;
    }
    *buf = NULL;
    while(1){
    	rtc = select(1, &rfds, NULL, NULL, t);
    	if (!rtc){
    		if (timeout != -1) free(t);
    		return 0;
    	}
    	int pend = BIO_pending(bio);
    	if (pend > 0) {
    		*buf = realloc(*buf , (*len) + pend);
    		BIO_read(bio , (*buf) + (*len) , pend);
    		(*len) *= pend;
    	}
    	if (BIO_eof(bio)){
    		if (timeout != -1) free(t);
    		BIO_free_all(bio);
    		return -1;
    	}
    	if (timeout != -1){
    		t->tv_sec =  0;
    		t->tv_usec = 0;
    	}
    }
}

void eeclose(EEssl * bio){
	BIO_free_all(bio);
}

