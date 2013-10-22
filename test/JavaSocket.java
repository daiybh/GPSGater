import java.net.InetSocketAddress;
import java.net.Socket;
import java.io.IOException;
import java.io.UnsupportedEncodingException;  
public class JavaSocket{
	//private static final String IP="192.168.200.101";
	private static final String IP="127.0.0.1";
	private static final int NetPort=120;
	private Socket sock;
	public JavaSocket(){
	}
	public byte[] StringToByteArray(String str){
		byte[] temp=null;
		try{
		temp=str.getBytes("GBK");
		}catch(UnsupportedEncodingException e){
			e.printStackTrace();
		}
		return temp;
	}
	public void Run(){
	try{
	sock = new Socket();
	InetSocketAddress addr = new InetSocketAddress(IP,NetPort);
	sock.connect(addr);
	String SendStr="<?xml version=\"1.0\" encoding=\"utf-8\"?><setarea><id>3</id><sim>14787881482</sim><deviceid>014787881482</deviceid><type>2</type><leftlng>10241.6448144</leftlng><leftlat>2502.8372056</leftlat><riTSCmd>kf</riTSCmd></setarea>";
	SendStr="<?xml version=\"1.0\" encoding=\"utf-8\"?><SetArea><Id>13</Id><Sim>14787831213</Sim><deviceID>014787831213</deviceID><Type>2</Type><LeftLng>10241.2122276</LeftLng><LeftLat>2503.3971772</LeftLat><RightLng>10243.8386466</RightLng><RightLat>2502.4547488</RightLat><AlertType>1</AlertType><areaID>1</areaID></SetArea>";

	sock.getOutputStream().write(StringToByteArray(SendStr));
}catch(IOException e){
	e.printStackTrace();
}
	}
	public static void main(String [] args){
		JavaSocket js=new JavaSocket();
		js.Run();
	}
}