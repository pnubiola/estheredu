/*
 * Protocols.java
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

package cat.nubiola.estheredu;

import java.util.ArrayList;
import java.util.List;

public enum Protocols {
	http,
	https,
	fttp,
	fttps,
	sftp,
	eshteredu;
	
	static List<String>getList(){
		ArrayList<String> ret = new ArrayList<String>();
		for( Protocols prot : values()  ){
			ret.add(prot.name());
		}
		return ret;
	}
}
