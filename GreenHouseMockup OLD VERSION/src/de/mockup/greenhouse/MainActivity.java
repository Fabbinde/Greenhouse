package de.mockup.greenhouse;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity {

	private static final String ACTION_TEMP = "de.mockup.greenhouse.action.TEMP";
	
	Button testSendButton;
	Button startServiceButton;
	Button endServiceButton;
	EditText paramEditText;
	EditText editTextIp;
	TextView answerTextView;

	private Socket socket;

	Handler updateConversationHandler;

	private static final int SERVERPORT = 80;
	private String SERVER_IP = "192.168.2.101";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		editTextIp = (EditText) findViewById(R.id.editTextIp);
		testSendButton = (Button) findViewById(R.id.button_testSend);
		startServiceButton = (Button) findViewById(R.id.buttonStartService);
		endServiceButton = (Button) findViewById(R.id.buttonEndService);
		paramEditText = (EditText) findViewById(R.id.editText_Param);
		answerTextView = (TextView) findViewById(R.id.textView_Answer);

		updateConversationHandler = new Handler();

		startServiceButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				startNewService(v);
			}
		});

		endServiceButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				stopNewService(v);
			}
		});

		testSendButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				try {
					// TODO - Vielleicht nur ausf�hren wenn der Klick gemacht
					// wird und
					// danach wieder beenden, damit Arduino nicht blockiert

					SERVER_IP = editTextIp.getText().toString();
					new Thread(new ClientThread()).start();
					String str = paramEditText.getText().toString();
					PrintWriter out = new PrintWriter(new BufferedWriter(
							new OutputStreamWriter(socket.getOutputStream())),
							true);
					out.println(str);
					/* NEW */
					out.close();
				} catch (UnknownHostException e) {
					e.printStackTrace();
				} catch (IOException e) {
					e.printStackTrace();
				} catch (Exception e) {
					e.printStackTrace();
				}

			}
		});

	}

	@Override
	protected void onStop() {
		super.onStop();
		try {
			if (!socket.isClosed()) {
				socket.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
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

	public void startNewService(View view) {

		// Intent Service:
		//getApplicationContext().startService(new Intent(getApplicationContext(), GreenHouseIntentService.class));
		Intent intent = new Intent(getApplicationContext(), GreenHouseIntentService.class);
		intent.setAction(ACTION_TEMP);
		getApplicationContext().startService(intent);
		// Normaler Service:
		// startService(new Intent(this, GreenHouseService.class));
	}

	public void stopNewService(View view) {
		// stopService(new Intent(this, GreenHouseService.class));
	}

	class ClientThread implements Runnable {

		private BufferedReader input;

		@Override
		public void run() {

			try {
				InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
				// InetAddress serverAddr =
				// InetAddress.getByName(editTextIp.toString());

				socket = new Socket(serverAddr, SERVERPORT);
				// socket.setSoTimeout(3000);
				this.input = new BufferedReader(new InputStreamReader(
						socket.getInputStream()));
				// int connectionTime = 0;
				String read = input.readLine();
				updateConversationHandler.post(new updateTextThread(read));
				/*
				 * while (!Thread.currentThread().isInterrupted()) {
				 * Log.i("HIER!!!", "DA333"); if (connectionTime == 5) { return;
				 * } try { if (input.ready()) {
				 * 
				 * String read = input.readLine(); Log.i("DEBUG:::::", "Read: "
				 * + read); updateConversationHandler .post(new
				 * updateTextThread(read)); } } catch (IOException e) {
				 * e.printStackTrace(); } Thread.sleep(1000); connectionTime++;
				 * }
				 */

			} catch (UnknownHostException e1) {
				e1.printStackTrace();
			} catch (IOException e1) {
				e1.printStackTrace();
			} /*
			 * catch (InterruptedException e) { // TODO Auto-generated catch
			 * block e.printStackTrace(); }
			 */

		}

	}

	class updateTextThread implements Runnable {
		private String msg;

		public updateTextThread(String str) {
			this.msg = str;
		}

		@Override
		public void run() {
			answerTextView.setText(msg);
		}
	}

}
