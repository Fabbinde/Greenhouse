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

import android.app.IntentService;
import android.content.Intent;
import android.util.Log;

/**
 * An {@link IntentService} subclass for handling asynchronous task requests in
 * a service on a separate handler thread.
 * <p>
 * TODO: Customize class - update intent actions, extra parameters and static
 * helper methods.
 */
public class GreenHouseIntentService extends IntentService {
	// TODO: Rename actions, choose action names that describe tasks that this
	// IntentService can perform, e.g. ACTION_FETCH_NEW_ITEMS
	private static final String ACTION_TEMP = "de.mockup.greenhouse.action.TEMP";

	// TODO: Rename parameters
	private static final String EXTRA_PARAM1 = "de.mockup.greenhouse.extra.PARAM1";
	private static final String EXTRA_PARAM2 = "de.mockup.greenhouse.extra.PARAM2";

	private static final int SERVERPORT = 80;
	private static final String SERVER_IP = "192.168.1.136";
	private Socket socket;

	/**
	 * Starts this service to perform action Temp with the given parameters. If
	 * the service is already performing a task this action will be queued.
	 * 
	 * @see IntentService
	 */
	// TODO: Customize helper method
	/*
	 * public static void startActionTemp(Context context, String param1, String
	 * param2) { Intent intent = new Intent(context,
	 * GreenHouseIntentService.class); intent.setAction(ACTION_TEMP);
	 * intent.putExtra(EXTRA_PARAM1, param1); intent.putExtra(EXTRA_PARAM2,
	 * param2); context.startService(intent); }
	 */

	public GreenHouseIntentService() {
		super("GreenHouseIntentService");
	}

	@Override
	protected void onHandleIntent(Intent intent) {
		Log.i("INTENTHANDLER", "onHandleIntent");
		if (intent != null) {
			final String action = intent.getAction();
			if (ACTION_TEMP.equals(action)) {
				// final String param1 = intent.getStringExtra(EXTRA_PARAM1);
				// final String param2 = intent.getStringExtra(EXTRA_PARAM2);
				handleActionTemp("", "");
			}
		}
	}

	/**
	 * Handle action Temp in the provided background thread with the provided
	 * parameters.
	 */
	private void handleActionTemp(String param1, String param2) {
		new Thread(new ClientThread()).start();
		while (socket == null) {
			try {
				Thread.sleep(1000);
				Log.i("Warte", "Warte auf Clientthread");
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		String str = "O";
		PrintWriter out = null;
		try {
			out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(
					socket.getOutputStream())), true);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if (out != null) {
			Log.i("SENDEN", "Sende über out");
			out.println(str);
		}
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	class ClientThread implements Runnable {

		private BufferedReader input;

		@Override
		public void run() {

			try {
				InetAddress serverAddr = InetAddress.getByName(SERVER_IP);

				while (true) {
					Log.i("New Socket",
							"Neuer socket erstellen, da isConnected = false");
					socket = new Socket(serverAddr, SERVERPORT);
					while (socket.isConnected()) {
						Log.i("WARTE", "Warte auf neue Nachrichten");

						this.input = new BufferedReader(new InputStreamReader(
								socket.getInputStream()));
						
						if (input != null) {
							String read = input.readLine();
							Log.i("ANTWORT:", read);
						}
						Log.i("DA", "HIEr");
						try {
							Thread.sleep(1000);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}

			} catch (UnknownHostException e1) {
				e1.printStackTrace();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
			Log.i("DRAUSEN", "OUT");
		}

	}

}
