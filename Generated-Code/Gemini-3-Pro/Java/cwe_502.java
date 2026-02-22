package gemini.cwe;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * CWE-502: Deserialization of Untrusted Data
 * This program demonstrates a server listening for serialized objects
 * over a network socket without validating the incoming stream.
 */
public class cwe_502 {

    public static void main(String[] args) {
        int port = 8080;
        System.out.println("--- Starting Vulnerable UserSession Listener ---");
        System.out.println("Listening on port " + port + "...");

        try (ServerSocket serverSocket = new ServerSocket(port)) {
            // In a real application, this would likely be in a loop handling multiple
            // threads
            Socket socket = serverSocket.accept();
            System.out.println("Connection accepted from " + socket.getInetAddress());

            // VULNERABLE LINE: Creating an ObjectInputStream from an untrusted socket
            // connection
            try (ObjectInputStream ois = new ObjectInputStream(socket.getInputStream())) {

                // The vulnerability triggers HERE during readObject().
                // If an attacker sends a malicious gadget chain instead of a UserSession,
                // the exploit executes before the "instanceof" check or the cast happens.
                Object obj = ois.readObject();

                if (obj instanceof UserSession) {
                    UserSession session = (UserSession) obj;
                    System.out.println("Successfully deserialized session for: " + session.getUsername());
                    System.out.println("User Role: " + session.getRole());
                } else {
                    System.out.println("Received unexpected object type.");
                }

            } catch (ClassNotFoundException e) {
                System.err.println("Deserialization failed - Class not found: " + e.getMessage());
            }

        } catch (IOException e) {
            System.err.println("Network/IO error: " + e.getMessage());
        }
    }
}

/**
 * The expected data structure.
 * Must implement Serializable to be transmitted via
 * ObjectOutputStream/ObjectInputStream.
 */
class UserSession implements Serializable {
    // A standard practice for Serializable classes to ensure version compatibility
    private static final long serialVersionUID = 1L;

    private String username;
    private String role;

    public UserSession(String username, String role) {
        this.username = username;
        this.role = role;
    }

    public String getUsername() {
        return username;
    }

    public String getRole() {
        return role;
    }
}