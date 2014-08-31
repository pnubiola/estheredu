/*
 * sslfunc.h
 *
 *  Created on: 24/08/2014
 *      Author: perico
 */


 This file is part of EstherEdu.

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
#ifndef SSLFUNC_H_
#define SSLFUNC_H_

typedef ssl EEssl;

EEssl * initssl(int setssl, char *host, int port);
int eewrite(EEssl * bio ,struct msghdr *msg);
int eeread(EEssl * bio , char **buf ,int *len, int timeout);
int eewriteb(EEssl * bio ,char * buf1 , int len1 , char *buf2 , int len2);
void close(EEssl * bio);

#endif /* SSLFUNC_H_ */
