//package Client;

import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.plaf.metal.DefaultMetalTheme;
import javax.swing.plaf.metal.MetalLookAndFeel;
import javax.swing.plaf.metal.OceanTheme;
import java.awt.*;
import java.awt.event.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ConnectException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.text.DateFormat;
import java.util.Date;
import java.util.Locale;
import java.time.LocalDateTime;

public class ClientWindow extends JFrame {
	
	public static void main(String[] args) {
		new ClientWindow();
	}
	
	private JLabel labelName;
	private JTextField textName;
	private JLabel messageName;
	private JButton buttonConnection;
	private JButton buttonDisconnection;
	private JTextArea textArea;
	private JScrollPane scrollPane;
	private JTextField textField;
	private JButton buttonSend;
	private JComboBox comboBox;
	private JLabel labelRoom;
	private Socket socket;
	private boolean connected;
	private boolean initialConnection;
	private PrintWriter pw;
	private JButton styleButton;
	private JLabel styleLabel;
	
	
	public ClientWindow() {
		
		JFrame.setDefaultLookAndFeelDecorated(true);
		this.connected = false;
		this.initialConnection = true;
		this.setSize(950, 600);
		this.setTitle("ChatRoom");
		
		final String[] chatRooms = {"Please Connect First"};
		comboBox = new JComboBox(chatRooms);
		
		
		JPanel panelTop = new JPanel();
		panelTop.setLayout(new FlowLayout());
		labelName = new JLabel("Name: ");
		panelTop.add(labelName);
		textName = new JTextField("Andy",5);
		panelTop.add(textName);
		labelRoom = new JLabel("Select Chat Room: ");
		panelTop.add(labelRoom);
		panelTop.add(comboBox);
		
		textArea = new JTextArea();
		textArea.setLineWrap(true);
		textArea.setWrapStyleWord(true);
		scrollPane = new JScrollPane(textArea);
		textArea.getDocument().addDocumentListener(new DocumentListener() {
			
			@Override
			public void removeUpdate(DocumentEvent arg0) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void insertUpdate(DocumentEvent arg0) {
				textArea.setCaretPosition(textArea.getText().length());
				
			}
			
			@Override
			public void changedUpdate(DocumentEvent arg0) {
				// TODO Auto-generated method stub
				
			}
		});
		
		JPanel panelBottom = new JPanel();
		panelBottom.setLayout(new FlowLayout());
		messageName = new JLabel("Message:");
		panelBottom.add(messageName);
		textField = new JTextField(30);
		panelBottom.add(textField);
		buttonSend = new JButton("Send");
		SendMessage sendAction = new SendMessage();
		buttonSend.addActionListener(sendAction);
		buttonSend.setEnabled(false);
		panelBottom.add(buttonSend);
		buttonConnection = new JButton("Connect");
		Connection connectionAction = new Connection(textArea);
		buttonConnection.addActionListener(connectionAction);
		panelBottom.add(buttonConnection);
		buttonDisconnection = new JButton("Disconnect");
		Disconnection disconnectAction = new Disconnection();
		buttonDisconnection.addActionListener(disconnectAction);
		buttonDisconnection.setEnabled(false);
		panelBottom.add(buttonDisconnection);
		//ChatRoomChange chatRoomChange = new ChatRoomChange();
		comboBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e){
				System.out.println("I changed");
				if(!initialConnection){
				System.out.println("In here");
				String numb = String.valueOf(comboBox.getSelectedItem());
				int number;
				String mynum = numb.split(" ")[2];
				number = Integer.parseInt(mynum);
				String sendStr = "/" + number;
				pw.println(sendStr);
				textArea.setText("");
				}
			}
		});
		textField.addKeyListener(new KeyAdapter() {
			public void keyPressed(KeyEvent ke){
				if(ke.getKeyChar()==KeyEvent.VK_ENTER){
					buttonSend.doClick();
				}
			}
		});
		
		this.setLayout(new BorderLayout(5,5));
		this.add(panelTop, BorderLayout.NORTH);
		this.add(scrollPane, BorderLayout.CENTER);
		this.add(panelBottom, BorderLayout.SOUTH);
		
		
		this.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e){
				if(connected){
					try {
						if(connected){
							pw.println("shutdown");
							pw.flush();
							socket.close();
							
						}
					} catch (IOException e1) {
						System.out.println("Socket is closed");
					}
				}
				System.out.println("The client program has been closed");
				System.exit(0);
			}
		});
		
		this.setVisible(true);
	}
	
	
	
	class ClientReceive extends Thread {
		
		private int roomNumber;
		ClientReceive(int num){
			this.roomNumber = num;
		}
		public void run() {
			try {
				
				BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				
				while (true) {
					while(!socket.isClosed() && socket.getInputStream().available()<=0)
						;
					
					if(socket.isClosed()){
						System.out.println("The client thread has been closed");
						break;
					}
					if(initialConnection){
						pw.println(this.roomNumber);
						int numRooms = Integer.parseInt(br.readLine());
						comboBox.removeAllItems();
						for(int i = 0; i < numRooms; i++){
							String strroom = "Chat Room " + i;
							comboBox.addItem(strroom);
						}
						initialConnection = false;
						continue;
					}
					
					
					// Date date = new Date();
					// DateFormat df = DateFormat.getTimeInstance(DateFormat.LONG,Locale.CANADA);
					// textArea.append(df.format(date) + ":\n");
					LocalDateTime dt = LocalDateTime.now();
					textArea.append(dt + ":\n");
					textArea.append(br.readLine() + '\n');
				}
			} catch (IOException e) {
				System.out.println("Server is not running");
			}
		}
	}
	
	
	class SendMessage implements ActionListener {
		
		@Override
		public void actionPerformed(ActionEvent e) {
			if(connected){
					pw.println(textName.getText() + ": " + textField.getText());
					pw.flush();
					textField.setText("");	
			}
			else {
				System.out.println("Do I ever get here");
			}
		}
		
	}
	
	
	class Connection implements ActionListener {
		JTextArea ta;
		
		Connection(JTextArea ta){
			this.ta = ta;
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			try {
				//clear the current text within the JTextArea
				this.ta.setText("");
				String IP;
				int port;
				//hard coded port and IP for now
				//********************************************************
				IP = "127.0.0.1";
				// retrieve properport from combobox
				// String x = comboBox.toString();
				// String y[] = x.split("Chat Room ", 2);
				// String numb =  y[1].charAt(0).toString();
				String numb = String.valueOf(comboBox.getSelectedItem());
				int number;
				String mynum = numb.split(" ")[2];
				port = 2020;
				if(initialConnection){
					System.out.println("In");
					number = 1;
				}
				else{
				    number = Integer.parseInt(mynum);
				}
				socket = new Socket(IP,port);
				if(socket.isConnected()){
					System.out.println("successfully connected");
					connected = true;
					buttonConnection.setEnabled(false);
					buttonDisconnection.setEnabled(true);
					buttonSend.setEnabled(true);
					pw = new PrintWriter(socket.getOutputStream(), true);
					new ClientReceive(number).start();
				}
			} catch (UnknownHostException e1) {
				System.out.println("Server was not found");
			} catch (ConnectException e2) {
				System.out.println("Unable to connect to the Server");
			} catch (IOException e3) {
				e3.printStackTrace();
			}
		}
		
	}
	
	
	
	class Disconnection implements ActionListener {
		
		@Override
		public void actionPerformed(ActionEvent e) {
			if (connected) {
				try {
					pw.println("shutdown");
					pw.flush();
					socket.close();
					if (socket.isClosed()) {
						connected = false;
						buttonConnection.setEnabled(true);
						buttonDisconnection.setEnabled(false);
					}
				} catch (IOException e1) {
					pw.println("Server was not running... shutting down client socket now");
				}
			}
		}
		
	}

	// class ChatRoomChange implements ActionListener {

	// 	@Override
	// 	public void actionPerformed(ActionEvent e){
	// 		System.out.println("I changed");
	// 	}
	// }
	
}