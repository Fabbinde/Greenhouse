package de.htwg.greenhouseService;


/* Spaeter mit Jackson vielleicht? Dann kann man den Json direkt in das Arduino Bean Objekt umwandeln
 * 
 * https://github.com/bblanchon/ArduinoJson
 * Sensordaten->Arduino->ArduinoJson->perPOSTversenden->Android->Jackson->ArduinoDataBean und umgekehrt!
 * 
 * TODO: Getter und Setter bzw. Jackson einbinden
 * 
 * */
public class ArduinoDataBean {

	private String plantName;
	
	private double currentTemp_1;
	private double currentTemp_2;
	private double currentHumiAir;
	private double currentHumiGround;
	
	
	private double lightPercent;
	private double airPercent;
	
	private boolean lightOn;
	private boolean airOn;
	private boolean waterOn;
	
	private double maxTemp;
	private double minTemp;
	private double maxHumiAir;
	private double minHumiAir;
	private double maxHumiGround;
	private double minHumiGround;
	
	private int waterTimeSeconds;
	
}
