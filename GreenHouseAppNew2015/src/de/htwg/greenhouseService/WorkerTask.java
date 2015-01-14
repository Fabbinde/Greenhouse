package de.htwg.greenhouseService;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.UnknownHostException;

import android.os.AsyncTask;
import android.util.Log;

public class WorkerTask extends AsyncTask<String, Void, String> {

	int port = 80;

	@Override
	protected String doInBackground(String... urls) {
		String response = "";
		for (String url : urls) {
			String sentence;
			String modifiedSentence;
			Socket clientSocket;
			try {
				clientSocket = new Socket(url, port);

				DataOutputStream outToServer = new DataOutputStream(
						clientSocket.getOutputStream());
				BufferedReader inFromServer = new BufferedReader(
						new InputStreamReader(clientSocket.getInputStream()));
				sentence = "HAAALLO :)";
				outToServer.writeBytes(sentence + '\n');
				modifiedSentence = inFromServer.readLine();
				Log.i("FROM SERVER: ", "ANTWORT = " + modifiedSentence);
				clientSocket.close();
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return response;
	}

	@Override
	protected void onPostExecute(String result) {
		// textView.setText(result);
	}
}
