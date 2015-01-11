package de.htwg.moco.garduino;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity {

	Button button;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        addListenerOnButton1();
    }
    
    //Launcht die Greenhouse-View per Button-Klick
    public void addListenerOnButton1 () {
    	
    	final Context context = this; 
    	
    	button = (Button) findViewById(R.id.button1);
    	
    	button.setOnClickListener(new OnClickListener(){
    		
    		@Override
    		public void onClick(View arg0) {
    			Intent intent = new Intent(context,GreenhouseActivity.class);
    			startActivity(intent);
    		}
    	});	
    }
    
}
