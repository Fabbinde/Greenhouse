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

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class GreenHouseService extends Service {
	private static final int SERVERPORT = 80;
	private static final String SERVER_IP = "192.168.1.136";
	private Socket socket;

	public GreenHouseService() {
	}

	@Override
	public IBinder onBind(Intent intent) {
		// TODO: Return the communication channel to the service.
		throw new UnsupportedOperationException("Not yet implemented");
	}

	@Override
	public void onCreate() {
		Toast.makeText(this, "The new Service was Created", Toast.LENGTH_LONG)
				.show();

	}

	@Override
	public void onStart(Intent intent, int startId) {
		// For time consuming an long tasks you can launch a new thread here...
		new Thread(new ClientThread()).start();
		try {
			// Hier auf ClientThread 2 Sekunden warten
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
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
			out.println(str);
		}

		Toast.makeText(this, " Service Started", Toast.LENGTH_LONG).show();

	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// TODO Auto-generated method stub
		Toast.makeText(this, " Service OnStartCommand", Toast.LENGTH_LONG)
				.show();
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		if (!socket.isClosed()) {
			try {
				socket.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		Toast.makeText(this, "Service Destroyed", Toast.LENGTH_LONG).show();

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
				if (input != null) {
					String read = input.readLine();
					Log.i("ANTWORT:", read);
				}

			} catch (UnknownHostException e1) {
				e1.printStackTrace();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		}

	}

}
