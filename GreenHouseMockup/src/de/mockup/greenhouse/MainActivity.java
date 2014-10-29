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
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity {

	Button testSendButton;
	EditText paramEditText;
	TextView answerTextView;

	private Socket socket;

	Handler updateConversationHandler;

	private static final int SERVERPORT = 80;
	private static final String SERVER_IP = "192.168.2.101";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		testSendButton = (Button) findViewById(R.id.button_testSend);
		paramEditText = (EditText) findViewById(R.id.editText_Param);
		answerTextView = (TextView) findViewById(R.id.textView_Answer);

		new Thread(new ClientThread()).start();

		updateConversationHandler = new Handler();

		testSendButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {

				try {
					String str = paramEditText.getText().toString();
					PrintWriter out = new PrintWriter(new BufferedWriter(
							new OutputStreamWriter(socket.getOutputStream())),
							true);
					out.println(str);
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
			socket.close();
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

	class ClientThread implements Runnable {

		private BufferedReader input;

		@Override
		public void run() {

			try {
				InetAddress serverAddr = InetAddress.getByName(SERVER_IP);

				socket = new Socket(serverAddr, SERVERPORT);
				this.input = new BufferedReader(new InputStreamReader(
						socket.getInputStream()));
				while (!Thread.currentThread().isInterrupted()) {

					try {

						String read = input.readLine();
						updateConversationHandler.post(new updateTextThread(
								read));

					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			} catch (UnknownHostException e1) {
				e1.printStackTrace();
			} catch (IOException e1) {
				e1.printStackTrace();
			}

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
