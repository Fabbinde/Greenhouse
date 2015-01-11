package de.htwg.greenhouseApp;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;
import de.htwg.greenhouseService.GreenHouseArduinoService;
import de.htwg.greenhouseappnew2015.R;

public class MainActivity extends Activity {

	Intent service;

	Button startServiceButton;
	Button stopServiceHandlerButton;

	public void makeToast(String text) {
		Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		startServiceButton = (Button) findViewById(R.id.buttonLoadData);
		stopServiceHandlerButton = (Button) findViewById(R.id.buttonStopHandler);

		startServiceButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				 service = new Intent(v.getContext(),
				 GreenHouseArduinoService.class);
				 startService(service);
				 
				 /*
				  * TODO MORGEN
				  * - ContentProvider benutzen um die Daten abzuspeichern. 
				  * (hier dann beachten das mehrere Messwerte zur�ckkommen k�nnen
				  * , da der arduino ja alle speichert.) (am besten immer nicht mehr
				  * wie 20 Messwertzeitpunkte, dann lieber ein Flag mit�bergeben damit man hier
				  * weis ob der Arduino noch was nachsendet (bei 50 noch ok, bei 20000 Datens�tzen
				  * nicht mehr)
				  * - Activity greift dann auf den Contentprovider zu um auf die Daten vom Service 
				  * zugreifen zu k�nnen
				  * - Vielleicht auch einfach einen Zeitraum angeben, von wann bis wann (Nur f�r die Statistik wichtig)
				  * die aktuellen Werte reichen ja dann auch so aus.
				  * - Hier auf Android seite dann einfach die Werte die jede 2 Minuten geladen werden vom Arduino 
				  * im Service �berpr�fen und ggf. Alarm ausgeben wenn was nicht stimmt
				  * 
				  * TCP Verbindung l�uft noch nicht ganz, server checken!!
				  * 
				  * */
				 
				 
			}
		});

		stopServiceHandlerButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				stopService(service);
			}
		});

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
}
