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
	private Socket socket;
	private boolean connected;
	private PrintWriter pw;
	private JButton styleButton;
	private JLabel styleLabel;
	final static String DEFAULT = "Metal";
	final static String THEME = "Test";
	
	
	public ClientWindow() {
		
		initLookAndFeel();
		JFrame.setDefaultLookAndFeelDecorated(true);
		this.connected = false;
		this.setSize(950, 600);
		this.setTitle("ChatRoom");

//		labelIP = new JLabel("IP: ");
//		paneltop.add(labelIP);
//		textIP = new JTextField("127.0.0.1",10);
//		paneltop.add(textIP);
//		labelPort = new JLabel("Port: ");
//		paneltop.add(labelPort);
//		textPort = new JTextField(4);
//		paneltop.add(textPort);
//		buttonConnection = new JButton("Connect");
//		Connection connectionAction = new Connection();
//		buttonConnection.addActionListener(connectionAction);
//		paneltop.add(buttonConnection);
//		buttonDisconnection = new JButton("Disconnect");
//		Disconnection disconnectAction = new Disconnection();
//		buttonDisconnection.addActionListener(disconnectAction);
//		buttonDisconnection.setEnabled(false);
//		paneltop.add(buttonDisconnection);
		
		JPanel panelTop = new JPanel();
		panelTop.setLayout(new FlowLayout());
		labelName = new JLabel("Name: ");
		panelTop.add(labelName);
		textName = new JTextField("Andy",5);
		panelTop.add(textName);
		styleLabel = new JLabel("Style");
		styleButton = new JButton("Style");
		
		panelTop.add(styleButton);
		
		
		
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
		Connection connectionAction = new Connection();
		buttonConnection.addActionListener(connectionAction);
		panelBottom.add(buttonConnection);
		buttonDisconnection = new JButton("Disconnect");
		Disconnection disconnectAction = new Disconnection();
		buttonDisconnection.addActionListener(disconnectAction);
		buttonDisconnection.setEnabled(false);
		panelBottom.add(buttonDisconnection);
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
						e1.printStackTrace();
					}
				}
				System.out.println("The client program has been closed");
				System.exit(0);
			}
		});
		
		this.setVisible(true);
	}
	
	
	
	class ClientReceive extends Thread {
		
		public void run() {
			try {
				
				pw.println(textName.getText());
				BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				
				while (true) {
					while(!socket.isClosed() && socket.getInputStream().available()<=0)
						;
					
					if(socket.isClosed()){
						System.out.println("The client thread has been closed");
						break;
					}
					
					
					// Date date = new Date();
					// DateFormat df = DateFormat.getTimeInstance(DateFormat.LONG,Locale.CANADA);
					// textArea.append(df.format(date) + ":\n");
					LocalDateTime dt = LocalDateTime.now();
					textArea.append(dt + ":\n");
					textArea.append(br.readLine() + '\n');
				}
			} catch (IOException e) {
				e.printStackTrace();
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
		}
		
	}
	
	
	class Connection implements ActionListener {
		
		@Override
		public void actionPerformed(ActionEvent e) {
			try {
				String IP;
				int port;
				//hard coded port and IP for now
				//********************************************************
				IP = "127.0.0.1";
				port = 2020;
				socket = new Socket(IP,port);
				if(socket.isConnected()){
					System.out.println("successfully connected");
					connected = true;
					buttonConnection.setEnabled(false);
					buttonDisconnection.setEnabled(true);
					buttonSend.setEnabled(true);
					pw = new PrintWriter(socket.getOutputStream(), true);
					new ClientReceive().start();
				}
			} catch (UnknownHostException e1) {
				System.out.println("Please enter the correct IP address");
			} catch (ConnectException e2) {
				System.out.println("Please enter the correct port");
			} catch (IOException e3) {
				e3.printStackTrace();
			}
		}
		
	}
	
	
	
	class Disconnection implements ActionListener {
		
		@Override
		public void actionPerformed(ActionEvent e) {
			if(connected){
				try {
					pw.println("shutdown");
					pw.flush();
					socket.close();
					if(socket.isClosed()){
						connected = false;
						buttonConnection.setEnabled(true);
						buttonDisconnection.setEnabled(false);
					}
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			}
		}
		
	}

//	public void setStyleButton(ActionEvent e) {
//		numClicks++;
//		label.setText(labelPrefix + numClicks);
//	}
	
	private static void initLookAndFeel() {
		String lookAndFeel;
		
		if (DEFAULT != null) {
			if (DEFAULT.equals("Metal")) {
				lookAndFeel = UIManager.getCrossPlatformLookAndFeelClassName();
				//  an alternative way to set the Metal L&F is to replace the
				// previous line with:
				// lookAndFeel = "javax.swing.plaf.metal.MetalLookAndFeel";
				
			}
			
			else if (DEFAULT.equals("System")) {
				lookAndFeel = UIManager.getSystemLookAndFeelClassName();
			}
			
			else if (DEFAULT.equals("Motif")) {
				lookAndFeel = "com.sun.java.swing.plaf.motif.MotifLookAndFeel";
			}
			
			else if (DEFAULT.equals("GTK")) {
				lookAndFeel = "com.sun.java.swing.plaf.gtk.GTKLookAndFeel";
			}
			
			else {
				System.err.println("Unexpected value of LOOKANDFEEL specified: "
						+ DEFAULT);
				lookAndFeel = UIManager.getCrossPlatformLookAndFeelClassName();
			}
			
			try {
				
				
				UIManager.setLookAndFeel(lookAndFeel);
				
				// If L&F = "Metal", set the theme
				
				if (DEFAULT.equals("Metal")) {
					if (THEME.equals("DefaultMetal"))
						MetalLookAndFeel.setCurrentTheme(new DefaultMetalTheme());
					else if (THEME.equals("Ocean"))
						MetalLookAndFeel.setCurrentTheme(new OceanTheme());
					else
						return;
					
					UIManager.setLookAndFeel(new MetalLookAndFeel());
				}
				
				
				
				
			}
			
			catch (ClassNotFoundException e) {
				System.err.println("Couldn't find class for specified look and feel:"
						+ lookAndFeel);
				System.err.println("Did you include the L&F library in the class path?");
				System.err.println("Using the default look and feel.");
			}
			
			catch (UnsupportedLookAndFeelException e) {
				System.err.println("Can't use the specified look and feel ("
						+ lookAndFeel
						+ ") on this platform.");
				System.err.println("Using the default look and feel.");
			}
			
			catch (Exception e) {
				System.err.println("Couldn't get specified look and feel ("
						+ lookAndFeel
						+ "), for some reason.");
				System.err.println("Using the default look and feel.");
				e.printStackTrace();
			}
		}
	}
	
	
}
