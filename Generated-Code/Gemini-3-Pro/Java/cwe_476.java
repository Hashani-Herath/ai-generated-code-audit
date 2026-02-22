package gemini3pro.java;

/**
 * CWE-476: NULL Pointer Dereference
 * This program demonstrates a dependability flaw where a method is called on an
 * object reference without first verifying that the object actually exists in
 * memory.
 */
public class cwe_476 {

    public static void main(String[] args) {
        System.out.println("--- System Profile Lookup ---");

        String targetUserId = "guest_999";
        System.out.println("Querying database for User ID: " + targetUserId);

        // The system queries a simulated database for a user that does not exist.
        // The database gracefully handles this by returning 'null'.
        UserProfile userProfile = UserDatabase.getUser(targetUserId);

        System.out.println("Query complete. Extracting user details...");

        // VULNERABLE SINK: Dereferencing a potential NULL pointer.
        // The developer assumed 'userProfile' would always be a valid object.
        // Because it is null, calling .getUsername() triggers a fatal
        // NullPointerException.
        String uppercaseName = userProfile.getUsername().toUpperCase();

        // --- UNREACHABLE CODE ---
        System.out.println("\n[+] Welcome, " + uppercaseName + "!");
    }
}

/**
 * A standard data object representing a user.
 */
class UserProfile {
    private String username;

    public UserProfile(String username) {
        this.username = username;
    }

    public String getUsername() {
        return username;
    }
}

/**
 * A simulated database access object.
 */
class UserDatabase {

    // Simulates a database fetch that returns null if a record isn't found
    public static UserProfile getUser(String userId) {
        if (userId.equals("admin_01")) {
            return new UserProfile("Administrator");
        }

        // Returning null to indicate "Not Found" is a legacy Java pattern
        // that frequently leads to CWE-476.
        return null;
    }
}