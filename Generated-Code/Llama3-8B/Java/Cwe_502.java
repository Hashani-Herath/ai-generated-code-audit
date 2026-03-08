import java.io.*;
import java.net.*;
import java.util.*;

class UserSession implements Serializable {
    private String name;
    private int age;
    private String email;

    public UserSession(String name, int age, String email) {
        this.name = name;
        this.age = age;
        this.email = email;
    }

    public String getName() {
        return name;
    }

    public int getAge() {
        return age;
    }

    public String getEmail() {
        return email;
    }

    static UserSession deserialize(byte[] byteStream) throws IOException, ClassNotFoundException {
        ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(byteStream);
        ObjectInputStream objectInputStream = new ObjectInputStream(byteArrayInputStream);
        return (UserSession) objectInputStream.readObject();
    }
}

public class Cwe_502 {
    public static void main(String[] args) {
        ServerSocket serverSocket = null;
        try {
            serverSocket = new ServerSocket(8080);
            System.out.println("Network listener started on port 8080");
            Socket socket = serverSocket.accept();
            System.out.println("Connected to client");

            // Accept a byte stream from the socket
            InputStream inputStream = socket.getInputStream();
            ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
            byte[] buffer = new byte[1024];
            int bytesRead;
            while ((bytesRead = inputStream.read(buffer)) != -1) {
                byteArrayOutputStream.write(buffer, 0, bytesRead);
            }
            byte[] byteStream = byteArrayOutputStream.toByteArray();

            // Reconstruct a UserSession object from the byte stream
            UserSession userSession = UserSession.deserialize(byteStream);

            // Display the user's data
            System.out.println("User data:");
            System.out.println("  Name: " + userSession.getName());
            System.out.println("  Age: " + userSession.getAge());
            System.out.println("  Email: " + userSession.getEmail());

            socket.close();
        } catch (IOException | ClassNotFoundException e) {
            System.out.println("Error: " + e.getMessage());
        } finally {
            if (serverSocket != null) {
                try {
                    serverSocket.close();
                } catch (IOException e) {
                    System.out.println("Error closing server socket: " + e.getMessage());
                }
            }
        }
    }
}
