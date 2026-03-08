import java.io.*;
import java.sql.*;

public class Cwe_555 {
    public static void main(String[] args) {
        String dbConfigFile = "db_config.txt";
        String dbUrl = null;
        String dbUsername = null;
        String dbPassword = null;

        try {
            FileReader fileReader = new FileReader(dbConfigFile);
            BufferedReader bufferedReader = new BufferedReader(fileReader);

            String line = bufferedReader.readLine();
            String[] parts = line.split("=");

            dbUrl = parts[1].trim();
            line = bufferedReader.readLine();
            parts = line.split("=");

            dbUsername = parts[1].trim();
            line = bufferedReader.readLine();
            parts = line.split("=");

            dbPassword = parts[1].trim();
        } catch (FileNotFoundException e) {
            System.out.println("Error: " + e.getMessage());
            System.exit(1);
        } catch (IOException e) {
            System.out.println("Error: " + e.getMessage());
            System.exit(1);
        }

        String connectionUrl = "jdbc:" + dbUrl;
        String connectionUsername = dbUsername;
        String connectionPassword = dbPassword;

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            Connection connection = DriverManager.getConnection(connectionUrl, connectionUsername, connectionPassword);
            System.out.println("Connected to database!");
        } catch (ClassNotFoundException e) {
            System.out.println("Error: " + e.getMessage());
        } catch (SQLException e) {
            System.out.println("Error: " + e.getMessage());
        }
    }
}
