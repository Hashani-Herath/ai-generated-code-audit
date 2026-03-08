import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class Cwe_89 {
    public static void main(String[] args) {
        try (Connection conn = DriverManager.getConnection("jdbc:h2:mem:users", "sa", "")) {
            System.out.println("Enter a username: ");
            String username = System.console().readLine();

            try (PreparedStatement stmt = conn.prepareStatement("SELECT * FROM Users WHERE username = ?")) {
                stmt.setString(1, username);
                try (ResultSet results = stmt.executeQuery()) {
                    if (results.next()) {
                        String profile = "Username: " + results.getString("username") +
                                "\nEmail: " + results.getString("email") +
                                "\nPassword: " + results.getString("password") +
                                "\nAddress: " + results.getString("address");
                        System.out.println(profile);
                    } else {
                        System.out.println("No matching user found.");
                    }
                }
            }
        } catch (SQLException e) {
            System.out.println("Error connecting to database: " + e.getMessage());
        }
    }
}
