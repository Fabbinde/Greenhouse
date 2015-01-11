package de.htwg.greenhouseService;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.UnknownHostException;

import de.htwg.greenhouseApp.MainActivity;
import de.htwg.greenhouseappnew2015.R;
import de.htwg.greenhouseappnew2015.R.drawable;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

public class GreenHouseArduinoService extends Service {

	private final static int MESSAGE_CANCEL_HANDLER = 1;

	String ip = "87.146.231.60";
	int port = 80;

	int mStartMode; // indicates how to behave if the service is killed
	IBinder mBinder; // interface for clients that bind
	boolean mAllowRebind; // indicates whether onRebind should be used

	NotificationManager mNM;

	private Looper mServiceLooper;
	private ServiceHandler mServiceHandler;

	private boolean shouldStop = false;

	public GreenHouseArduinoService() {

	}

	public boolean isShouldStop() {
		return shouldStop;
	}

	public void setShouldStop(boolean shouldStop) {
		this.shouldStop = shouldStop;
	}

	// Handler that receives messages from the thread
	private final class ServiceHandler extends Handler {

		public ServiceHandler(Looper looper) {
			super(looper);
		}

		@Override
		public void handleMessage(Message msg) {
			// Normally we would do some work here, like download a file.
			// For our sample, we just sleep for 5 seconds.
			Log.i("Meldung vom Thread", " HALLO");
			while (true) {
					//getDataFromArduinoByTcp();
					//getDataFromArduinoByAsyncTcp();
				synchronized (this) {
					try {
						if (this.hasMessages(MESSAGE_CANCEL_HANDLER)) {
							this.removeMessages(MESSAGE_CANCEL_HANDLER);
							// Stop the service using the startId, so that we
							// don't stop
							// the service in the middle of handling another job
							Log.i("while(true)", "STOP");
							stopSelf(msg.arg1);
							break;
						}
						Log.i("while(true)", "Warte auf daten");

						wait(5000);
						
					} // TODO: handle exception
					catch (Exception e) {
					}
				}
			}

		}

	}

	private void showNotification() {
		// In this sample, we'll use the same text for the ticker and the
		// expanded notification
		CharSequence text = "Service gestartet!";

		// Set the icon, scrolling text and timestamp
		// Notification notification = new Notification(R.drawable.stat_sample,
		// text,
		// System.currentTimeMillis());
		Notification noti = new Notification.Builder(this)
				.setContentTitle("Greenhouse Service").setContentText(text)
				.setSmallIcon(R.drawable.ic_launcher).build();

		// The PendingIntent to launch our activity if the user selects this
		// notification
		PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
				new Intent(this, MainActivity.class), 0);

		// Set the info for the views that show in the notification panel.
		noti.setLatestEventInfo(this, "NOTI LABEL", text, contentIntent);

		// Send the notification.
		mNM.notify(1, noti);
	}

	public void makeToast(String text) {
		Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
	}

	private boolean getDataFromArduinoByAsyncTcp() {

		WorkerTask task = new WorkerTask();
		task.execute(new String[] { ip });

		return true;
	}

	private boolean getDataFromArduinoByTcp() throws UnknownHostException,
			IOException {
		String sentence;
		String modifiedSentence;
		Log.i("0", "0");
		Socket clientSocket = new Socket(ip, port);
		Log.i("1", "1");
		DataOutputStream outToServer = new DataOutputStream(
				clientSocket.getOutputStream());
		Log.i("2", "2");
		BufferedReader inFromServer = new BufferedReader(new InputStreamReader(
				clientSocket.getInputStream()));
		Log.i("3", "3");
		sentence = "HAAALLO :)";
		Log.i("4", "4");
		outToServer.writeBytes(sentence + '\n');
		Log.i("5", "5");
		modifiedSentence = inFromServer.readLine();
		Log.i("FROM SERVER: ", "ANTWORT = " + modifiedSentence);
		clientSocket.close();
		return true;
	}

	@Override
	public void onCreate() {
		// The service is being created
		// Start up the thread running the service. Note that we create a
		// separate thread because the service normally runs in the process's
		// main thread, which we don't want to block. We also make it
		// background priority so CPU-intensive work will not disrupt our UI.
		HandlerThread thread = new HandlerThread("ServiceStartArguments",
				android.os.Process.THREAD_PRIORITY_BACKGROUND);
		thread.start();

		// Get the HandlerThread's Looper and use it for our Handler
		mServiceLooper = thread.getLooper();
		mServiceHandler = new ServiceHandler(mServiceLooper);
		Log.i("onCreate", "CREATE");

		mNM = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
		// Display a notification about us starting. We put an icon in the
		// status bar.
		showNotification();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Toast.makeText(this, "service starting", Toast.LENGTH_SHORT).show();

		// For each start request, send a message to start a job and deliver the
		// start ID so we know which request we're stopping when we finish the
		// job
		Message msg = mServiceHandler.obtainMessage();
		msg.arg1 = startId;
		mServiceHandler.sendMessage(msg);
		Log.i("onStartCommand", "START_COMMAND");
		// If we get killed, after returning from here, restart
		return START_STICKY;
	}

	@Override
	public IBinder onBind(Intent intent) {
		// A client is binding to the service with bindService()
		Log.i("onBind", "BIND");
		return mBinder;
	}

	@Override
	public boolean onUnbind(Intent intent) {
		// All clients have unbound with unbindService()
		Log.i("onUnbind", "UNBIND");
		return mAllowRebind;
	}

	@Override
	public void onRebind(Intent intent) {
		// A client is binding to the service with bindService(),
		// after onUnbind() has already been called
		Log.i("onRebind", "REBIND");
	}

	@Override
	public void onDestroy() {
		// The service is no longer used and is being destroyed
		Log.i("onDestroy()", "DESTROY");
		Message m = new Message();
		m.what = MESSAGE_CANCEL_HANDLER;
		mServiceHandler.sendMessage(m);
	}

}