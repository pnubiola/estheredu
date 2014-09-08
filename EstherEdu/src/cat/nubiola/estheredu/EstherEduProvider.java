/*
 * EstherEduProvider.java
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

import java.io.FileNotFoundException;
import java.util.HashMap;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.os.ParcelFileDescriptor;
import android.telephony.TelephonyManager;
import android.util.Log;

public class EstherEduProvider extends ContentProvider {
	
	static {
		System.loadLibrary("estheredu");
	}
	private static final String TAG = "EstherEduPovider";
	public native void initialize(String Protocol , String Domain , int Port , String Path , String query ,String User , String Passw , String ident);
	
	private native int getFd();
	public native void freeMem();
	
	protected static String protocol;
	protected static String domain;
	protected static int port;
	protected static String path;
	protected static String query;
	protected static String user;
	protected static String passw;

	

	public static final Uri CONTENT_URI = Uri.parse("etheredu://cat.nubiola.estherdemo/");
	public static final HashMap<String ,String> MIME_TYPES = new HashMap<String,String>();

	static {
		MIME_TYPES.put(".jpg", "image/jpeg");
		MIME_TYPES.put(".jpeg", "image/jpeg");		
	}
	
	public int fd;
	@Override
	public boolean onCreate() {
		Log.v(TAG , "onCreate: init");
		initialize(protocol , domain , port , path , query , user.isEmpty()? "" : user , passw.isEmpty()? "" : passw , unicIdentifier());
		fd = getFd();
		getContext().getContentResolver().notifyChange(CONTENT_URI, null);
		Log.v(TAG , "onCreate: end fd=" + fd);
		return true;
	}

	@Override
	public String getType(Uri uri) {
		String path = uri.toString();
		Log.v(TAG , "getType: init uri=" + uri);
		for (String ext : MIME_TYPES.keySet()){
			if(path.endsWith(ext)){
				Log.v(TAG , "getType: end ext=" + ext);
				return (MIME_TYPES.get(ext));
			}
		}
		Log.v(TAG , "getType: end ext=unknow");
		return null;
	}

	@Override
	public ParcelFileDescriptor openFile(Uri uri, String mode)
			throws FileNotFoundException {
		Log.v(TAG , "openFile: called");
		return ParcelFileDescriptor.adoptFd(fd);
	}

	@Override
	public Cursor query(Uri uri, String[] projection, String selection,
			String[] selectionArgs, String sortOrder) {
		throw new RuntimeException("Operation npot Suported");
	}

	@Override
	public Uri insert(Uri uri, ContentValues values) {
		throw new RuntimeException("Operation npot Suported");
	}

	@Override
	public int delete(Uri uri, String selection, String[] selectionArgs) {
		throw new RuntimeException("Operation npot Suported");
	}

	@Override
	public int update(Uri uri, ContentValues values, String selection,
			String[] selectionArgs) {
		throw new RuntimeException("Operation npot Suported");
	}
	String unicIdentifier(){
		Log.v(TAG, "unicIdentifier: init");
		TelephonyManager tMgr =(TelephonyManager)getContext().getSystemService(Context.TELEPHONY_SERVICE);
		String t = null;
		String tp = "LN";
		if ((t = tMgr.getLine1Number()) == null || t.trim().length() == 0){
			tp = "SI";
			if((t = tMgr.getSubscriberId()) == null || t.trim().length() == 0){ 
				tp = "DI";
				if ((t = tMgr.getDeviceId()) == null || t.trim().length() == 0){
					tp = "SN";
					t = tMgr.getSimSerialNumber();
				}
			}
		}
		if (t != null &&  t.trim().length() != 0) {
			Log.v(TAG, "unicIdentifier: end return " + tp + t);
			return (tp + t);
		}
		Log.v(TAG, "unicIdentifier: end return null");
		return null;

	}

}
