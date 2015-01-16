package de.htwg.greenhouseApp;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Messenger;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import de.htwg.greenhouseService.ArduinoDataBean;
import de.htwg.greenhouseService.GreenhouseArduinoService;
import de.htwg.greenhouseService.GreenhouseArduinoService.LocalBinder;
import de.htwg.greenhouseUtil.GreenhouseContentProvider;
import de.htwg.greenhouseUtil.GreenhouseUtils;
import de.htwg.greenhouseappnew2015.R;

public class MainActivity extends Activity {

	GreenhouseArduinoService greenhouseService;

	GreenhouseUtils greenhouseUtils = new GreenhouseUtils();

	Intent greenhouseIntent;

	ArduinoDataBean aBean;

	Button startServiceButton;
	Button stopServiceHandlerButton;
	Button startContentProviderButton;
	Button startDirectFromServiceButton;

	TextView textViewTemp1;
	TextView textViewHumiAir;

	// GreenhouseArduinoService mService = null;
	boolean mIsBound = false;
	boolean newDataLoaded = false;

	int serviceCheckIntervallOnBinded = 2000;
	int serviceCheckIntervallOnUnBinded = 5000;

	String URL = "content://de.htwg.greenhouseUtil.GreenhouseContentProvider/cte";

	private String LOG_TAG = this.getClass().getSimpleName();

	private MainActivity myActivity;
	
	ServiceConnection serviceConnection = new ServiceConnection() {

		@Override
		public void onServiceDisconnected(ComponentName name) {

			mIsBound = false;
			Toast.makeText(getBaseContext(),
					"Verbindung zum Greenhouse Service wurde unterbrochen!",
					Toast.LENGTH_LONG).show();
		}

		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			LocalBinder binder = (LocalBinder) service;
			greenhouseService = binder.getService();
			greenhouseService.addGuiListener(myActivity);
			
			
			mIsBound = true;
			Toast.makeText(getBaseContext(),
					"Verbindung zum Greenhouse Service wurde hergestellt!",
					Toast.LENGTH_LONG).show();

		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		aBean = new ArduinoDataBean();

		
		
		startServiceButton = (Button) findViewById(R.id.buttonStartService);
		stopServiceHandlerButton = (Button) findViewById(R.id.buttonStopHandler);
		startContentProviderButton = (Button) findViewById(R.id.buttonLoadDataFromContentProvider);
		startDirectFromServiceButton = (Button) findViewById(R.id.buttonLoadDataDirectFromService);

		textViewTemp1 = (TextView) findViewById(R.id.textViewTemp);
		textViewHumiAir = (TextView) findViewById(R.id.textViewHumi);

		startServiceButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				greenhouseIntent = new Intent(v.getContext(),
						GreenhouseArduinoService.class);
				startService(greenhouseIntent);

				if (bindService(new Intent(v.getContext(),
						GreenhouseArduinoService.class), serviceConnection,
						Context.BIND_IMPORTANT)) {
					mIsBound = true;

					// greenhouseService = new GreenhouseArduinoService();

				} else
					mIsBound = false;
			}
		});

		stopServiceHandlerButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				unbindService(serviceConnection);
				stopService(greenhouseIntent);
				mIsBound = false;
			}
		});

		startContentProviderButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				loadDataToGuiByProvider();
			}
		});

		startDirectFromServiceButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				if (mIsBound) {
					loadDataDirectFromService();
				} else {
					Toast.makeText(getBaseContext(),
							"Service nicht verbunden, kann Daten nicht laden!",
							Toast.LENGTH_LONG).show();
				}

			}
		});
		myActivity = this;
	}

	@Override
	protected void onPause() {
		if (mIsBound) {
			greenhouseService
					.setServiceCheckIntervall(serviceCheckIntervallOnUnBinded);
			unbindService(serviceConnection);
			mIsBound = false;

		}
		super.onPause();
	}

	@Override
	protected void onDestroy() {
		if (mIsBound) {
			greenhouseService
					.setServiceCheckIntervall(serviceCheckIntervallOnUnBinded);
			unbindService(serviceConnection);
			mIsBound = false;

		}
		super.onDestroy();
	}

	@Override
	protected void onResume() {
		Log.i(LOG_TAG,"onResume");
		greenhouseIntent = new Intent(getApplicationContext(),
				GreenhouseArduinoService.class);
		if (!mIsBound) {
			if (bindService(new Intent(this.getApplicationContext(),
					GreenhouseArduinoService.class), serviceConnection,
					Context.BIND_IMPORTANT)) {
				greenhouseService = new GreenhouseArduinoService();
				greenhouseService
						.setServiceCheckIntervall(serviceCheckIntervallOnBinded);
				greenhouseService.setNotificationTemperatureIsShowing(false);
				greenhouseService.setNotificationHumiAirIsShowing(false);
				mIsBound = true;
			}
		}
		super.onResume();
	}

	@Override
	protected void onRestart() {
		Log.i(LOG_TAG,"onRestart");
		greenhouseIntent = new Intent(getApplicationContext(),
				GreenhouseArduinoService.class);
		if (!mIsBound) {
			if (bindService(new Intent(this.getApplicationContext(),
					GreenhouseArduinoService.class), serviceConnection,
					Context.BIND_IMPORTANT)) {
				greenhouseService = new GreenhouseArduinoService();
				greenhouseService
						.setServiceCheckIntervall(serviceCheckIntervallOnBinded);
				greenhouseService.setNotificationTemperatureIsShowing(false);
				greenhouseService.setNotificationHumiAirIsShowing(false);
				mIsBound = true;
			}
		}
		super.onRestart();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	public void loadDataToGuiByProvider() {

		Uri data = Uri.parse(URL);
		Cursor c = getContentResolver().query(data, null, null, null,
				"timestamp");
		String result = "";

		if (!c.moveToFirst()) {
			Log.e(LOG_TAG + "loadDataToGui()", " Cant put into cp");
		} else {
			while (c.moveToNext()) {
				if (c.isLast()) {
					result = c
							.getString(c
									.getColumnIndex(GreenhouseContentProvider.jsonData));
					break;
				}
			}
		}
		// Ab hier hat man nun alle Daten vom Arduino
		this.aBean = greenhouseUtils.StringToBean(result);
		loadDataFromBeanToGui();
	}

	public void loadDataDirectFromService() {
		this.aBean = greenhouseService.getDataBean();

		if (this.aBean != null) {
			newDataLoaded = true;
		}
		loadDataFromBeanToGui();
	}

	public void loadDataFromBeanToGui() {

		textViewTemp1.setText(String.valueOf(this.aBean.getCurrentTemp_1()));
		textViewHumiAir.setText(String.valueOf(this.aBean.getCurrentHumiAir()));

	}
	
	public void addBeanListener(ArduinoDataBean bean) {
		Log.i("HALLO VON ACTIVITY", String.valueOf(bean.getCurrentTemp_1()));
		this.aBean = bean;
	}

}
