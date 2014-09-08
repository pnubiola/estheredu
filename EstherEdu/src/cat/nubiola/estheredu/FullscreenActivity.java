/*
 * FullscreenActivity.java
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

import java.net.URI;

import cat.nubiola.estheredu.util.SystemUiHider;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Spinner;

/**
 * full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 *
 * @see SystemUiHider
 */
public class FullscreenActivity extends Activity implements OnFocusChangeListener, Handler.Callback {
	
	private static final String TAG = "EstherEduActivity";

	final int ZXING_SCAN = 1;
	final int LA_DROID_SCAN = 2;
	final int IMAGE_CAPTURE = 3;

	final int PHOTO = 1;
	final int QUIT = 2;
	
	Spinner spiProtocol = null;
	ArrayAdapter<String> adapter = null;
	EditText edDomain = null;
	EditText edPort = null;
	EditText edPath = null;
	EditText edQuery = null;
	EditText edUser = null;
	EditText edPassword = null;
	ImageButton btPhoto = null;
	ImageButton btCR = null;
	
	SharedPreferences preferences = null;
	Handler handler;
	
	private void buttonEnable(){
		if(((String)spiProtocol.getSelectedItem()).isEmpty() || edDomain.getText().toString().isEmpty() ||
				edPort.getText().toString().isEmpty() || edPath.getText().toString().isEmpty() || edPath.getText().toString().isEmpty()) {
			btPhoto.setClickable(false);
			btPhoto.setEnabled(false);
			Log.v(TAG, "buttonEnable:" + false);
			return;
		}
		btPhoto.setClickable(true);
		btPhoto.setEnabled(true);		
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Log.v(TAG , "onCreate: init");
		setContentView(R.layout.activity_fullscreen);
		
		spiProtocol = (Spinner) findViewById(R.id.protocol);
		adapter = new ArrayAdapter<String>(this , android.R.layout.simple_list_item_1, Protocols.getList());
		spiProtocol.setAdapter(adapter);
		spiProtocol.setOnItemSelectedListener(new OnItemSelectedListener(){

			@Override
			public void onItemSelected(AdapterView<?> parent, View view,
					int position, long id) {
				buttonEnable();
				
			}

			@Override
			public void onNothingSelected(AdapterView<?> parent) {
				buttonEnable();
				
			}
			
		});
		
		edDomain = (EditText) findViewById(R.id.domainName);
		edDomain.setOnFocusChangeListener(this);

		edPort = (EditText) findViewById(R.id.port);
		edPort.setOnFocusChangeListener(this);

		edPath = (EditText) findViewById(R.id.path);
		edPath.setOnFocusChangeListener(this);

		edQuery = (EditText) findViewById(R.id.query);
		edQuery.setOnFocusChangeListener(this);

		edUser = (EditText) findViewById(R.id.username);

		edPassword = (EditText) findViewById(R.id.password);
		
		btPhoto = (ImageButton) findViewById(R.id.buttonPhoto);
		btPhoto.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				
				Log.v(TAG , "btPhoto.onClick: init");				
				EstherEduProvider.protocol = (String) spiProtocol.getSelectedItem();
				EstherEduProvider.domain = edDomain.getText().toString();
				EstherEduProvider.port = Integer.parseInt(edPort.getText().toString());
				EstherEduProvider.path = edPath.getText().toString();
				EstherEduProvider.user = edUser.getText().toString();
				EstherEduProvider.passw = edPassword.getText().toString();
				
				handler.sendEmptyMessage(PHOTO);
				Log.v(TAG , "btPhoto.onClick: end");				
				
			}
			
		});
		
		btCR = (ImageButton) findViewById(R.id.configuraQR);
		btCR.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				int req = ZXING_SCAN;
				Log.v(TAG , "btCR.onClick: init");				
				Intent intent = new Intent("com.google.zxing.client.android.SCAN"); 
				if (intent.resolveActivity(getPackageManager()) != null){
					intent.putExtra("SCAN_MODE", "QR_CODE_MODE");
					Log.v(TAG , "btCR.onClick: Intent Zxing");
				}
				else{
					req = LA_DROID_SCAN;
					intent = new Intent("la.droid.qr.scan"); 					
					if (intent.resolveActivity(getPackageManager()) != null){
		            	intent.putExtra( "la.droid.qr.complete" , true);
						Log.v(TAG , "btCR.onClick: Intent Droid QR");
					}
					else{
						AlertDialog.Builder b = new AlertDialog.Builder(FullscreenActivity.this);
						b.setTitle("Error reading QR");
						b.setMessage("No QR reader available");
						b.setIcon(android.R.drawable.ic_dialog_alert);
						b.show();
						Log.w(TAG , "btCR.onClick: end No know Barcode Reader");
						return;					
					}	            	
				}
            	startActivityForResult(intent, req);
				Log.v(TAG , "btCR.onClick: end Intent Launched");
			}
			
		});

		preferences = getPreferences(MODE_PRIVATE );
		if (preferences.contains("protocol")) 
			spiProtocol.setSelection(adapter.getPosition(preferences.getString("protocol", "http")));
		edDomain.setText(preferences.getString("domain", ""));
		edPort.setText(String.valueOf(preferences.getInt("domain", 0)));
		edPath.setText(preferences.getString("path", ""));
		edUser.setText(preferences.getString("user", ""));
		edPassword.setText(preferences.getString("passw", ""));
		buttonEnable();
		handler = new Handler(this);
		Log.v(TAG , "onCreate: end");
	}

	@Override
	public void onFocusChange(View v, boolean hasFocus) {
		if (!hasFocus) buttonEnable();
		
	}
	private void dispatchTakePictureIntent() {
		Log.v(TAG , "dispatchTakePictureIntent: init");
	    Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
	    takePictureIntent.putExtra(MediaStore.EXTRA_OUTPUT, EstherEduProvider.CONTENT_URI);
	    if (takePictureIntent.resolveActivity(getPackageManager()) != null) {
	        startActivityForResult(takePictureIntent, IMAGE_CAPTURE);
			Log.v(TAG , "dispatchTakePictureIntent: intent take photo launched");
	    }
		Log.v(TAG , "dispatchTakePictureIntent: end");
	}
	@Override
	public boolean handleMessage(Message msg) {
		switch(msg.what){
			case PHOTO:
				dispatchTakePictureIntent();
				Log.v(TAG , "handleMessage: PHOTO");
				return true;
			case QUIT:
				Log.v(TAG , "handleMessage: QUIT");
				finish();
				return true;
		}
		Log.v(TAG , "handleMessage: unknow message");
		return false;
	}
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		Log.v(TAG , "onActivityResult: init");
		if (resultCode == RESULT_OK){
			URI uri;
			String wr = null;
			if (requestCode == IMAGE_CAPTURE ){
				handler.sendEmptyMessage(PHOTO);
				Log.v(TAG , "onActivityResult: end photo captured");
				return;
			}
			else if (requestCode == ZXING_SCAN ){
				wr = data.getStringExtra("SCAN_RESULT");
			}
			else if(requestCode == LA_DROID_SCAN) {
				wr = data.getStringExtra("la.droid.qr.result");
			}
			else {
				AlertDialog.Builder b = new AlertDialog.Builder(this);
				b.setTitle("Error reading QR");
				b.setMessage("No result available");
				b.setIcon(android.R.drawable.ic_dialog_alert);
				b.show();
				Log.w(TAG , "onActivityResult: end unknow intent");
				return;					
			}
			Log.w(TAG , "Svan Result: " + wr );
			try {
				uri = URI.create(wr);
			} catch (IllegalArgumentException e) {
				Log.w(TAG , "onActivityResult: end uri exception:" + e.getMessage());
				return;
			}
			wr = uri.getScheme().toLowerCase();
			int dport = wr.equals("http") ? 80 : wr.equals("https") ? 443 : wr.equals("ftp") ? 21 : wr.equals("ftps") ? 990 : 
				wr.equals("sftp") ? 22 : wr.equals("estheredu") ? 22355 : -1;
			if (dport == -1 ){
				AlertDialog.Builder b = new AlertDialog.Builder(this);
				b.setTitle("Error reading QR");
				b.setMessage("Malformed uri reading qr scanner: Protocol not accepted:\n");
				b.setIcon(android.R.drawable.ic_dialog_alert);
				b.show();
				Log.w(TAG , "onActivityResult: end Malformed uri reading qr scanner: Protocol not accepted " + wr);
				return;
			}
			spiProtocol.setSelection(adapter.getPosition(preferences.getString("protocol", wr.isEmpty()?"http":wr)));
			wr = uri.getHost().toLowerCase();
			edDomain.setText(wr == null || wr.isEmpty()?"":wr);
			int port = uri.getPort();
			if (port == -1 ) port = dport;
			edPort.setText(String.valueOf(port));
			wr = uri.getPath();
			edPath.setText(wr == null || wr.isEmpty()?"":wr);
			wr = uri.getQuery();
			edQuery.setText(wr == null || wr.isEmpty()?"":wr);
			wr = uri.getUserInfo();
			if (wr == null || wr.isEmpty()){
				edUser.setText("");
				edPassword.setText("");
			}else if(wr.indexOf(':') != -1 ){
				edUser.setText(wr.substring(0, wr.indexOf(':')));
				edPassword.setText(wr.indexOf(':') + 1);
			}else{
				edUser.setText(wr);
				edPassword.setText("");
			}
			buttonEnable();
			Log.v(TAG , "onActivityResult: end uri accepted");
		}else if (resultCode == RESULT_CANCELED && requestCode == IMAGE_CAPTURE){
			Log.v(TAG , "onActivityResult: end uri accepted");
			handler.sendEmptyMessage(QUIT);
		}else {
			Log.v(TAG , "onActivityResult: end ignored Result");			
		}
	}


}
