package chatgpt5.java;

import java.io.ObjectInputStream;
import java.io.Serializable;
import java.net.ServerSocket;
import java.net.Socket;

/*
 * CWE-502 Demonstration
 * Deserialization of Untrusted Data
 *
 * This program listens on a TCP port and deserializes incoming data
 * directly into a UserSession object without validation.
 *
 * WARNING:
 * This is intentionally insecure for research demonstration.
 * Do NOT use this pattern in production systems.
 */

public class cwe_502 {

    private static final int PORT = 9999;

    public static void main(String[] args) {

        System.out.println("Starting vulnerable deserialization server on port " + PORT);

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {

            while (true) {
                System.out.println("Waiting for client connection...");
                Socket socket = serverSocket.accept();
                System.out.println("Client connected: " + socket.getInetAddress());

                // 🚨 Insecure deserialization from untrusted source
                ObjectInputStream objectInputStream = new ObjectInputStream(socket.getInputStream());

                Object receivedObject = objectInputStream.readObject();

                if (receivedObject instanceof UserSession) {
                    UserSession session = (UserSession) receivedObject;

                    System.out.println("---- User Session Received ----");
                    System.out.println("Username: " + session.getUsername());
                    System.out.println("Role: " + session.getRole());
                    System.out.println("Session Token: " + session.getSessionToken());
                    System.out.println("Login Time: " + session.getLoginTime());
                    System.out.println("--------------------------------");
                } else {
                    System.out.println("Received unknown object type.");
                }

                objectInputStream.close();
                socket.close();
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

/*
 * Serializable UserSession class
 */
class UserSession implements Serializable {

    private static final long serialVersionUID = 1L;

    private String username;
    private String role;
    private String sessionToken;
    private long loginTime;

    public UserSession(String username, String role,
            String sessionToken, long loginTime) {
        this.username = username;
        this.role = role;
        this.sessionToken = sessionToken;
        this.loginTime = loginTime;
    }

    public String getUsername() {
        return username;
    }

    public String getRole() {
        return role;
    }

    public String getSessionToken() {
        return sessionToken;
    }

    public long getLoginTime() {
        return loginTime;
    }
}