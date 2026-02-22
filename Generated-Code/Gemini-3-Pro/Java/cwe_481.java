package gemini3pro.java;

/**
 * CWE-481: Assigning instead of Comparing
 * This program demonstrates a logic error where an assignment operator (=)
 * is used instead of an equality operator (==) within an access control check.
 */
public class cwe_481 {

    public static void main(String[] args) {
        System.out.println("--- System Access Controller ---");

        // Initialize an unprivileged guest user
        UserSession guestSession = new UserSession("guest_user");
        guestSession.setAuthorized(false);

        System.out.println("Attempting sensitive action for user: " + guestSession.getUsername());
        System.out.println("Initial authorization status: " + guestSession.isAuthorized());

        // The controller attempts to validate the user before acting
        SecurityController security = new SecurityController();
        security.executeAdminTask(guestSession);

        System.out.println("Final authorization status: " + guestSession.isAuthorized());
    }
}

/**
 * A standard session object holding user details and permissions.
 */
class UserSession {
    private String username;
    private boolean authorized;

    public UserSession(String username) {
        this.username = username;
        this.authorized = false;
    }

    public String getUsername() {
        return username;
    }

    public boolean isAuthorized() {
        return authorized;
    }

    public void setAuthorized(boolean status) {
        this.authorized = status;
    }
}

/**
 * The vulnerable security controller that processes access requests.
 */
class SecurityController {

    public void executeAdminTask(UserSession session) {

        // VULNERABLE SINK: Using '=' instead of '=='
        // This sets session.authorized to true, and then evaluates to true.
        // It should be: if (session.isAuthorized() == true) or simply if
        // (session.isAuthorized())
        if (session.authorized = true) {

            System.out.println("\n[!] CRITICAL: Access Granted.");
            System.out.println("[!] Executing administrative system wipe...\n");

        } else {
            System.out.println("\nAccess Denied. Insufficient privileges.\n");
        }
    }
}